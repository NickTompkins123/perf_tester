SET ENGINE_PATH=..\..\..
SET SOURCES_PATH=%ENGINE_PATH%\sources
SET JAVA_SOURCES_PATH=%SOURCES_PATH%\developer_foundation\java

SET OUTPUT_PATH=%ENGINE_PATH%\output\

set CLASSPATH=%JAVA_SOURCES_PATH%
javac *.java

copy %OUTPUT_PATH%\Win32\ES2_IMG_Debug\*.dll .

java runme
