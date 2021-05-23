@echo off

set CFLAGS=/O2 /std:c17 /utf-8 /analyze /Wall /sdl
set INCLUDES=/I SDL2\include /I SDL2_ttf\include /I .
set LIBS=SDL2\lib\x64\SDL2.lib SDL2\lib\x64\SDL2main.lib SDL2_ttf\lib\x64\SDL2_ttf.lib Shell32.lib

mkdir out

rc.exe ressource.rc
cl.exe %CFLAGS% %INCLUDES% p4.c main.c /link %LIBS% ressource.res -SUBSYSTEM:windows /out:out/main.exe

copy *.ttf out
copy SDL2\lib\x64\*.dll out
copy SDL2_ttf\lib\x64\*.dll out
copy SDL2\README-SDL.txt out
