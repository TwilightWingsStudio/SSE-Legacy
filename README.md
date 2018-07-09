```C
------------------------------------------------------------------------------------------------
|  _______  _______  _______ _________ _______           _______    _______  _______  _______  |
| (  ____ \(  ____ \(  ____ )\__   __/(  ___  )|\     /|(  ____ \  (  ____ \(  ___  )(       ) |
| | (    \/| (    \/| (    )|   ) (   | (   ) || )   ( || (    \/  | (    \/| (   ) || () () | |
| | (_____ | (__    | (____)|   | |   | |   | || |   | || (_____   | (_____ | (___) || || || | |
| (_____  )|  __)   |     __)   | |   | |   | || |   | |(_____  )  (_____  )|  ___  || |(_)| | |
|       ) || (      | (\ (      | |   | |   | || |   | |      ) |        ) || (   ) || |   | | |
| /\____) || (____/\| ) \ \_____) (___| (___) || (___) |/\____) |  /\____) || )   ( || )   ( | |
| \_______)(_______/|/   \__/\_______/(_______)(_______)\_______)  \_______)|/     \||/     \| |
|==============================================================================================|
|    _______________   ____________  .____     ____ ______________.___________    _______      |
|    \_   _____/\   \ /   /\_____  \ |    |   |    |   \__    ___/|   \_____  \   \      \     |
|     |    __)_  \   Y   /  /   |   \|    |   |    |   / |    |   |   |/   |   \  /   |   \    |
|     |        \  \     /  /    |    \    |___|    |  /  |    |   |   /    |    \/    |    \   |
|    /_______  /   \___/   \_______  /_______ \______/   |____|   |___\_______  /\____|__  /   |
|            \/                    \/        \/                               \/         \/    |
------------------------------------------------------------------------------------------------
```
# [SSE] - Serious Sam Evolution
##### Don't confuse with damned Revolution!
========================

This is the unofficial fork of Serious Engine 1.
This fork will be always synchronized with the official repository - https://github.com/Croteam-official/Serious-Engine.

This fork is created for extra features which are focused on improving gameplay stability and adding more opportunities for different kinds of content creation - mapping (with set of new Entities and improved existing ones) and modding (with new functionality added to the engine).

For some reasons these features are not added into Official repository. But I wish that in some day my project will become superior than Official.

Overall changes
---------------
 - Code cleanup and refactor in many places.
 - Engine's filesystem reworked to have more organized view.
 - More than 30 new entities for building maps.
 - Improved master-server support.
   - Now engine supports DarkPlaces protocol. So `dpmaster` open-source master-server program can be used instead of any ugly-coded GameSpy protocol emulators.
 - New gameplay options which you can setup during server startup. (TDM, mutators, etc.)
 - Improved, but not ideal support for widescreen resolutions.
 - Discord Rich Presence integration.
 
========================

Here is Serious Engine v.1.10 source code, including the following projects:

* Commons
  * `DedicatedServer`
  * `Ecc` The *Entity Class Compiler*, a custom build tool used to compile *.es files
  * `Engine` Serious Engine 1.10
  * `EntitiesMP` All the entity logic
  * `GameMP` All the game logic
  * `RCon` Used to connect to servers using an admin password
  * `Shaders` Compiled shaders
  * `SeriousSam` The main game executable
* Editors
  * `GameGUIMP` Common GUI things for game tools
  * `EngineGUI` Common GUI things for game tools
  * `Modeler` Serious Modeler
  * `SeriousSkaStudio` Serious Ska Studio
  * `WorldEditor` Serious Editor
* Tools
  * `DecodeReport` Used to decode crash *.rpt files
  * `Depend` Used to build a list of dependency files based on a list of root files
  * `LWSkaExporter` Exporter for use in LightWave
  * `MakeFONT` Used for generating *.fnt files
* Other
  * `GameAgent` The serverlist masterserver written in Python (requires version 2.X).
  * `libogg`, `libvorbis` Third party libraries used for playing OGG-encoded ingame music (see http://www.vorbis.com/ for more information)

These have been modified to run correctly under the recent version of Windows.
(Tested: Win7 x64, Win8 x64, Win8.1 x64, Win10 x64)

Building
--------

**Setting up workspace.**

Clone the repository into folder.

Do not use spaces, non-latin letters or special symbols such as ()+/[]' in the path to the source code. If you use described symbols/letters, you will have building errors because Visual Studio can not correctly operate with these symbols/letters. But you can use dash "-" and underscore "_" symbols!

**IDE and compilier.**

To build Serious Engine 1, you'll need Visual Studio 2013 or 2015, Professional or Community edition ( https://www.visualstudio.com/post-download-vs?sku=community ).

WARNING: If you will use Visual Studio higher or lower than 2013 then you can encounter bugs. Also game compiled with newer or older version of Visual Studio may be network incompatible with game compiled in VS 2013.

**Building the Lua library.**

Once you've installed Visual Studio and (optionally) DirectX8 SDK.
 - Open Visual Studio Developer Console (you can find it in your Start menu).
 - Command prompt directory should be `/Sources/luajit/src`.
   - Use `cd /d <path>` command to navigate there.
 - Run msvcbuild.bat (located in `/Sources/luajit/src`) to build luajit library.

**Building the engine.**

After that you can build the engine solution (`/Sources/All.sln`). Press F7 or Build -> Build solution. The libraries and executables will be put into `/Bin/` directory (or `/Bin/Debug/` if you are using the Debug configuration).

Optional features
-----------------

DirectX support is disabled by default. If you need DirectX support you'll have to download DirectX8 SDK (headers & libraries) ( http://files.seriouszone.com/download.php?fileid=759 or https://www.microsoft.com/en-us/download/details.aspx?id=6812 ) and then enable the SE1_D3D switch for all projects in the solution (Project properties -> Configuration properties -> C/C++ -> Preprocessor -> Preprocessor definitions -> Add "SE1_D3D" for both Debug and Release builds). You will also need to make sure the DirectX8 headers and libraries are located in the following folders (make the folder structure if it's not existing yet):
* `/Tools.Win32/Libraries/DX8SDK/Include/..`
* `/Tools.Win32/Libraries/DX8SDK/Lib/..`

MP3 playback is disabled by default. If you need this feature, you will have to copy amp11lib.dll to the `/Bin/` directory (and `/Bin/Debug/` for MP3 support in debug mode). The amp11lib.dll is distributed with older versions of Serious Sam: The First Encounter.

3D Exploration support is disabled in the open source version of Serious Engine 1 due to copyright issues. In case if you need to create new models you will have to either use editing tools from any of the original games, or write your own code for 3D object import/export.

IFeel support is disabled in the open source version of Serious Engine 1 due to copyright issues. In case if you need IFeel support you will have to copy IFC22.dll and ImmWrapper.dll from the original game into the `/Bin/` folder.

DiscordRichPresence binaries are not included into repository. You can take compiled binaries here ( https://github.com/discordapp/discord-rpc/releases ), Just put discord-rpc.dll into the `/Bin/` folder.

Running
-------

This version of the engine comes with a set of resources (`/SE1_10.GRO`) that allow you to freely use the engine without any additional resources required. However if you want to open or modify levels from Serious Sam Classic: The First Encounter or The Second Encounter (including most user-made levels), you will have to copy the game's resources (.GRO files) into the engine folder. You can buy the original games on Steam, as a part of a bundle with Serious Sam Revolution ( http://store.steampowered.com/app/227780 )

WARNING: If you will try to run original The First Encounter levels or custom maps made for TFE then you need to resave them in the Serious Editor 1.07/1.10 or you will have not working scripts on levels.

When running a selected project, make sure its project settings on Debugging is set to the right command:
* For debug:
    `$(SolutionDir)..\Bin\Debug\$(TargetName).exe`
* For release:
    `$(SolutionDir)..\Bin\$(TargetName).exe`
* And its working directory:
    `$(SolutionDir)..\`

Common problems
---------------

Before starting the build process, make sure you have a "Temp" folder in your development directory. If it doesn't exist, create it.
SeriousSkaStudio has some issues with MFC windows that can prevent the main window from being displayed properly.

License
-------

Serious Engine is licensed under the GNU GPL v2 (see LICENSE file).

Some of the code included with the engine sources is not licensed under the GNU GPL v2:

* zlib (located in `Sources/Engine/zlib`) by Jean-loup Gailly and Mark Adler
* LightWave SDK (located in `Sources/LWSkaExporter/SDK`) by NewTek Inc.
* libogg/libvorbis (located in `Sources/libogg` and `Sources/libvorbis`) by Xiph.Org Foundation
* LuaJIT (located in `Sources/luajit`) by Mike Pall
