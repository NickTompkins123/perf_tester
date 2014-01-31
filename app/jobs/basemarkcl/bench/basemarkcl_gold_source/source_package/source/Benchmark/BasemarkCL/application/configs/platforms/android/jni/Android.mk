#
# Please not that this file is not used if you build with SCons
#
LOCAL_PATH:= $(call my-dir)

KANZI_ENGINE_PATH := $(LOCAL_PATH)/../../../../../../../Kanzi/Engine
KANZI_LIBRARY_PATH := $(KANZI_ENGINE_PATH)/lib/android/ES2_Release
EXT_LIBRARY_PATH := $(KANZI_ENGINE_PATH)/libraries/platforms/android
BENCHMARK_PATH_REL := ../../../../../..
BENCHMARK_PATH := $(LOCAL_PATH)/$(BENCHMARK_PATH_REL)
CLMARK_PATH_REL := ../../../..
CLMARK_PATH := $(LOCAL_PATH)/$(CLMARK_PATH_REL)
APPLICATION_PATH := ../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := benchmark 
LOCAL_CFLAGS := -O2 -DKZ_OPENGL_VERSION=KZ_OPENGL_ES_2_0 -DANDROID

LOCAL_C_INCLUDES := \
$(APPLICATION_PATH)/src \
$(KANZI_ENGINE_PATH)/sources/system_layer/environments/android/src/ \
$(KANZI_ENGINE_PATH)/sources/system_layer/common/src/ \
$(KANZI_ENGINE_PATH)/sources/core_layer/src/ \
$(KANZI_ENGINE_PATH)/sources/user_layer/src/ \
$(KANZI_ENGINE_PATH)/sources/application_framework/common/src/ \
$(KANZI_ENGINE_PATH)/sources/application_framework/platforms/android/src/ \
$(KANZI_ENGINE_PATH)/libraries/common/libjpeg/include \
$(KANZI_ENGINE_PATH)/libraries/common/libpng/include \
$(KANZI_ENGINE_PATH)/libraries/common/zlib/include \
$(CLMARK_PATH)/libraries/common/ffmpeg/include \
$(CLMARK_PATH)/libraries/common/opencl/include \
$(CLMARK_PATH)/sources

LOCAL_SRC_FILES_CLUTIL := \
$(CLMARK_PATH_REL)/sources/clutil/clu_image.c \
$(CLMARK_PATH_REL)/sources/clutil/clu_kernel.c \
$(CLMARK_PATH_REL)/sources/clutil/clu_platform.c \
$(CLMARK_PATH_REL)/sources/clutil/clu_program.c \
$(CLMARK_PATH_REL)/sources/clutil/clu_util.c \
$(CLMARK_PATH_REL)/sources/clutil/clu_floatbuffer.c \
$(CLMARK_PATH_REL)/sources/clutil/clu_profiler.c \
$(CLMARK_PATH_REL)/sources/clutil/clu_intbuffer.c \
$(CLMARK_PATH_REL)/sources/clutil/video.c 

LOCAL_SRC_FILES_BFUTIL := \
$(CLMARK_PATH_REL)/sources/benchmarkutil/bf_benchmark_framework.c \
$(CLMARK_PATH_REL)/sources/benchmarkutil/report/bf_info.c \
$(CLMARK_PATH_REL)/sources/benchmarkutil/report/bf_logger.c \
$(CLMARK_PATH_REL)/sources/benchmarkutil/report/bf_report.c \
$(CLMARK_PATH_REL)/sources/benchmarkutil/report/bf_report_document.c \
$(CLMARK_PATH_REL)/sources/benchmarkutil/report/xml/bf_xml_attribute.c \
$(CLMARK_PATH_REL)/sources/benchmarkutil/report/xml/bf_xml_document.c \
$(CLMARK_PATH_REL)/sources/benchmarkutil/report/xml/bf_xml_node.c \
$(CLMARK_PATH_REL)/sources/benchmarkutil/report/bf_timer.c \
$(CLMARK_PATH_REL)/sources/benchmarkutil/scene/bf_scene.c \
$(CLMARK_PATH_REL)/sources/benchmarkutil/scene/bf_scene_queue.c \
$(CLMARK_PATH_REL)/sources/benchmarkutil/screenshot/bf_screenshot.c \
$(CLMARK_PATH_REL)/sources/benchmarkutil/settings/bf_settings.c \
$(CLMARK_PATH_REL)/sources/benchmarkutil/util/bf_input_state.c \
$(CLMARK_PATH_REL)/sources/benchmarkutil/util/bf_util.c \
$(CLMARK_PATH_REL)/sources/benchmarkutil/bf_version.c 


LOCAL_SRC_FILES_CLMARK := \
$(CLMARK_PATH_REL)/sources/clmark/application_interface.c \
$(CLMARK_PATH_REL)/sources/clmark/menu/cl_loadscreen.c \
$(CLMARK_PATH_REL)/sources/clmark/menu/cl_menu.c \
$(CLMARK_PATH_REL)/sources/clmark/tests/feature/cl_compiler.c \
$(CLMARK_PATH_REL)/sources/clmark/tests/feature/cl_julia.c \
$(CLMARK_PATH_REL)/sources/clmark/tests/feature/cl_mandelbulb.c \
$(CLMARK_PATH_REL)/sources/clmark/tests/image/cl_bilateral.c \
$(CLMARK_PATH_REL)/sources/clmark/tests/image/cl_blur.c \
$(CLMARK_PATH_REL)/sources/clmark/tests/image/cl_histogram.c \
$(CLMARK_PATH_REL)/sources/clmark/tests/image/cl_image.c \
$(CLMARK_PATH_REL)/sources/clmark/tests/image/cl_image_common.c \
$(CLMARK_PATH_REL)/sources/clmark/tests/image/cl_median.c \
$(CLMARK_PATH_REL)/sources/clmark/tests/image/cl_sharpening.c \
$(CLMARK_PATH_REL)/sources/clmark/tests/image/cl_video.c \
$(CLMARK_PATH_REL)/sources/clmark/tests/physics/cl_fluid.c \
$(CLMARK_PATH_REL)/sources/clmark/tests/physics/cl_soft_body.c \
$(CLMARK_PATH_REL)/sources/clmark/tests/physics/cl_sph.c \
$(CLMARK_PATH_REL)/sources/clmark/tests/physics/cl_wave.c \
$(CLMARK_PATH_REL)/sources/clmark/tests/physics/dynamic_mesh.c

LOCAL_SRC_FILES := $(LOCAL_SRC_FILES_CLUTIL) $(LOCAL_SRC_FILES_CLMARK) $(LOCAL_SRC_FILES_BFUTIL)

LOCAL_LDLIBS    := -llog -lGLESv2 -ldl -L$(KANZI_LIBRARY_PATH) -lapplicationframework -luser -lcore -lsystem \
-L$(EXT_LIBRARY_PATH)/libpng/lib -lpng -L$(EXT_LIBRARY_PATH)/zlib/lib -lz -L$(EXT_LIBRARY_PATH)/libjpeg/lib -ljpeg -lgcc \
-L$(EXT_LIBRARY_PATH)/freetype/lib -lfreetype \
-L$(CLMARK_PATH)/libraries/platforms/android/ffmpeg/lib -lavcodec -lavformat -lswscale -lavutil

# TODO: include correct cl libraries when available



