@ECHO OFF
SETLOCAL
IF NOT DEFINED PROJECT_PATH (
	FOR %%i IN (%CD%\..\..) DO (
		SET PROJECT_PATH=%%~fi
		SET KANZI_ENGINE_PATH=%%~fi
	)
)
FOR %%i IN (%1) DO SET MODULE_NAME=%%~ni
CALL run_lint %MODULE_NAME%
START "" "%PROJECT_PATH%\Output\lint\%MODULE_NAME%.xml"
ENDLOCAL