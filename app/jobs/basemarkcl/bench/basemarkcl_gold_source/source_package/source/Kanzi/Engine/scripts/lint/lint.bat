@ECHO OFF
SET LINT_HOME=%~dp0..\..\tools\thirdparty\pc-lint
"%LINT_HOME%\lint-nt" -i"%LINT_HOME%" std.lnt %*