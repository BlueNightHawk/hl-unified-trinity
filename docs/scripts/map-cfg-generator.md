# MapCfgGenerator

> **Note**
> This is a [dotnet script tool](/docs/tutorials/setting-up-and-using-dotnet-script.md)

> **Note**
> This tool is included in the `scripts` directory in the [assets repository](https://github.com/SamVanheer/halflife-unified-sdk-assets)

The MapCfgGenerator script is used to generate map configuration files for official Half-Life, Opposing Force and Blue Shift maps.
These files all have the same contents for a particular game.

## Usage

### Command line

```
Usage:
  dotnet script "path/to/MapCfgGenerator.csx" <destination-directory> [options]

Arguments:
  <destination-directory>  Where to put the generated files

Options:
  --version       Show version information
  -?, -h, --help  Show help and usage information
```

The destination directory should point to the `cfgsrc/maps` directory in your local copy of the [assets repository](https://github.com/SamVanheer/halflife-unified-sdk-assets).

## Error reporting

If any errors occur the script will log it to the console.

## Sample configuration file contents

Possible contents for a Half-Life map are:
```jsonc
{
	"Includes": [
		"cfg/HalfLifeConfig.json"
	]
}
```

The included file depends on the game the map is a part of.