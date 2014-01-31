@ECHO OFF
SETLOCAL
IF NOT DEFINED PROJECT_PATH (
	FOR %%i IN (%CD%\..\..) DO (
		SET PROJECT_PATH=%%~fi
		SET KANZI_ENGINE_PATH=%%~fi
	)
)
SET MODULES_FILE=%PROJECT_PATH%\configs\lint\modules.txt
ECHO Running PC-lint for whole project (all modules in "%MODULES_FILE%"):

FOR /F "eol=# tokens=1,2,3,4" %%a IN (%MODULES_FILE%) DO (
	CALL run_lint_module %KANZI_ENGINE_PATH% %PROJECT_PATH% %%a %%b %%c %%d %1
)

COPY /Y %KANZI_ENGINE_PATH%\configs\lint\lint.xsl %PROJECT_PATH%\output\lint\ >nul
ENDLOCAL