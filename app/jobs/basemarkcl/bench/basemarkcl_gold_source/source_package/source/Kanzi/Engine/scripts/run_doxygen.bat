@ECHO off
SETLOCAL

PUSHD ..

ECHO [DEBUG] SET TARGET=%1
SET TARGET=%1

IF NOT DEFINED TARGET (
    ECHO Usage: run_doxygen ^<target^>, where ^<target^> is either "api" or "developer" (without quotation marks^).
    GOTO fail
)

FOR %%i IN (system core user framework) DO (
    ECHO Generating documentation for %%i
    IF EXIST docs\%TARGET%\%%i (
        ECHO [DEBUG] DEL /Q /S docs\%TARGET%\%%i\*
        DEL /Q /S docs\%TARGET%\%%i\*
    ) ELSE (
        ECHO [DEBUG] MKDIR docs\%TARGET%\%%i
        MKDIR docs\%TARGET%\%%i
    )
    
    ECHO [DEBUG] tools\thirdparty\graphviz\bin\dot.exe -c
    ECHO [DEBUG] tools\thirdparty\doxygen\doxygen.exe configs\doxygen\doxygen_%%i.conf 2> docs\%TARGET%\%%i\warnings.txt   
    tools\thirdparty\graphviz\bin\dot.exe -c
    tools\thirdparty\doxygen\doxygen.exe configs\doxygen\doxygen_%%i.conf 2> docs\%TARGET%\%%i\warnings.txt
    
    IF %ERRORLEVEL% NEQ 0 (
        ECHO Doxygen for %%i failed.
        GOTO fail
    )
)

GOTO success

:fail
ECHO ERROR: Failed to generate some documentations
GOTO end

:success
ECHO All documentations succesfully generated
GOTO end

:end

POPD

ENDLOCAL