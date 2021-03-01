@echo off

set CFLAGS=/O3
set INCLUDES=/I SDL2\include SDL2_ttf\include
set LIBS=SDL2\lib\x64\SDL2.lib SDL2\lib\x64\SDL2main.lib SDL2_ttf\lib\x64\SDL2_ttf.lib Shell32.lib

cl.exe %CXXFLAGS% %INCLUDES% main.c /link %LIBS% -SUBSYSTEM:windows
