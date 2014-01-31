@ECHO OFF
cd Kanzi\Engine\scripts\lint\
call run_lint_all.bat
call python hudsonize_lint_xml.py %WORKSPACE% Kanzi\Engine\output\lint
dir %WORKSPACE%\Kanzi\Engine\output\lint