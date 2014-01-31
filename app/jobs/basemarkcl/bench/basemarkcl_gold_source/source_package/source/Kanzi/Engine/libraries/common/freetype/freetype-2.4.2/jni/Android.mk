LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE  := freetype
LOCAL_CFLAGS  := -DFT2_BUILD_LIBRARY

LOCAL_C_INCLUDES := \
$(LOCAL_PATH)/../include

SRC_DIR := ../src

# from builds directory
BASE_DIR := $(SRC_DIR)/base
LOCAL_SRC_FILES := \
$(BASE_DIR)/ftsystem.c \
$(BASE_DIR)/ftdebug.c  \
$(BASE_DIR)/ftinit.c


# generated from src directory with
# find -name "rules.mk" |xargs grep -h "_DIR *:="
LZW_DIR := $(SRC_DIR)/lzw
SFNT_DIR := $(SRC_DIR)/sfnt
GXV_DIR := $(SRC_DIR)/gxvalid
PSAUX_DIR := $(SRC_DIR)/psaux
CACHE_DIR   := $(SRC_DIR)/cache
BDF_DIR := $(SRC_DIR)/bdf
PSHINTER_DIR := $(SRC_DIR)/pshinter
PFR_DIR := $(SRC_DIR)/pfr
PSNAMES_DIR := $(SRC_DIR)/psnames
TT_DIR := $(SRC_DIR)/truetype
RASTER_DIR := $(SRC_DIR)/raster
T1_DIR := $(SRC_DIR)/type1
OTV_DIR := $(SRC_DIR)/otvalid
CFF_DIR := $(SRC_DIR)/cff
SMOOTH_DIR := $(SRC_DIR)/smooth
FNT_DIR := $(SRC_DIR)/winfonts
CID_DIR := $(SRC_DIR)/cid
AUTOF_DIR := $(SRC_DIR)/autofit
GZIP_DIR := $(SRC_DIR)/gzip
PCF_DIR := $(SRC_DIR)/pcf
T42_DIR := $(SRC_DIR)/type42

# generated from src directory + manually added \\'s and removed lines after first without \\ in the end
# find -name "rules.mk" |xargs grep -oh "\$.[A-Z]*_DIR..[a-z0-9_-]*\.c *\\\\*" |uniq >> jni/Android.mk
# find -name "rules.mk" |xargs grep -oh "$.[A-Z]*_DIR..[a-z0-9_-]*\.c *\\*" |uniq >> jni/Android.mk
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES) \
$(LZW_DIR)/ftlzw.c      \
$(LZW_DIR)/ftzopen.c    \
$(SFNT_DIR)/ttload.c   \
$(SFNT_DIR)/ttmtx.c    \
$(SFNT_DIR)/ttcmap.c   \
$(SFNT_DIR)/ttsbit.c   \
$(SFNT_DIR)/ttpost.c   \
$(SFNT_DIR)/ttkern.c   \
$(SFNT_DIR)/ttbdf.c    \
$(SFNT_DIR)/sfobjs.c   \
$(SFNT_DIR)/sfdriver.c \
$(GXV_DIR)/gxvcommn.c \
$(GXV_DIR)/gxvfeat.c  \
$(GXV_DIR)/gxvbsln.c  \
$(GXV_DIR)/gxvtrak.c  \
$(GXV_DIR)/gxvopbd.c  \
$(GXV_DIR)/gxvprop.c  \
$(GXV_DIR)/gxvjust.c  \
$(GXV_DIR)/gxvmort.c  \
$(GXV_DIR)/gxvmort0.c \
$(GXV_DIR)/gxvmort1.c \
$(GXV_DIR)/gxvmort2.c \
$(GXV_DIR)/gxvmort4.c \
$(GXV_DIR)/gxvmort5.c \
$(GXV_DIR)/gxvmorx.c  \
$(GXV_DIR)/gxvmorx0.c \
$(GXV_DIR)/gxvmorx1.c \
$(GXV_DIR)/gxvmorx2.c \
$(GXV_DIR)/gxvmorx4.c \
$(GXV_DIR)/gxvmorx5.c \
$(GXV_DIR)/gxvlcar.c  \
$(GXV_DIR)/gxvkern.c  \
$(GXV_DIR)/gxvmod.c   \
$(PSAUX_DIR)/psobjs.c   \
$(PSAUX_DIR)/t1decode.c \
$(PSAUX_DIR)/t1cmap.c   \
$(PSAUX_DIR)/afmparse.c \
$(PSAUX_DIR)/psconv.c   \
$(PSAUX_DIR)/psauxmod.c \
$(CACHE_DIR)/ftcbasic.c \
$(CACHE_DIR)/ftccache.c \
$(CACHE_DIR)/ftccmap.c  \
$(CACHE_DIR)/ftcglyph.c \
$(CACHE_DIR)/ftcimage.c \
$(CACHE_DIR)/ftcmanag.c \
$(CACHE_DIR)/ftcmru.c   \
$(CACHE_DIR)/ftcsbits.c \
$(BDF_DIR)/bdflib.c         \
$(BDF_DIR)/bdfdrivr.c       \
$(PSHINTER_DIR)/pshrec.c  \
$(PSHINTER_DIR)/pshglob.c \
$(PSHINTER_DIR)/pshmod.c  \
$(PSHINTER_DIR)/pshalgo.c \
$(PFR_DIR)/pfrload.c  \
$(PFR_DIR)/pfrgload.c \
$(PFR_DIR)/pfrcmap.c  \
$(PFR_DIR)/pfrdrivr.c \
$(PFR_DIR)/pfrsbit.c  \
$(PFR_DIR)/pfrobjs.c  \
$(PSNAMES_DIR)/psmodule.c \
$(TT_DIR)/ttobjs.c   \
$(TT_DIR)/ttpload.c  \
$(TT_DIR)/ttgload.c  \
$(TT_DIR)/ttinterp.c \
$(TT_DIR)/ttgxvar.c  \
$(TT_DIR)/ttdriver.c \
$(RASTER_DIR)/ftraster.c \
$(RASTER_DIR)/ftrend1.c  \
$(T1_DIR)/t1parse.c  \
$(T1_DIR)/t1load.c   \
$(T1_DIR)/t1driver.c \
$(T1_DIR)/t1afm.c    \
$(T1_DIR)/t1gload.c  \
$(T1_DIR)/t1objs.c   \
$(OTV_DIR)/otvbase.c  \
$(OTV_DIR)/otvcommn.c \
$(OTV_DIR)/otvgdef.c  \
$(OTV_DIR)/otvgpos.c  \
$(OTV_DIR)/otvgsub.c  \
$(OTV_DIR)/otvjstf.c  \
$(OTV_DIR)/otvmath.c  \
$(OTV_DIR)/otvmod.c   \
$(CFF_DIR)/cffobjs.c  \
$(CFF_DIR)/cffload.c  \
$(CFF_DIR)/cffgload.c \
$(CFF_DIR)/cffparse.c \
$(CFF_DIR)/cffcmap.c  \
$(CFF_DIR)/cffdrivr.c \
$(SMOOTH_DIR)/ftgrays.c  \
$(SMOOTH_DIR)/ftsmooth.c \
$(FNT_DIR)/winfnt.c \
$(CID_DIR)/cidparse.c \
$(CID_DIR)/cidload.c  \
$(CID_DIR)/cidriver.c \
$(CID_DIR)/cidgload.c \
$(CID_DIR)/cidobjs.c  \
$(AUTOF_DIR)/afangles.c \
$(AUTOF_DIR)/afcjk.c    \
$(AUTOF_DIR)/afdummy.c  \
$(AUTOF_DIR)/afglobal.c \
$(AUTOF_DIR)/afhints.c  \
$(AUTOF_DIR)/afindic.c  \
$(AUTOF_DIR)/aflatin.c  \
$(AUTOF_DIR)/afloader.c \
$(AUTOF_DIR)/afmodule.c \
$(AUTOF_DIR)/afwarp.c   \
$(GZIP_DIR)/ftgzip.c \
$(BASE_DIR)/ftadvanc.c \
$(BASE_DIR)/ftcalc.c   \
$(BASE_DIR)/ftdbgmem.c \
$(BASE_DIR)/ftgloadr.c \
$(BASE_DIR)/ftobjs.c   \
$(BASE_DIR)/ftoutln.c  \
$(BASE_DIR)/ftrfork.c  \
$(BASE_DIR)/ftsnames.c \
$(BASE_DIR)/ftstream.c \
$(BASE_DIR)/fttrigon.c \
$(BASE_DIR)/ftutil.c   \
$(PCF_DIR)/pcfdrivr.c \
$(PCF_DIR)/pcfread.c  \
$(PCF_DIR)/pcfutil.c  \
$(T42_DIR)/t42objs.c  \
$(T42_DIR)/t42parse.c \
$(T42_DIR)/t42drivr.c

include $(BUILD_STATIC_LIBRARY)
include $(CLEAR_VARS)
