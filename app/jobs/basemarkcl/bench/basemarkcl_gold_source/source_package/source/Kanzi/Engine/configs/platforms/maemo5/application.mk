KANZI_LIB_ROOT = ../../../../../../Engine/lib/maemo5
EXT_LIB_ROOT_ARM = ../../../../../../Engine/libraries/platforms/linux-arm
EXT_LIB_ROOT_MAEMO = ../../../../../../Engine/libraries/platforms/maemo5
KANZI_INC_ROOT = ../../../../../../Engine/headers

ifeq ($(GL), ES1)
GL_VERSION = KZ_OPENGL_ES_1_1
GL_LIB = GLES_CM
else
ifeq ($(GL), ES2)
GL_VERSION = KZ_OPENGL_ES_2_0
GL_LIB = GLESv2
else
$(error define GL=ES1 or GL=ES2)
endif
endif

ifeq ($(CONFIG), Debug)
else
ifeq ($(CONFIG), Release)
else
$(error define CONFIG=Debug or CONFIG=Release)
endif
endif

KANZI_LIB_PATH = $(KANZI_LIB_ROOT)/$(GL)_$(CONFIG)

APP_SRC_ROOT = ../../../src
INC_DIRS = 	$(KANZI_INC_ROOT)/application_framework/common/include \
		$(KANZI_INC_ROOT)/application_framework/platforms/maemo5/include \
		$(KANZI_INC_ROOT)/core/include \
		$(KANZI_INC_ROOT)/user/include \
		$(KANZI_INC_ROOT)/system/common/include \
		$(KANZI_INC_ROOT)/system/environments/egl/include \
		$(KANZI_INC_ROOT)/system/platforms/maemo5_egl/include

EXT_LIBS = 	-L$(EXT_LIB_ROOT_ARM)/libjpeg/lib -ljpeg \
		-L$(EXT_LIB_ROOT_ARM)/libpng/lib -lpng \
		-L$(EXT_LIB_ROOT_ARM)/zlib/lib -lz \
		-L$(EXT_LIB_ROOT_MAEMO)/freetype/lib -lfreetype \
		-L$(EXT_LIB_ROOT_MAEMO)/lua/lib -llua -ltolua++

CC = gcc
LFLAGS = -lm -l$(GL_LIB) -lEGL -lIMGegl -lsrv_um -lXau -lXdmcp -lX11 -L$(KANZI_LIB_PATH) -lApplicationFramework -lUser -lCore -lSystem $(EXT_LIBS)
CFLAGS = $(addprefix -I, $(INC_DIRS)) -DKZ_OPENGL_VERSION=$(GL_VERSION) -DMAEMO

ifeq ($(CONFIG), Debug)
CFLAGS += -D_DEBUG
endif
