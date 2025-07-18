# Steam Test

Test Steam integration with [Castle Game Engine](https://castle-engine.io/). This is a ready application that connects to Steam and allows to report achievements.

Follow the documentation about [Steam and Castle Game Engine](https://castle-engine.io/steam) to set up the Steam dynamic library for this project. If you don't do this, then the integration will do nothing -- the game will still run, but without the Steam dynamic library present we cannot "talk" with Steam.

![Screenshot](screenshot.png)

Using [Castle Game Engine](https://castle-engine.io/).

## Building

Compile by:

- [CGE editor](https://castle-engine.io/editor). Just use menu items _"Compile"_ or _"Compile And Run"_.

- Or use [CGE command-line build tool](https://castle-engine.io/build_tool). Run `castle-engine compile` in this directory.

- Or use [Lazarus](https://www.lazarus-ide.org/). Open in Lazarus `steam_test_standalone.lpi` file and compile / run from Lazarus. Make sure to first register [CGE Lazarus packages](https://castle-engine.io/lazarus).

- Or use [Delphi](https://www.embarcadero.com/products/Delphi). Open in Delphi `steam_test_standalone.dproj` file and compile / run from Delphi. See [CGE and Delphi](https://castle-engine.io/delphi) documentation for details.
