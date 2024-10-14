#!/bin/bash

# fetch SDL2 DLL releases.
mkdir sdl2-dll
cd sdl2-dll
wget https://github.com/libsdl-org/SDL/releases/download/release-2.30.1/SDL2-2.30.1-win32-x64.zip

# unzip downloaded zips to get DLLs.
unzip -o SDL2-2.30.1-win32-x64.zip
cd ..

# apply patches to the actual project itself.
cd ..
git apply windows-build/build.patch
cp windows-build/sdl2-dll/*.dll .
