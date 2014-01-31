CPP_PATH=../cpp/src
ENGINE_PATH=../../..

SOURCES_PATH=$ENGINE_PATH/sources

#WRAPPER_OUTPUT_PATH=$ENGINE_PATH/configs/platforms/android/developer_foundation/jni
JAVA_SOURCES_PATH=$SOURCES_PATH/developer_foundation/java/com/rightware/kanzi/
WRAPPER_OUTPUT_PATH=$SOURCES_PATH/developer_foundation/java/

swig -java -verbose -package com.rightware.kanzi -c++ -outdir $JAVA_SOURCES_PATH -includeall -I$CPP_PATH -I$SOURCES_PATH/system_layer/common/src -I$SOURCES_PATH/core_layer/src -I$SOURCES_PATH/user_layer/src -o $WRAPPER_OUTPUT_PATH/kanzi_wrap.cpp $ENGINE_PATH/configs/swig/developer_foundation.i
