@ECHO OFF
SETLOCAL
IF NOT DEFINED PROJECT_PATH (
	FOR %%i IN (%CD%\..\..) DO (
		SET PROJECT_PATH=%%~fi
		SET KANZI_ENGINE_PATH=%%~fi
	)
)
SET MODULES_FILE=%PROJECT_PATH%\configs\lint\modules.txt
SET BACKSTAGE_MODULES_FILE=%PROJECT_PATH%\configs\lint\modules_backstage.txt
IF "%1"=="" (
	ECHO Usage:	run_lint.bat ^<module_name^> [module2_name ...]
	ECHO       	Where module_name is one of the modules listed in "%MODULES_FILE%" or "%BACKSTAGE_MODULES_FILE%"
	GOTO :end
)

:loop
IF NOT "%1"=="" (
	FOR /F "eol=# tokens=1,2,3,4 usebackq" %%a IN ("%MODULES_FILE%" "%BACKSTAGE_MODULES_FILE%") DO (
		IF /I "%%b"=="%1" (
			CALL run_lint_module "%KANZI_ENGINE_PATH%" "%PROJECT_PATH%" %%a %%b %%c %%d
			goto :success
		)
	)
	
	ECHO Error: Module "%1" not found from "%MODULES_FILE%" or "%BACKSTAGE_MODULES_FILE%"
	GOTO :end

	:success
	SHIFT
	GOTO :loop
)

COPY /Y "%KANZI_ENGINE_PATH%\configs\lint\lint.xsl" "%PROJECT_PATH%\output\lint\" >nul

:end
ENDLOCAL
