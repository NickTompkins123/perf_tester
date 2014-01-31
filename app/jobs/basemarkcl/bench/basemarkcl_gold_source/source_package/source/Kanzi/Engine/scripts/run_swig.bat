@ECHO OFF
SETLOCAL
CD ..\tools\thirdparty\swig
SET ENGINE_PATH=..\..\..
IF NOT EXIST %ENGINE_PATH%\output\swig MKDIR %ENGINE_PATH%\output\swig
IF NOT EXIST %ENGINE_PATH%\output\swig\java MKDIR %ENGINE_PATH%\output\swig\java
SET SOURCES_PATH=%ENGINE_PATH%\sources
REM swig -java -verbose -includeall -I%SOURCES_PATH%\system_layer\common\src -I%SOURCES_PATH%\core_layer\src -I%SOURCES_PATH%\user_layer\src -o %ENGINE_PATH%\output\swig\Kanzi.c -outdir %ENGINE_PATH%\output\swig\java %ENGINE_PATH%\configs\swig\Kanzi.i
swig -java -verbose -o %ENGINE_PATH%\output\swig\Kanzi.c -outdir %ENGINE_PATH%\output\swig\java %ENGINE_PATH%\configs\swig\Kanzi.i
ENDLOCAL