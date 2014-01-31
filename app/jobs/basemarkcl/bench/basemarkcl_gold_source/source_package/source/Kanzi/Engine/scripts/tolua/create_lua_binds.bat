@ECHO OFF
ECHO.
ECHO Creating C to Lua files..

PUSHD .
CD tolua

SET TARGET=c_to_lua

FOR /f "delims=" %%? IN ('DIR /b pkg') DO (
   ECHO Creating bindings for %%~n?.pkg
   tolua++.exe -S -H %TARGET%\lua_%%~n?.h  -o %TARGET%\lua_%%~n?.c pkg\%%~n?.pkg
)

ECHO.
ECHO Bindings created. Files are in engine\scripts\tolua\c_to_lua folder.
POPD
