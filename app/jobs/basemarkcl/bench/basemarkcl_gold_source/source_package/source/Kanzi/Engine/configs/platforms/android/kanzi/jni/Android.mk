# useful command line for regenerating this stuff
# find sources -name "*.h" |sed "s|\(.*src\)\/.*\$|\$(LOCAL_PATH)/\$(KANZI_PATH)\/\1\/|g" |uniq
# find sources/user_layer -name "*.c" |sed "s|\(^.*\$\)|\$(KANZI_PATH)\/\\1 \\\\|"   <-- fish variant
# find sources/core_layer -name "*.c" |sed "s|\(^.*\$\)|\$(KANZI_PATH)\/\\1 \\|"     <-- bash variant(?)
# find . -name "*.h" |sed "s|\.\/\(.*\)\/[A-Za-z0-9_.-]*$|\t\"\1\",|g" |uniq

LOCAL_PATH := $(call my-dir)

# "Engine" relative to jni directory
KANZI_PATH := ../../../../..

ifeq ($(GL), ES1)
GL_FLAGS := -DKZ_OPENGL_VERSION=KZ_OPENGL_ES_1_1
else
GL_FLAGS := -DKZ_OPENGL_VERSION=KZ_OPENGL_ES_2_0
endif
# will be replaced by scons in the final release --j

ifeq ($(CONFIG), Debug)
CONF_FLAGS := -g -DDEBUG -D_DEBUG
else
CONF_FLAGS :=
# CONF_FLAGS := -O2  # not a good idea, -O0 is generated by ndk-build
endif

KANZI_C_FLAGS := -DANDROID -DENABLE_LUA $(GL_FLAGS) $(CONF_FLAGS)

# find sources -name "*.h" |sed "s|\(.*src\)\/.*\$|\$(LOCAL_PATH)/\$(KANZI_PATH)\/\1\/ \\\\|g" |uniq
KANZI_LIBRARY_INCLUDE_PATHS := \
$(LOCAL_PATH)/$(KANZI_PATH)/sources/system_layer/environments/android/src/ \
$(LOCAL_PATH)/$(KANZI_PATH)/sources/system_layer/common/src/ \
$(LOCAL_PATH)/$(KANZI_PATH)/sources/lua_wrapper/src/ \
$(LOCAL_PATH)/$(KANZI_PATH)/sources/managed_wrapper/src/ \
$(LOCAL_PATH)/$(KANZI_PATH)/sources/application_framework/platforms/android/src/ \
$(LOCAL_PATH)/$(KANZI_PATH)/sources/application_framework/common/src/ \
$(LOCAL_PATH)/$(KANZI_PATH)/sources/core_layer/src/ \
$(LOCAL_PATH)/$(KANZI_PATH)/sources/user_layer/src/

# find libraries -name "*.h" |grep "include\/" |sed "s|\(.*include\)\/.*\$|\$(LOCAL_PATH)/\$(KANZI_PATH)\/\1\/ \\\\|g" |uniq
EXT_LIBRARY_INCLUDE_PATHS := \
$(LOCAL_PATH)/$(KANZI_PATH)/libraries/platforms/linux-arm/libts/include/ \
$(LOCAL_PATH)/$(KANZI_PATH)/libraries/platforms/linux-arm/itype/include/ \
$(LOCAL_PATH)/$(KANZI_PATH)/libraries/platforms/linux-arm/libjpeg/include/ \
$(LOCAL_PATH)/$(KANZI_PATH)/libraries/common/itype/include \
$(LOCAL_PATH)/$(KANZI_PATH)/libraries/common/libjpeg/include \
$(LOCAL_PATH)/$(KANZI_PATH)/libraries/common/libpng/include \
$(LOCAL_PATH)/$(KANZI_PATH)/libraries/common/zlib/include \
$(LOCAL_PATH)/$(KANZI_PATH)/libraries/common/lua/include \
$(LOCAL_PATH)/$(KANZI_PATH)/libraries/common/freetype/include

# $(LOCAL_PATH)/$(KANZI_PATH)/libraries/common/itype/include
# $(LOCAL_PATH)/$(KANZI_PATH)/libraries/platforms/linux-arm/itype/include

KANZI_INCLUDE_PATHS := $(KANZI_LIBRARY_INCLUDE_PATHS) $(EXT_LIBRARY_INCLUDE_PATHS)

include $(CLEAR_VARS)



##################################################################### jpeg

LOCAL_MODULE    := jpeg
LOCAL_CFLAGS    := 

LOCAL_C_INCLUDES := \
$(LOCAL_PATH)/$(KANZI_PATH)/libraries/common/libjpeg/include

LOCAL_SRC_FILES := \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jaricom.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jcapimin.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jcapistd.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jcarith.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jccoefct.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jccolor.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jcdctmgr.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jchuff.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jcinit.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jcmainct.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jcmarker.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jcmaster.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jcomapi.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jcparam.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jcprepct.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jcsample.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jctrans.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jdapimin.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jdapistd.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jdarith.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jdatadst.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jdatasrc.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jdcoefct.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jdcolor.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jddctmgr.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jdhuff.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jdinput.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jdmainct.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jdmarker.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jdmaster.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jdmerge.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jdpostct.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jdsample.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jdtrans.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jerror.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jfdctflt.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jfdctfst.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jfdctint.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jidctflt.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jidctfst.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jidctint.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jquant1.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jquant2.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jutils.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jmemmgr.c \
$(KANZI_PATH)/libraries/common/libjpeg/jpeg-7/jmemnobs.c

include $(BUILD_STATIC_LIBRARY)
include $(CLEAR_VARS)

##################################################################### zlib

LOCAL_MODULE := z
LOCAL_CFLAGS := 

LOCAL_C_INCLUDES := \
$(LOCAL_PATH)/$(KANZI_PATH)/libraries/common/zlib/include

LOCAL_SRC_FILES := \
$(KANZI_PATH)/libraries/common/zlib/zlib123/adler32.c \
$(KANZI_PATH)/libraries/common/zlib/zlib123/compress.c \
$(KANZI_PATH)/libraries/common/zlib/zlib123/crc32.c \
$(KANZI_PATH)/libraries/common/zlib/zlib123/deflate.c \
$(KANZI_PATH)/libraries/common/zlib/zlib123/gzio.c \
$(KANZI_PATH)/libraries/common/zlib/zlib123/uncompr.c \
$(KANZI_PATH)/libraries/common/zlib/zlib123/trees.c \
$(KANZI_PATH)/libraries/common/zlib/zlib123/zutil.c \
$(KANZI_PATH)/libraries/common/zlib/zlib123/inflate.c \
$(KANZI_PATH)/libraries/common/zlib/zlib123/infback.c \
$(KANZI_PATH)/libraries/common/zlib/zlib123/inftrees.c \
$(KANZI_PATH)/libraries/common/zlib/zlib123/inffast.c

include $(BUILD_STATIC_LIBRARY)
include $(CLEAR_VARS)

##################################################################### png

LOCAL_MODULE := png
LOCAL_CFLAGS := 

LOCAL_C_INCLUDES := \
$(LOCAL_PATH)/$(KANZI_PATH)/libraries/common/libpng/include

LOCAL_SRC_FILES := \
$(KANZI_PATH)/libraries/common/libpng/libpng-1.2.41/png.c \
$(KANZI_PATH)/libraries/common/libpng/libpng-1.2.41/pngset.c \
$(KANZI_PATH)/libraries/common/libpng/libpng-1.2.41/pngget.c \
$(KANZI_PATH)/libraries/common/libpng/libpng-1.2.41/pngrutil.c \
$(KANZI_PATH)/libraries/common/libpng/libpng-1.2.41/pngtrans.c \
$(KANZI_PATH)/libraries/common/libpng/libpng-1.2.41/pngwutil.c \
$(KANZI_PATH)/libraries/common/libpng/libpng-1.2.41/pngread.c \
$(KANZI_PATH)/libraries/common/libpng/libpng-1.2.41/pngrio.c \
$(KANZI_PATH)/libraries/common/libpng/libpng-1.2.41/pngwio.c \
$(KANZI_PATH)/libraries/common/libpng/libpng-1.2.41/pngwrite.c \
$(KANZI_PATH)/libraries/common/libpng/libpng-1.2.41/pngrtran.c \
$(KANZI_PATH)/libraries/common/libpng/libpng-1.2.41/pngwtran.c \
$(KANZI_PATH)/libraries/common/libpng/libpng-1.2.41/pngmem.c \
$(KANZI_PATH)/libraries/common/libpng/libpng-1.2.41/pngerror.c \
$(KANZI_PATH)/libraries/common/libpng/libpng-1.2.41/pngpread.c

include $(BUILD_STATIC_LIBRARY)
include $(CLEAR_VARS)


##################################################################### lua wrapper

LOCAL_MODULE := luawrapper
LOCAL_CFLAGS := $(KANZI_C_FLAGS)
LOCAL_C_INCLUDES := $(KANZI_INCLUDE_PATHS)
# $(LOCAL_PATH)/$(KANZI_PATH)/libraries/common/lua/include

# find sources/lua_wrapper -name "*.c" |sed "s|\(^.*\$\)|\$(KANZI_PATH)\/\\1 \\\\|"
LOCAL_SRC_FILES := \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_project.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_object.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_vector4_property.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_texture_property.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_ui_event_listener.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_bool_property.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_renderer.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_material.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_mesh.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_light.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_vector3_property.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_camera.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_matrix4x4_property.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_ui_component.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_string_property.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_composer.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_ui_component_type.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_user.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_matrix2x2_property.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_ui_event.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_engine.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_bounding_volume.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_ui_screen.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_float_property.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_light_property.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_matrix3x3_property.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_color_property.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_property.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_scene.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_ui_action.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_render_pass.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_engine_message.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_vector2_property.c \
$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/lua_bind.c

# old stuff that once was there
#$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/user/lua_kzu_globals.c
#$(KANZI_PATH)/sources/lua_wrapper/src/lua_wrapper/system/lua_kzs_log.c

include $(BUILD_STATIC_LIBRARY)
include $(CLEAR_VARS)


##################################################################### kanzi system

LOCAL_MODULE := system
LOCAL_CFLAGS := $(KANZI_C_FLAGS)
LOCAL_C_INCLUDES := $(KANZI_INCLUDE_PATHS)

LOCAL_SRC_FILES := \
$(KANZI_PATH)/sources/system_layer/environments/android/src/system/input/kzs_input_native.c \
$(KANZI_PATH)/sources/system_layer/environments/android/src/system/display/kzs_surface_native.c \
$(KANZI_PATH)/sources/system_layer/environments/android/src/system/display/kzs_display_native.c \
$(KANZI_PATH)/sources/system_layer/environments/android/src/system/display/kzs_window_native.c \
$(KANZI_PATH)/sources/system_layer/environments/android/src/system/file/kzs_resource_file.c \
$(KANZI_PATH)/sources/system_layer/environments/android/src/system/debug/kzs_log_android.c \
$(KANZI_PATH)/sources/system_layer/environments/android/src/system/time/kzs_tick.c \
$(KANZI_PATH)/sources/system_layer/environments/android/src/system/time/kzs_time.c \
$(KANZI_PATH)/sources/system_layer/environments/android/src/system/time/kzs_sleep.c \
$(KANZI_PATH)/sources/system_layer/common/src/system/kzs_system.c \
$(KANZI_PATH)/sources/system_layer/common/src/system/input/kzs_input.c \
$(KANZI_PATH)/sources/system_layer/common/src/system/kzs_globals.c \
$(KANZI_PATH)/sources/system_layer/common/src/system/wrappers/kzs_openvg_base.c \
$(KANZI_PATH)/sources/system_layer/common/src/system/wrappers/kzs_math.c \
$(KANZI_PATH)/sources/system_layer/common/src/system/wrappers/kzs_memory.c \
$(KANZI_PATH)/sources/system_layer/common/src/system/wrappers/kzs_opengl_base.c \
$(KANZI_PATH)/sources/system_layer/common/src/system/display/kzs_display.c \
$(KANZI_PATH)/sources/system_layer/common/src/system/display/kzs_surface.c \
$(KANZI_PATH)/sources/system_layer/common/src/system/display/kzs_window.c \
$(KANZI_PATH)/sources/system_layer/common/src/system/file/kzs_resource_file.c \
$(KANZI_PATH)/sources/system_layer/common/src/system/file/kzs_file_base.c \
$(KANZI_PATH)/sources/system_layer/common/src/system/file/kzs_file.c \
$(KANZI_PATH)/sources/system_layer/common/src/system/debug/kzs_log.c \
$(KANZI_PATH)/sources/system_layer/common/src/system/debug/kzs_counter.c \
$(KANZI_PATH)/sources/system_layer/common/src/system/debug/kzs_error.c

include $(BUILD_STATIC_LIBRARY)
include $(CLEAR_VARS)

##################################################################### kanzi core

LOCAL_MODULE := core
LOCAL_CFLAGS := $(KANZI_C_FLAGS)
LOCAL_C_INCLUDES := $(KANZI_INCLUDE_PATHS)

LOCAL_SRC_FILES := \
$(KANZI_PATH)/sources/core_layer/src/core/resource_manager/vertex_buffer/kzc_resource_vertex_buffer.c \
$(KANZI_PATH)/sources/core_layer/src/core/resource_manager/vertex_buffer/kzc_resource_vertex.c \
$(KANZI_PATH)/sources/core_layer/src/core/resource_manager/index_buffer/kzc_resource_index_buffer.c \
$(KANZI_PATH)/sources/core_layer/src/core/resource_manager/texture/kzc_resource_texture.c \
$(KANZI_PATH)/sources/core_layer/src/core/resource_manager/kzc_resource_manager.c \
$(KANZI_PATH)/sources/core_layer/src/core/resource_manager/frame_buffer/kzc_resource_frame_buffer.c \
$(KANZI_PATH)/sources/core_layer/src/core/resource_manager/shader/kzc_resource_shader.c \
$(KANZI_PATH)/sources/core_layer/src/core/memory/kzc_memory_manager.c \
$(KANZI_PATH)/sources/core_layer/src/core/memory/kzc_memory_custom.c \
$(KANZI_PATH)/sources/core_layer/src/core/memory/kzc_byte_buffer.c \
$(KANZI_PATH)/sources/core_layer/src/core/memory/kzc_memory_pool.c \
$(KANZI_PATH)/sources/core_layer/src/core/memory/kzc_memory_pooled.c \
$(KANZI_PATH)/sources/core_layer/src/core/memory/kzc_memory_quick.c \
$(KANZI_PATH)/sources/core_layer/src/core/memory/kzc_memory_system.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/data_log/kzc_data_log.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/data_log/kzc_stopwatch.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/io/kzc_output_stream.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/io/kzc_input_stream.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/io/kzc_file.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/font/kzc_text_layout.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/font/kzc_itype_font.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/font/kzc_freetype_font.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/font/kzc_bitmap_font.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/font/kzc_configured_font.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/font/kzc_font.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/font/kzc_text_layouter.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/font/kzc_truetype.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/collection/kzc_sort.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/collection/kzc_hash_set.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/collection/kzc_comparator.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/collection/kzc_balanced_tree.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/collection/kzc_hash_code.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/collection/kzc_hash_map.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/collection/kzc_stack.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/collection/kzc_linked_list.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/collection/kzc_dynamic_array.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/collection/kzc_queue.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/collection/kzc_shuffle.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/string/kzc_string_buffer.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/string/kzc_string.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/image/kzc_image.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/image/kzc_etc.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/image/kzc_screen_capture.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/math/kzc_matrix4x4.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/math/kzc_matrix3x3.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/math/kzc_quaternion.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/math/kzc_rectangle.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/math/kzc_vector2.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/math/kzc_spherical_coordinates.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/math/kzc_ray.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/math/kzc_volume.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/math/kzc_vector3.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/math/kzc_plane.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/math/kzc_vector4.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/math/kzc_random.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/math/kzc_matrix2x2.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/curve/kzc_spline.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/curve/kzc_trajectory.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/curve/kzc_interpolation.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/settings/kzc_settings_parser.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/settings/kzc_settings.c \
$(KANZI_PATH)/sources/core_layer/src/core/util/color/kzc_color.c \
$(KANZI_PATH)/sources/core_layer/src/core/debug/kzc_log.c \
$(KANZI_PATH)/sources/core_layer/src/core/renderer/kzc_renderer_es2.c \
$(KANZI_PATH)/sources/core_layer/src/core/renderer/kzc_renderer_es1.c

include $(BUILD_STATIC_LIBRARY)
include $(CLEAR_VARS)

##################################################################### kanzi user

LOCAL_MODULE := user
LOCAL_CFLAGS := $(KANZI_C_FLAGS)
LOCAL_C_INCLUDES := $(KANZI_INCLUDE_PATHS)

LOCAL_SRC_FILES := \
$(KANZI_PATH)/sources/user_layer/src/user/binary/kzu_binary_util.c \
$(KANZI_PATH)/sources/user_layer/src/user/binary/kzu_binary_loader.c \
$(KANZI_PATH)/sources/user_layer/src/user/binary/kzu_binary_directory.c \
$(KANZI_PATH)/sources/user_layer/src/user/material/kzu_material_type.c \
$(KANZI_PATH)/sources/user_layer/src/user/material/kzu_material.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_loader_user_interface.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_loader_composer.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_loader_property.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_loader_project.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_loader_transition.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_creator_ui_object.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_loader_trajectory.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_loader_scene.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_loader_custom_object.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_patcher.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_loader_mesh.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_loader.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_creator_ui_component.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_loader_animation.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_loader_camera.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_loader_shader.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_loader_texture.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_loader_script.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_loader_object_node.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_loader_property_group.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_loader_object_source.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_loader_light.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_loader_material.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_loader_spline.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_loader_render_pass.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_loader_image.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_gpu_memory_type.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_loader_lod.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_loader_font.c \
$(KANZI_PATH)/sources/user_layer/src/user/project/kzu_project_loader_general.c \
$(KANZI_PATH)/sources/user_layer/src/user/filter/kzu_contains_property_filter.c \
$(KANZI_PATH)/sources/user_layer/src/user/filter/kzu_frustum_cull_filter.c \
$(KANZI_PATH)/sources/user_layer/src/user/filter/kzu_distance_filter.c \
$(KANZI_PATH)/sources/user_layer/src/user/filter/kzu_object_type_filter.c \
$(KANZI_PATH)/sources/user_layer/src/user/filter/kzu_lod_filter.c \
$(KANZI_PATH)/sources/user_layer/src/user/filter/kzu_sort_by_z_object_source.c \
$(KANZI_PATH)/sources/user_layer/src/user/filter/kzu_object_set_filter.c \
$(KANZI_PATH)/sources/user_layer/src/user/filter/kzu_object_source.c \
$(KANZI_PATH)/sources/user_layer/src/user/filter/kzu_sort_by_material_type_source.c \
$(KANZI_PATH)/sources/user_layer/src/user/engine/kzu_engine_message.c \
$(KANZI_PATH)/sources/user_layer/src/user/engine/kzu_engine.c \
$(KANZI_PATH)/sources/user_layer/src/user/engine/kzu_engine_message_queue.c \
$(KANZI_PATH)/sources/user_layer/src/user/util/hud/kzu_hud_data.c \
$(KANZI_PATH)/sources/user_layer/src/user/util/hud/kzu_hud.c \
$(KANZI_PATH)/sources/user_layer/src/user/util/kzu_transform_gizmo.c \
$(KANZI_PATH)/sources/user_layer/src/user/util/kzu_raypick.c \
$(KANZI_PATH)/sources/user_layer/src/user/util/kzu_free_camera.c \
$(KANZI_PATH)/sources/user_layer/src/user/util/kzu_pick.c \
$(KANZI_PATH)/sources/user_layer/src/user/common/kzu_user.c \
$(KANZI_PATH)/sources/user_layer/src/user/common/kzu_globals.c \
$(KANZI_PATH)/sources/user_layer/src/user/properties/kzu_texture_property.c \
$(KANZI_PATH)/sources/user_layer/src/user/properties/kzu_vector4_property.c \
$(KANZI_PATH)/sources/user_layer/src/user/properties/kzu_void_property.c \
$(KANZI_PATH)/sources/user_layer/src/user/properties/kzu_property_group.c \
$(KANZI_PATH)/sources/user_layer/src/user/properties/kzu_float_property.c \
$(KANZI_PATH)/sources/user_layer/src/user/properties/kzu_vector2_property.c \
$(KANZI_PATH)/sources/user_layer/src/user/properties/kzu_string_property.c \
$(KANZI_PATH)/sources/user_layer/src/user/properties/kzu_light_property.c \
$(KANZI_PATH)/sources/user_layer/src/user/properties/kzu_fixed_properties.c \
$(KANZI_PATH)/sources/user_layer/src/user/properties/kzu_matrix3x3_property.c \
$(KANZI_PATH)/sources/user_layer/src/user/properties/kzu_vector3_property.c \
$(KANZI_PATH)/sources/user_layer/src/user/properties/kzu_property_collection.c \
$(KANZI_PATH)/sources/user_layer/src/user/properties/kzu_property.c \
$(KANZI_PATH)/sources/user_layer/src/user/properties/kzu_int_property.c \
$(KANZI_PATH)/sources/user_layer/src/user/properties/kzu_matrix4x4_property.c \
$(KANZI_PATH)/sources/user_layer/src/user/properties/kzu_matrix2x2_property.c \
$(KANZI_PATH)/sources/user_layer/src/user/properties/kzu_bool_property.c \
$(KANZI_PATH)/sources/user_layer/src/user/properties/kzu_color_property.c \
$(KANZI_PATH)/sources/user_layer/src/user/ui/kzu_ui_event.c \
$(KANZI_PATH)/sources/user_layer/src/user/ui/kzu_ui_screen.c \
$(KANZI_PATH)/sources/user_layer/src/user/ui/kzu_ui_component.c \
$(KANZI_PATH)/sources/user_layer/src/user/ui/kzu_ui_action.c \
$(KANZI_PATH)/sources/user_layer/src/user/ui/kzu_ui_manager.c \
$(KANZI_PATH)/sources/user_layer/src/user/ui/kzu_ui_component_type.c \
$(KANZI_PATH)/sources/user_layer/src/user/ui/components/kzu_ui_toggle_button.c \
$(KANZI_PATH)/sources/user_layer/src/user/ui/components/kzu_ui_dropdown.c \
$(KANZI_PATH)/sources/user_layer/src/user/ui/components/kzu_ui_grid_layout.c \
$(KANZI_PATH)/sources/user_layer/src/user/ui/components/kzu_ui_actions.c \
$(KANZI_PATH)/sources/user_layer/src/user/ui/components/kzu_ui_dock_layout.c \
$(KANZI_PATH)/sources/user_layer/src/user/ui/components/kzu_ui_trajectory_layout.c \
$(KANZI_PATH)/sources/user_layer/src/user/ui/components/kzu_ui_button.c \
$(KANZI_PATH)/sources/user_layer/src/user/ui/components/kzu_ui_slider.c \
$(KANZI_PATH)/sources/user_layer/src/user/ui/components/kzu_ui_viewbox.c \
$(KANZI_PATH)/sources/user_layer/src/user/ui/components/kzu_ui_playlist_player.c \
$(KANZI_PATH)/sources/user_layer/src/user/ui/components/kzu_ui_stack_layout.c \
$(KANZI_PATH)/sources/user_layer/src/user/ui/components/kzu_ui_label.c \
$(KANZI_PATH)/sources/user_layer/src/user/ui/kzu_ui_event_listener.c \
$(KANZI_PATH)/sources/user_layer/src/user/scripting/kzu_script.c \
$(KANZI_PATH)/sources/user_layer/src/user/scripting/lua/kzu_lua.c \
$(KANZI_PATH)/sources/user_layer/src/user/renderer/kzu_shadow_map.c \
$(KANZI_PATH)/sources/user_layer/src/user/renderer/kzu_composer.c \
$(KANZI_PATH)/sources/user_layer/src/user/renderer/kzu_renderer.c \
$(KANZI_PATH)/sources/user_layer/src/user/renderer/kzu_renderer_util.c \
$(KANZI_PATH)/sources/user_layer/src/user/renderer/kzu_default_shader.c \
$(KANZI_PATH)/sources/user_layer/src/user/renderer/kzu_render_pass.c \
$(KANZI_PATH)/sources/user_layer/src/user/effect_system/postprocessing/kzu_depth_shader.c \
$(KANZI_PATH)/sources/user_layer/src/user/effect_system/postprocessing/kzu_depth_of_field_shader.c \
$(KANZI_PATH)/sources/user_layer/src/user/effect_system/postprocessing/kzu_box_blur_shader.c \
$(KANZI_PATH)/sources/user_layer/src/user/effect_system/transition/kzu_transition.c \
$(KANZI_PATH)/sources/user_layer/src/user/effect_system/transition/kzu_transition_shader.c \
$(KANZI_PATH)/sources/user_layer/src/user/animation/kzu_time_line_entry.c \
$(KANZI_PATH)/sources/user_layer/src/user/animation/kzu_animation_player.c \
$(KANZI_PATH)/sources/user_layer/src/user/animation/kzu_animation_clip.c \
$(KANZI_PATH)/sources/user_layer/src/user/animation/kzu_animation.c \
$(KANZI_PATH)/sources/user_layer/src/user/animation/kzu_time_line_sequence.c \
$(KANZI_PATH)/sources/user_layer/src/user/animation/kzu_animation_key.c \
$(KANZI_PATH)/sources/user_layer/src/user/scene_graph/bounding_volume/kzu_bounding_volume.c \
$(KANZI_PATH)/sources/user_layer/src/user/scene_graph/kzu_mesh.c \
$(KANZI_PATH)/sources/user_layer/src/user/scene_graph/kzu_object.c \
$(KANZI_PATH)/sources/user_layer/src/user/scene_graph/kzu_custom_object.c \
$(KANZI_PATH)/sources/user_layer/src/user/scene_graph/kzu_lod_selector.c \
$(KANZI_PATH)/sources/user_layer/src/user/scene_graph/kzu_transformed_scene.c \
$(KANZI_PATH)/sources/user_layer/src/user/scene_graph/kzu_light.c \
$(KANZI_PATH)/sources/user_layer/src/user/scene_graph/kzu_text_object.c \
$(KANZI_PATH)/sources/user_layer/src/user/scene_graph/kzu_scene.c \
$(KANZI_PATH)/sources/user_layer/src/user/scene_graph/kzu_bone.c \
$(KANZI_PATH)/sources/user_layer/src/user/scene_graph/kzu_transformed_object.c \
$(KANZI_PATH)/sources/user_layer/src/user/scene_graph/kzu_camera.c

include $(BUILD_STATIC_LIBRARY)
include $(CLEAR_VARS)

##################################################################### kanzi application framework

LOCAL_MODULE := applicationframework
LOCAL_CFLAGS := $(KANZI_C_FLAGS)
LOCAL_C_INCLUDES := $(KANZI_INCLUDE_PATHS)

LOCAL_SRC_FILES := \
$(KANZI_PATH)/sources/application_framework/platforms/android/src/application/kza_android_main.c \
$(KANZI_PATH)/sources/application_framework/platforms/android/src/application/kza_android_application_interface.c \
$(KANZI_PATH)/sources/application_framework/common/src/application/kza_application_events.c \
$(KANZI_PATH)/sources/application_framework/common/src/application/kza_application.c \
$(KANZI_PATH)/sources/application_framework/common/src/application/kza_splash.c \
$(KANZI_PATH)/sources/application_framework/common/src/application/splash_screen_images/kza_splash_bg.c \
$(KANZI_PATH)/sources/application_framework/common/src/application/splash_screen_images/kza_splash_bottom.c \
$(KANZI_PATH)/sources/application_framework/common/src/application/splash_screen_images/kza_splash_top.c \
$(KANZI_PATH)/sources/application_framework/common/src/application/kza_window.c \
$(KANZI_PATH)/sources/application_framework/common/src/application/transition/kza_transition_shader.c \
$(KANZI_PATH)/sources/application_framework/common/src/application/transition/kza_transition.c

include $(BUILD_STATIC_LIBRARY)
include $(CLEAR_VARS)

