@echo off

set CFLAGS=/O3
set INCLUDES=/I SDL2\include /I SDL2_ttf\include /I .
set LIBS=SDL2\lib\x64\SDL2.lib SDL2\lib\x64\SDL2main.lib SDL2_ttf\lib\x64\SDL2_ttf.lib Shell32.lib

cl.exe %CXXFLAGS% %INCLUDES% p4.c main.c /link %LIBS% -SUBSYSTEM:windows
