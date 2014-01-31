LOCAL_PATH:= $(call my-dir)

# Also build Kanzi libraries
#include $(LOCAL_PATH)/../../kanzi/jni/Android.mk

# relative to app's jni directory
KANZI_PATH:=../../../../..
EXT_LIBRARY_PATH := $(LOCAL_PATH)/$(KANZI_PATH)/libraries/platforms/android

include $(CLEAR_VARS)

LOCAL_MODULE := kanzi
LOCAL_CFLAGS := -O2 -DKZ_OPENGL_VERSION=KZ_OPENGL_ES_2_0 -DANDROID

LOCAL_C_INCLUDES := \
$(LOCAL_PATH)/$(KANZI_PATH)/sources/system_layer/environments/android/src/ \
$(LOCAL_PATH)/$(KANZI_PATH)/sources/system_layer/common/src/ \
$(LOCAL_PATH)/$(KANZI_PATH)/sources/core_layer/src/ \
$(LOCAL_PATH)/$(KANZI_PATH)/sources/user_layer/src/ \
$(LOCAL_PATH)/$(KANZI_PATH)/sources/application_framework/common/src/ \
$(LOCAL_PATH)/$(KANZI_PATH)/libraries/common/libjpeg/include \
$(LOCAL_PATH)/$(KANZI_PATH)/libraries/common/libpng/include \
$(LOCAL_PATH)/$(KANZI_PATH)/libraries/common/zlib/include \
$(LOCAL_PATH)/$(KANZI_PATH)/libraries/common/itype/include \
$(LOCAL_PATH)/$(KANZI_PATH)/libraries/platforms/linux-arm/itype/include

LOCAL_SRC_FILES := \
$(KANZI_PATH)/applications/kzb_player/src/kzb_player_main.c

LIBPATH := $(LOCAL_PATH)/../../kanzi/obj/local/armeabi/

#LOCAL_STATIC_LIBRARIES := ApplicationFramework User Core System png z jpeg

LOCAL_LDLIBS    := -llog -lGLESv2 -ldl -L$(LIBPATH) -lApplicationFramework -lUser -lCore -lSystem -lluawrapper -lpng -lz -ljpeg -lgcc \
-L$(EXT_LIBRARY_PATH)/freetype/lib -lfreetype -L$(EXT_LIBRARY_PATH)/lua/lib -llua -ltolua

include $(BUILD_SHARED_LIBRARY)

