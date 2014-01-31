@ECHO OFF
ECHO.
ECHO Preparing directories..
ECHO.

PUSHD .

CD tolua

IF EXIST pkg (
    DEL /Q /S pkg\*
) ELSE (
    MKDIR pkg
)

IF EXIST c_to_lua (
    DEL /Q /S c_to_lua\*
) ELSE (
    MKDIR c_to_lua
)

POPD