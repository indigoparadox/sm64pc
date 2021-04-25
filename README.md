# sm64pc-python
Fork of [sm64-port/sm64-port](https://github.com/sm64-port/sm64-port) with additional features. 

Feel free to report bugs and contribute, but remember, there must be **no upload of any copyrighted asset**. 
Run `./extract_assets.py --clean && make clean` or `make distclean` to remove ROM-originated content.

Please contribute **first** to the [nightly branch](https://github.com/sm64pc/sm64ex/tree/nightly/). New functionality will be merged to master once they're considered to be well-tested.

*Read this in other languages: [Español](README_es_ES.md), [Português](README_pt_BR.md) or [简体中文](README_zh_CN.md).*

## Python Support

This is experimental and highly fraught/incomplete at the moment.

Just make sure to run the compiled binary from the directory where sm64.py is located. Some functionality has been moved to python, so things will not work without the python module present.

Currently buggy as heck and, again, incomplete.

## Compiling Under Windows

The simplest way to do this is with mingw32-x86\_64 and cmake.

Install the following packages:

 * pacman -S mingw64/mingw-w64-x86_64-python
 * pacman -S mingw64/mingw-w64-x86_64-SDL2
 * pacman -S mingw64/mingw-w64-x86_64-glew
 * pacman -S mingw64/mingw-w64-x86_64-mesa

Then change to the project directory and:

 * mkdir build
 * cd build

Initialize cmake and compile by doing:

 * cmake .. -G "MinGW Makefiles"
 * mingw32-make.exe

The resulting binary should be placed in a directory with some DLLs from mingw in order to function:

 * mkdir sm64
 * cp ../sm64.py sm64
 * cp /mingw64/bin/{SDL2.dll,libpython3.8.dll,libwinpthread-1.dll,glew32.dll,zlib1.dll} .
 * cp -R /mingw64/lib/python3.8 .
 * cp ../tools/sm64.bat .

The result should be runnable by executing sm64.bat.

## New features

 * Options menu with various settings, including button remapping.
 * Optional external data loading (so far only textures and assembled soundbanks), providing support for custom texture packs.
 * Optional analog camera and mouse look (using [Puppycam](https://github.com/FazanaJ/puppycam)).
 * Optional OpenGL1.3-based renderer for older machines, as well as the original GL2.1, D3D11 and D3D12 renderers from Emill's [n64-fast3d-engine](https://github.com/Emill/n64-fast3d-engine/).
 * Option to disable drawing distances.
 * Optional model and texture fixes (e.g. the smoke texture).
 * Skip introductory Peach & Lakitu cutscenes with the `--skip-intro` CLI option
 * Cheats menu in Options (activate with `--cheats` or by pressing L thrice in the pause menu).
 * Support for both little-endian and big-endian save files (meaning you can use save files from both sm64-port and most emulators), as well as an optional text-based save format.

Recent changes in Nightly have moved the save and configuration file path to `%HOMEPATH%\AppData\Roaming\sm64pc` on Windows and `$HOME/.local/share/sm64pc` on Linux. This behaviour can be changed with the `--savepath` CLI option.
For example `--savepath .` will read saves from the current directory (which not always matches the exe directory, but most of the time it does);
   `--savepath '!'` will read saves from the executable directory.

## Building
For building instructions, please refer to the [wiki](https://github.com/sm64pc/sm64ex/wiki).

**Make sure you have MXE first before attempting to compile for Windows on Linux and WSL. Follow the guide on the wiki.**
