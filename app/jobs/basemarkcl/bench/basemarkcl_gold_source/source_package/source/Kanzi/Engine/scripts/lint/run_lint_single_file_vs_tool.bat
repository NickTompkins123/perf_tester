@ECHO OFF
SETLOCAL
IF NOT DEFINED PROJECT_PATH (
	FOR %%i IN (%CD%\..\..) DO (
		SET PROJECT_PATH=%%~fi
		SET KANZI_ENGINE_PATH=%%~fi
	)
)
FOR %%i IN (%1) DO (
	SET MODULE_NAME=%%~ni
)
FOR %%i IN (%2) DO (
	SET EXTENSION=%%~xi
)

SET LINT_SCRIPTS_PATH=%~dp0
SET MODULES_FILE=%PROJECT_PATH%\configs\lint\modules.txt
SET BACKSTAGE_MODULES_FILE=%PROJECT_PATH%\configs\lint\modules_backstage.txt
FOR /F "eol=# tokens=1,2,3,4 usebackq" %%a IN ("%MODULES_FILE%" "%BACKSTAGE_MODULES_FILE%") DO (
	IF /I "%%b"=="%MODULE_NAME%" (
		CALL :lint-module %2 %%b %%c %%d
		goto :end
	)
)

ECHO Module "%MODULE_NAME%" not found
GOTO :end

:lint-module
REM Parameters:
REM %1: <file-name>
REM %2: <name>
REM %3: <configuration.lnt>
REM %4: <source-path>

PUSHD "%KANZI_ENGINE_PATH%"

REM Setup
SET FILENAME=%1
SET NAME=%2
SET CONFIG=%3
SET SOURCE_PATH=%PROJECT_PATH%\%4\src

ECHO Running PC-lint for file %FILENAME% (module "%NAME%")

IF "%EXTENSION%"==".h" SET H_LNT=h.lnt
CALL %LINT_SCRIPTS_PATH%\lint -i"%KANZI_ENGINE_PATH%\configs\lint" -i"%SOURCE_PATH%" kanzi.lnt %H_LNT% vs.lnt -b %CONFIG% -u %FILENAME%
ECHO PC-Lint succeeded - %ERRORLEVEL% issues

:end
POPD
ENDLOCAL