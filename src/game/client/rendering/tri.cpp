//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

// Triangle rendering, if any

#include "hud.h"

// Triangle rendering apis are in gEngfuncs.pTriAPI

#include "const.h"
#include "entity_state.h"
#include "cl_entity.h"
#include "triangleapi.h"
#include "Exports.h"

#include "particleman.h"
#include "tri.h"
#include "view.h"

// RENDERERS START
#include "bsprenderer.h"
#include "propmanager.h"
#include "particle_engine.h"
#include "watershader.h"
#include "mirrormanager.h"

#include "studio.h"
#include "StudioModelRenderer.h"
#include "GameStudioModelRenderer.h"

extern CGameStudioModelRenderer g_StudioRenderer;
// RENDERERS END

#if 0
void RenderFog()
{
	if (g_WaterLevel <= WaterLevel::Feet && g_FogDensity >= 0)
	{
		gEngfuncs.pTriAPI->FogParams(g_FogDensity, g_FogSkybox);
		gEngfuncs.pTriAPI->Fog(g_FogColor, g_FogStartDistance, g_FogStopDistance, int(g_RenderFog));
	}
}
#endif
/**
 *	@brief Non-transparent triangles-- add them here
 */
void DLLEXPORT HUD_DrawNormalTriangles()
{
	// RENDERERS START
	// 2012-02-25
	R_DrawNormalTriangles();
	// RENDERERS END

	gHUD.m_Spectator.DrawOverview();
}

/**
 *	@brief Render any triangles with transparent rendermode needs here
 */
void DLLEXPORT HUD_DrawTransparentTriangles()
{
	// RENDERERS START
	// 2012-02-25
	R_DrawTransparentTriangles();
	// RENDERERS END

	if (g_pParticleMan)
		g_pParticleMan->Update();

	// Handled in V_CalcRefdef.
	// RenderFog();
}
