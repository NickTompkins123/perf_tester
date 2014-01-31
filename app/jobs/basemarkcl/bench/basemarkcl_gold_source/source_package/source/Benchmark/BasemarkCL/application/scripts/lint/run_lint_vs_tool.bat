@ECHO OFF
SETLOCAL
FOR %%i IN (%CD%\..\..) DO SET PROJECT_PATH=%%~fi
FOR %%i IN (%PROJECT_PATH%\..\..\..\Kanzi\Engine) DO SET KANZI_ENGINE_PATH=%%~fi
PUSHD %KANZI_ENGINE_PATH%\scripts\lint
CALL run_lint_vs_tool %1
POPD
ENDLOCAL
