SET WRAPPER_PATH=..\cpp
SET ENGINE_PATH=..\..\..

SET SOURCES_PATH=%ENGINE_PATH%\sources
SET WRAPPER_SOURCES_PATH=%WRAPPER_PATH%
SET JAVA_SOURCES_PATH=%SOURCES_PATH%\developer_foundation\java

swig -java -verbose -c++ -outdir %JAVA_SOURCES_PATH% -includeall -I%WRAPPER_SOURCES_PATH% -I%SOURCES_PATH%\system_layer\common\src -I%SOURCES_PATH%\core_layer\src -I%SOURCES_PATH%\user_layer\src -o %WRAPPER_SOURCES_PATH%\kanzi_wrap.cxx %ENGINE_PATH%\configs\swig\developer_foundation.i
