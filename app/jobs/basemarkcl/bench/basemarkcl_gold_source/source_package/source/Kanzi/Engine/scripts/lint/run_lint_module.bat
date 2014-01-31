@ECHO OFF
SETLOCAL
IF "%6"=="" (
	ECHO Invalid arguments for batch file
	GOTO :EOF
)
REM Parameters:
REM %~1: <kanzi-engine-path>
REM %~2: <project-path>
REM %3: <library|program>
REM %4: <name>
REM %5: <configuration.lnt>
REM %~6: <source-path>
REM %7: [optional lint parameter]

PUSHD %KANZI_ENGINE_PATH%

REM Setup
SET KANZI_ENGINE_PATH=%~1
SET PROJECT_PATH=%~2
SET NAME=%4
SET CONFIG=%5
SET SOURCE_PATH=%PROJECT_PATH%\%~6\src
SET LINT_SCRIPTS_PATH=%~dp0

FOR %%i IN ("%SOURCE_PATH%") DO (
	SET REMOVE_DIR=%%~fi\
)
SET OPTIONAL_LINT_PARAMETER=%7

ECHO Running PC-lint for module "%NAME%"

SET OUTPUT=%PROJECT_PATH%\output\lint
IF NOT EXIST "%OUTPUT%" MKDIR "%OUTPUT%"
SET OUTFILE=%OUTPUT%\%NAME%.xml

IF %3==library (
	CALL :lint-library %CONFIG%
	GOTO :end
) ELSE IF %3==program (
	CALL :lint-program %CONFIG%
	GOTO :end
) ELSE (
	ECHO Invalid lint configuration type: %3
	GOTO :end
)

:lint-files
FOR /R "%SOURCE_PATH%" %%i IN (*.c) DO CALL :lint %1 "%%i"
FOR /R "%SOURCE_PATH%" %%i IN (*.h) DO CALL :lint %1 "%%i" h.lnt
GOTO :EOF

:lint
SET FILENAME=%~2
CALL SET "FILENAME=%%FILENAME:%REMOVE_DIR%=%%"

ECHO 	^<module name="%FILENAME%"^> >> "%OUTFILE%"
CALL %LINT_SCRIPTS_PATH%\lint -i"%KANZI_ENGINE_PATH%\configs\lint" -i"%SOURCE_PATH%" +"os(%OUTFILE%)" kanzi.lnt %3 xml.lnt -zero -b %1 %OPTIONAL_LINT_PARAMETER% -u %FILENAME%
ECHO 	^</module^> >> "%OUTFILE%"
ECHO. >> "%OUTFILE%"
SET FILENAME=
GOTO :EOF

:lint-library
ECHO ^<?xml version="1.0" ?^> > "%OUTFILE%"
ECHO ^<?xml-stylesheet type="text/xsl" href="lint.xsl"?^> >> "%OUTFILE%"
ECHO ^<lint name="%NAME%"^> >> "%OUTFILE%"
CALL :lint-files %1
ECHO ^</lint^> >> "%OUTFILE%"
GOTO :EOF

:lint-program
SET LINTFILE=%OUTPUT%\%NAME%.lnt
ECHO. > "%LINTFILE%"
FOR /R "%SOURCE_PATH%" %%i IN (*.c) DO CALL :add-file "%%i"
GOTO :continue
:add-file
SET FILENAME=%~1
CALL SET "FILENAME=%%FILENAME:%REMOVE_DIR%=%%"
ECHO %FILENAME% >> "%LINTFILE%"
SET FILENAME=
GOTO :EOF
:continue

IF EXIST "%OUTFILE%" DEL "%OUTFILE%"
ECHO ^<?xml version="1.0" ?^> >> "%OUTFILE%"
ECHO ^<?xml-stylesheet type="text/xsl" href="lint.xsl"?^> >> "%OUTFILE%"
ECHO ^<lint name="%NAME%"^> >> "%OUTFILE%"
CALL :lint-files %1
ECHO 	^<inter-module^> >> "%OUTFILE%"
CALL %LINT_SCRIPTS_PATH%\lint -i"%KANZI_ENGINE_PATH%\configs\lint" -i"%SOURCE_PATH%" +"os(%OUTFILE%)" kanzi.lnt xml.lnt -zero -b %1 intermodule.lnt %OPTIONAL_LINT_PARAMETER% "%LINTFILE%"
DEL "%LINTFILE%"
ECHO 	^</inter-module^> >> "%OUTFILE%"
ECHO ^</lint^> >> "%OUTFILE%"
SET LINTFILE=
GOTO :EOF

:end

POPD
ENDLOCAL
