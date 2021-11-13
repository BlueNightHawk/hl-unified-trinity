/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#include <regex>
#include <stdexcept>
#include <string_view>

#include <spdlog/fmt/fmt.h>

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CServerLibrary.h"

#include "config/GameConfigDefinition.h"
#include "config/GameConfigLoader.h"
#include "config/sections/CommandsSection.h"
#include "config/sections/EchoSection.h"

using namespace std::literals;

constexpr const char* const MapConfigCommandWhitelistFileName = "cfg/MapConfigCommandWhitelist.json";
const std::regex MapConfigCommandWhitelistRegex{"^[\\w]+$"};

cvar_t servercfgfile = {"sv_servercfgfile", "cfg/server/server.json", FCVAR_NOEXTRAWHITEPACE | FCVAR_ISPATH};
cvar_t mapchangecfgfile = {"sv_mapchangecfgfile", "", FCVAR_NOEXTRAWHITEPACE | FCVAR_ISPATH};

static void AddCommonConfigSections(std::vector<std::unique_ptr<const GameConfigSection>>& sections)
{
	//Always add this
	sections.push_back(std::make_unique<EchoSection>());
}

CServerLibrary::CServerLibrary() = default;
CServerLibrary::~CServerLibrary() = default;

bool CServerLibrary::Initialize()
{
	if (!InitializeCommon())
	{
		return false;
	}

	g_engfuncs.pfnCVarRegister(&servercfgfile);
	g_engfuncs.pfnCVarRegister(&mapchangecfgfile);

	g_JSON.RegisterSchema("MapConfigCommandWhitelist", &CServerLibrary::GetMapConfigCommandWhitelistSchema);

	try
	{
		auto whitelistSchema = GetMapConfigCommandWhitelistSchema();

		if (whitelistSchema.is_null())
		{
			return false;
		}

		m_MapConfigCommandWhitelistValidator.set_root_schema(std::move(whitelistSchema));
		CreateConfigDefinitions();
	}
	catch (const std::invalid_argument& e)
	{
		g_GameConfigLoader.GetLogger()->error("{}", e.what());
		return false;
	}

	return true;
}

void CServerLibrary::Shutdown()
{
	m_MapConfigDefinition.reset();
	m_MapChangeConfigDefinition.reset();
	m_ServerConfigDefinition.reset();

	ShutdownCommon();
}

void CServerLibrary::NewMapStarted(bool loadGame)
{
	ClearStringPool();

	//Initialize map state to its default state
	m_MapState = CMapState{};

	//Load the config files, which will initialize the map state as needed
	LoadServerConfigFiles();
}

void CServerLibrary::PreMapActivate()
{
}

void CServerLibrary::PostMapActivate()
{
	LoadMapChangeConfigFile();
}

void CServerLibrary::CreateConfigDefinitions()
{
	m_ServerConfigDefinition = g_GameConfigLoader.CreateDefinition("ServerGameConfig", []()
		{
			std::vector<std::unique_ptr<const GameConfigSection>> sections;

			AddCommonConfigSections(sections);
			sections.push_back(std::make_unique<CommandsSection>());

			return sections;
		}());

	m_MapConfigDefinition = g_GameConfigLoader.CreateDefinition("MapGameConfig", [this]()
		{
			std::vector<std::unique_ptr<const GameConfigSection>> sections;

			AddCommonConfigSections(sections);
			sections.push_back(std::make_unique<CommandsSection>(GetMapConfigCommandWhitelist()));

			return sections;
		}());

	m_MapChangeConfigDefinition = g_GameConfigLoader.CreateDefinition("MapChangeGameConfig", []()
		{
			std::vector<std::unique_ptr<const GameConfigSection>> sections;

			//Limit the map change config to commands only, configuration should be handled by other cfg files
			AddCommonConfigSections(sections);
			sections.push_back(std::make_unique<CommandsSection>());

			return sections;
		}());
}

void CServerLibrary::LoadConfigFile(const char* fileName, const GameConfigDefinition& definition, const GameConfigLoadParameters& parameters)
{
	if (const auto config = g_GameConfigLoader.TryLoad(fileName, definition, parameters); config.has_value())
	{
		//Configuration will apply to the map state object
		config->Apply(&m_MapState);
	}
}

void CServerLibrary::LoadServerConfigFiles()
{
	if (const auto cfgFile = servercfgfile.string; cfgFile && cfgFile[0])
	{
		LoadConfigFile(cfgFile, *m_ServerConfigDefinition, {.PathID = "GAMECONFIG"});
	}

	//Check if the file exists so we don't get errors about it during loading
	if (const auto mapCfgFileName = fmt::format("cfg/maps/{}.json", STRING(gpGlobals->mapname));
		g_pFileSystem->FileExists(mapCfgFileName.c_str()))
	{
		LoadConfigFile(mapCfgFileName.c_str(), *m_MapConfigDefinition);
	}
}

void CServerLibrary::LoadMapChangeConfigFile()
{
	if (const auto cfgFile = mapchangecfgfile.string; cfgFile && cfgFile[0])
	{
		LoadConfigFile(cfgFile, *m_MapChangeConfigDefinition, {.PathID = "GAMECONFIG"});
	}
}

json CServerLibrary::GetMapConfigCommandWhitelistSchema()
{
	auto schema = fmt::format(R"(
{{
	"$schema": "http://json-schema.org/draft-07/schema#",
	"title": "Map Configuration Command Whitelist",
	"type": "array",
	"items": {{
		"title": "Command Name",
		"type": "string",
		"pattern": "^[\\w]+$"
	}}
}}
)");

	return g_JSON.ParseJSONSchema(schema).value_or(json{});
}

std::unordered_set<std::string> CServerLibrary::GetMapConfigCommandWhitelist()
{
	//Load the whitelist from a file
	auto whitelist = g_JSON.ParseJSONFile(
		MapConfigCommandWhitelistFileName,
		m_MapConfigCommandWhitelistValidator,
		[](const json& input)
		{
			std::unordered_set<std::string> list;

			if (input.is_array())
			{
				list.reserve(input.size());

				for (const auto& element : input)
				{
					auto command = element.get<std::string>();

					if (std::regex_match(command, MapConfigCommandWhitelistRegex))
					{
						if (!list.insert(std::move(command)).second)
						{
							g_GameConfigLoader.GetLogger()->debug("Whitelist command \"{}\" encountered more than once", command);
						}
					}
					else
					{
						g_GameConfigLoader.GetLogger()->warn("Whitelist command \"{}\" has invalid syntax, ignoring", command);
					}
				}
			}

			return list;
		},
		"GAMECONFIG");

	return whitelist.value_or(std::unordered_set<std::string>{});
}
