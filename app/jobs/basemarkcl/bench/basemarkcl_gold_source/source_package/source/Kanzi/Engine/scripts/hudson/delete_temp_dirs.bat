REM ----====[ Deletes directory with wildcard matching ]====----
@ECHO OFF
SET PATTERN=%USERNAME%_%COMPUTERNAME%
CALL powershell.exe "get-childitem -path '.' -filter %PATTERN%* | remove-item -force -recurse"