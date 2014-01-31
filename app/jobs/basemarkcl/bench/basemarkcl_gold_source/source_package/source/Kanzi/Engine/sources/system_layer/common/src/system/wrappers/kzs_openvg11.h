/**
* \file
* OpenVG 1.1 wrappers.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/

#ifndef KZS_OPENVG11_H
#define KZS_OPENVG11_H


#include "kzs_openvg_base.h"

/*System */
#include <system/kzs_types.h>

#include <VG/openvg.h>
#include <VG/vgplatform.h>

/*Lint comment*/

/*lint ++fie*/

#ifndef KZS_VG_MAXSHORT
#define KZS_VG_MAXSHORT VG_MAXSHORT
#endif

#ifndef KZS_VG_MAXINT
#define KZS_VG_MAXINT VG_MAXINT
#endif

#ifndef KZS_VG_MAX_ENUM
#define KZS_VG_MAX_ENUM VG_MAX_ENUM
#endif

typedef VGHandle KzsVGHandle;

typedef VGPath KzsVGPath;
typedef VGImage KzsVGImage;
typedef VGMaskLayer KzsVGMaskLayer;
typedef VGFont KzsVGFont;
typedef VGPaint KzsVGPaint;

typedef VGshort kzVGshort;

#define KZS_VG_INVALID_HANDLE VG_INVALID_HANDLE

typedef enum {
  KZS_VG_FALSE = VG_FALSE,
  KZS_VG_TRUE = VG_TRUE,

  KZS_VG_BOOLEAN_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGboolean;

typedef enum {
  KZS_VG_NO_ERROR = VG_NO_ERROR,
  KZS_VG_BAD_HANDLE_ERROR = VG_BAD_HANDLE_ERROR,
  KZS_VG_ILLEGAL_ARGUMENT_ERROR = VG_ILLEGAL_ARGUMENT_ERROR,
  KZS_VG_OUT_OF_MEMORY_ERROR = VG_OUT_OF_MEMORY_ERROR,
  KZS_VG_PATH_CAPABILITY_ERROR = VG_PATH_CAPABILITY_ERROR,
  KZS_VG_UNSUPPORTED_IMAGE_FORMAT_ERROR = VG_UNSUPPORTED_IMAGE_FORMAT_ERROR,
  KZS_VG_UNSUPPORTED_PATH_FORMAT_ERROR = VG_UNSUPPORTED_PATH_FORMAT_ERROR,
  KZS_VG_IMAGE_IN_USE_ERROR = VG_IMAGE_IN_USE_ERROR,
  KZS_VG_NO_CONTEXT_ERROR = VG_NO_CONTEXT_ERROR,

  KZS_VG_ERROR_CODE_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGErrorCode;

typedef enum {
  /* Mode settings */
  KZS_VG_MATRIX_MODE = VG_MATRIX_MODE,
  KZS_VG_FILL_RULE = VG_FILL_RULE,
  KZS_VG_IMAGE_QUALITY = VG_IMAGE_QUALITY,
  KZS_VG_RENDERING_QUALITY = VG_RENDERING_QUALITY,
  KZS_VG_BLEND_MODE = VG_BLEND_MODE,
  KZS_VG_IMAGE_MODE = VG_IMAGE_MODE,

  /* Scissoring rectangles */
  KZS_VG_SCISSOR_RECTS = VG_SCISSOR_RECTS,

  /* Color Transformation */
  KZS_VG_COLOR_TRANSFORM = VG_COLOR_TRANSFORM,
  KZS_VG_COLOR_TRANSFORM_VALUES = VG_COLOR_TRANSFORM_VALUES,

  /* Stroke parameters */
  KZS_VG_STROKE_LINE_WIDTH = VG_STROKE_LINE_WIDTH,
  KZS_VG_STROKE_CAP_STYLE = VG_STROKE_CAP_STYLE,
  KZS_VG_STROKE_JOIN_STYLE = VG_STROKE_JOIN_STYLE,
  KZS_VG_STROKE_MITER_LIMIT = VG_STROKE_MITER_LIMIT,
  KZS_VG_STROKE_DASH_PATTERN = VG_STROKE_DASH_PATTERN,
  KZS_VG_STROKE_DASH_PHASE = VG_STROKE_DASH_PHASE,
  KZS_VG_STROKE_DASH_PHASE_RESET = VG_STROKE_DASH_PHASE_RESET,

  /* Edge fill color for VG_TILE_FILL tiling mode */
  KZS_VG_TILE_FILL_COLOR = VG_TILE_FILL_COLOR,

  /* Color for vgClear */
  KZS_VG_CLEAR_COLOR = VG_CLEAR_COLOR,

  /* Glyph origin */
  KZS_VG_GLYPH_ORIGIN = VG_GLYPH_ORIGIN,

  /* Enable/disable alpha masking and scissoring */
  KZS_VG_MASKING = VG_MASKING,
  KZS_VG_SCISSORING = VG_SCISSORING,

  /* Pixel layout information */
  KZS_VG_PIXEL_LAYOUT = VG_PIXEL_LAYOUT,
  KZS_VG_SCREEN_LAYOUT = VG_SCREEN_LAYOUT,

  /* Source format selection for image filters */
  KZS_VG_FILTER_FORMAT_LINEAR = VG_FILTER_FORMAT_LINEAR,
  KZS_VG_FILTER_FORMAT_PREMULTIPLIED = VG_FILTER_FORMAT_PREMULTIPLIED,

  /* Destination write enable mask for image filters */
  KZS_VG_FILTER_CHANNEL_MASK = VG_FILTER_CHANNEL_MASK,

  /* Implementation limits (read-only) */
  KZS_VG_MAX_SCISSOR_RECTS = VG_MAX_SCISSOR_RECTS,
  KZS_VG_MAX_DASH_COUNT = VG_MAX_DASH_COUNT,
  KZS_VG_MAX_KERNEL_SIZE = VG_MAX_KERNEL_SIZE,
  KZS_VG_MAX_SEPARABLE_KERNEL_SIZE = VG_MAX_SEPARABLE_KERNEL_SIZE,
  KZS_VG_MAX_COLOR_RAMP_STOPS = VG_MAX_COLOR_RAMP_STOPS,
  KZS_VG_MAX_IMAGE_WIDTH = VG_MAX_IMAGE_WIDTH,
  KZS_VG_MAX_IMAGE_HEIGHT = VG_MAX_IMAGE_HEIGHT,
  KZS_VG_MAX_IMAGE_PIXELS = VG_MAX_IMAGE_PIXELS,
  KZS_VG_MAX_IMAGE_BYTES = VG_MAX_IMAGE_BYTES,
  KZS_VG_MAX_FLOAT = VG_MAX_FLOAT,
  KZS_VG_MAX_GAUSSIAN_STD_DEVIATION = VG_MAX_GAUSSIAN_STD_DEVIATION,

  KZS_VG_PARAM_TYPE_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGParamType;

typedef enum {
  KZS_VG_RENDERING_QUALITY_NONANTIALIASED = VG_RENDERING_QUALITY_NONANTIALIASED,
  KZS_VG_RENDERING_QUALITY_FASTER = VG_RENDERING_QUALITY_FASTER,
  KZS_VG_RENDERING_QUALITY_BETTER = VG_RENDERING_QUALITY_BETTER, /* Default */

  KZS_VG_RENDERING_QUALITY_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGRenderingQuality;

typedef enum {
  KZS_VG_PIXEL_LAYOUT_UNKNOWN = VG_PIXEL_LAYOUT_UNKNOWN,
  KZS_VG_PIXEL_LAYOUT_RGB_VERTICAL = VG_PIXEL_LAYOUT_RGB_VERTICAL,
  KZS_VG_PIXEL_LAYOUT_BGR_VERTICAL = VG_PIXEL_LAYOUT_BGR_VERTICAL,
  KZS_VG_PIXEL_LAYOUT_RGB_HORIZONTAL = VG_PIXEL_LAYOUT_RGB_HORIZONTAL,
  KZS_VG_PIXEL_LAYOUT_BGR_HORIZONTAL = VG_PIXEL_LAYOUT_BGR_HORIZONTAL,

  KZS_VG_PIXEL_LAYOUT_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGPixelLayout;

typedef enum {
  KZS_VG_MATRIX_PATH_USER_TO_SURFACE = VG_MATRIX_PATH_USER_TO_SURFACE,
  KZS_VG_MATRIX_IMAGE_USER_TO_SURFACE = VG_MATRIX_IMAGE_USER_TO_SURFACE,
  KZS_VG_MATRIX_FILL_PAINT_TO_USER = VG_MATRIX_FILL_PAINT_TO_USER,
  KZS_VG_MATRIX_STROKE_PAINT_TO_USER = VG_MATRIX_STROKE_PAINT_TO_USER,
  KZS_VG_MATRIX_GLYPH_USER_TO_SURFACE = VG_MATRIX_GLYPH_USER_TO_SURFACE,

  KZS_VG_MATRIX_MODE_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGMatrixMode;

typedef enum {
  KZS_VG_CLEAR_MASK = VG_CLEAR_MASK,
  KZS_VG_FILL_MASK = VG_FILL_MASK,
  KZS_VG_SET_MASK = VG_SET_MASK,
  KZS_VG_UNION_MASK = VG_UNION_MASK,
  KZS_VG_INTERSECT_MASK = VG_INTERSECT_MASK,
  KZS_VG_SUBTRACT_MASK = VG_SUBTRACT_MASK,

  KZS_VG_MASK_OPERATION_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGMaskOperation;

#define KZS_VG_PATH_FORMAT_STANDARD VG_PATH_FORMAT_STANDARD

typedef enum {
  KZS_VG_PATH_DATATYPE_S_8 = VG_PATH_DATATYPE_S_8,
  KZS_VG_PATH_DATATYPE_S_16 = VG_PATH_DATATYPE_S_16,
  KZS_VG_PATH_DATATYPE_S_32 = VG_PATH_DATATYPE_S_32,
  KZS_VG_PATH_DATATYPE_F = VG_PATH_DATATYPE_F,

  KZS_VG_PATH_DATATYPE_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGPathDatatype;

typedef enum {
  KZS_VG_ABSOLUTE = VG_ABSOLUTE,
  KZS_VG_RELATIVE = VG_RELATIVE,

  KZS_VG_PATH_ABS_REL_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGPathAbsRel;

typedef enum {
  KZS_VG_CLOSE_PATH = VG_CLOSE_PATH,
  KZS_VG_MOVE_TO = VG_MOVE_TO,
  KZS_VG_LINE_TO = VG_LINE_TO,
  KZS_VG_HLINE_TO = VG_HLINE_TO,
  KZS_VG_VLINE_TO = VG_VLINE_TO,
  KZS_VG_QUAD_TO = VG_QUAD_TO,
  KZS_VG_CUBIC_TO = VG_CUBIC_TO,
  KZS_VG_SQUAD_TO = VG_SQUAD_TO,
  KZS_VG_SCUBIC_TO = VG_SCUBIC_TO,
  KZS_VG_SCCWARC_TO = VG_SCCWARC_TO,
  KZS_VG_SCWARC_TO = VG_SCWARC_TO,
  KZS_VG_LCCWARC_TO = VG_LCCWARC_TO,
  KZS_VG_LCWARC_TO = VG_LCWARC_TO,

  KZS_VG_PATH_SEGMENT_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGPathSegment;

typedef enum {
  KZS_VG_MOVE_TO_ABS = VG_MOVE_TO_ABS,
  KZS_VG_MOVE_TO_REL = VG_MOVE_TO_REL,
  KZS_VG_LINE_TO_ABS = VG_LINE_TO_ABS,
  KZS_VG_LINE_TO_REL = VG_LINE_TO_REL,
  KZS_VG_HLINE_TO_ABS = VG_HLINE_TO_ABS,
  KZS_VG_HLINE_TO_REL = VG_HLINE_TO_REL,
  KZS_VG_VLINE_TO_ABS = VG_VLINE_TO_ABS,
  KZS_VG_VLINE_TO_REL = VG_VLINE_TO_REL,
  KZS_VG_QUAD_TO_ABS = VG_QUAD_TO_ABS,
  KZS_VG_QUAD_TO_REL = VG_QUAD_TO_REL,
  KZS_VG_CUBIC_TO_ABS = VG_CUBIC_TO_ABS,
  KZS_VG_CUBIC_TO_REL = VG_CUBIC_TO_REL,
  KZS_VG_SQUAD_TO_ABS = VG_SQUAD_TO_ABS,
  KZS_VG_SQUAD_TO_REL = VG_SQUAD_TO_REL,
  KZS_VG_SCUBIC_TO_ABS = VG_SCUBIC_TO_ABS,
  KZS_VG_SCUBIC_TO_REL = VG_SCUBIC_TO_REL,
  KZS_VG_SCCWARC_TO_ABS = VG_SCCWARC_TO_ABS,
  KZS_VG_SCCWARC_TO_REL = VG_SCCWARC_TO_REL,
  KZS_VG_SCWARC_TO_ABS = VG_SCWARC_TO_ABS,
  KZS_VG_SCWARC_TO_REL = VG_SCWARC_TO_REL,
  KZS_VG_LCCWARC_TO_ABS = VG_LCCWARC_TO_ABS,
  KZS_VG_LCCWARC_TO_REL = VG_LCCWARC_TO_REL,
  KZS_VG_LCWARC_TO_ABS = VG_LCWARC_TO_ABS,
  KZS_VG_LCWARC_TO_REL = VG_LCWARC_TO_REL,

  KZS_VG_PATH_COMMAND_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGPathCommand;

typedef enum {
  KZS_VG_PATH_CAPABILITY_APPEND_FROM = VG_PATH_CAPABILITY_APPEND_FROM,
  KZS_VG_PATH_CAPABILITY_APPEND_TO = VG_PATH_CAPABILITY_APPEND_TO,
  KZS_VG_PATH_CAPABILITY_MODIFY = VG_PATH_CAPABILITY_MODIFY,
  KZS_VG_PATH_CAPABILITY_TRANSFORM_FROM = VG_PATH_CAPABILITY_TRANSFORM_FROM,
  KZS_VG_PATH_CAPABILITY_TRANSFORM_TO = VG_PATH_CAPABILITY_TRANSFORM_TO,
  KZS_VG_PATH_CAPABILITY_INTERPOLATE_FROM = VG_PATH_CAPABILITY_INTERPOLATE_FROM,
  KZS_VG_PATH_CAPABILITY_INTERPOLATE_TO = VG_PATH_CAPABILITY_INTERPOLATE_TO,
  KZS_VG_PATH_CAPABILITY_PATH_LENGTH = VG_PATH_CAPABILITY_PATH_LENGTH,
  KZS_VG_PATH_CAPABILITY_POINT_ALONG_PATH = VG_PATH_CAPABILITY_POINT_ALONG_PATH,
  KZS_VG_PATH_CAPABILITY_TANGENT_ALONG_PATH = VG_PATH_CAPABILITY_TANGENT_ALONG_PATH,
  KZS_VG_PATH_CAPABILITY_PATH_BOUNDS = VG_PATH_CAPABILITY_PATH_BOUNDS,
  KZS_VG_PATH_CAPABILITY_PATH_TRANSFORMED_BOUNDS = VG_PATH_CAPABILITY_PATH_TRANSFORMED_BOUNDS,
  KZS_VG_PATH_CAPABILITY_ALL = VG_PATH_CAPABILITY_ALL,

  KZS_VG_PATH_CAPABILITIES_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGPathCapabilities;

typedef enum {
  KZS_VG_PATH_FORMAT = VG_PATH_FORMAT,
  KZS_VG_PATH_DATATYPE = VG_PATH_DATATYPE,
  KZS_VG_PATH_SCALE = VG_PATH_SCALE,
  KZS_VG_PATH_BIAS = VG_PATH_BIAS,
  KZS_VG_PATH_NUM_SEGMENTS = VG_PATH_NUM_SEGMENTS,
  KZS_VG_PATH_NUM_COORDS = VG_PATH_NUM_COORDS,

  KZS_VG_PATH_PARAM_TYPE_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGPathParamType;

typedef enum {
  KZS_VG_CAP_BUTT = VG_CAP_BUTT,
  KZS_VG_CAP_ROUND = VG_CAP_ROUND,
  KZS_VG_CAP_SQUARE = VG_CAP_SQUARE,

  KZS_VG_CAP_STYLE_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGCapStyle;

typedef enum {
  KZS_VG_JOIN_MITER = VG_JOIN_MITER,
  KZS_VG_JOIN_ROUND = VG_JOIN_ROUND,
  KZS_VG_JOIN_BEVEL = VG_JOIN_BEVEL,

  KZS_VG_JOIN_STYLE_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGJoinStyle;

typedef enum {
  KZS_VG_EVEN_ODD = VG_EVEN_ODD,
  KZS_VG_NON_ZERO = VG_NON_ZERO,

  KZS_VG_FILL_RULE_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGFillRule;

typedef enum {
  KZS_VG_STROKE_PATH = VG_STROKE_PATH,
  KZS_VG_FILL_PATH = VG_FILL_PATH,

  KZS_VG_PAINT_MODE_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGPaintMode;

typedef enum {
  /* Color paint parameters */
  KZS_VG_PAINT_TYPE = VG_PAINT_TYPE,
  KZS_VG_PAINT_COLOR = VG_PAINT_COLOR,
  KZS_VG_PAINT_COLOR_RAMP_SPREAD_MODE = VG_PAINT_COLOR_RAMP_SPREAD_MODE,
  KZS_VG_PAINT_COLOR_RAMP_PREMULTIPLIED = VG_PAINT_COLOR_RAMP_PREMULTIPLIED,
  KZS_VG_PAINT_COLOR_RAMP_STOPS = VG_PAINT_COLOR_RAMP_STOPS,

  /* Linear gradient paint parameters */
  KZS_VG_PAINT_LINEAR_GRADIENT = VG_PAINT_LINEAR_GRADIENT,

  /* Radial gradient paint parameters */
  KZS_VG_PAINT_RADIAL_GRADIENT = VG_PAINT_RADIAL_GRADIENT,

  /* Pattern paint parameters */
  KZS_VG_PAINT_PATTERN_TILING_MODE = VG_PAINT_PATTERN_TILING_MODE,

  KZS_VG_PAINT_PARAM_TYPE_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGPaintParamType;

typedef enum {
  KZS_VG_PAINT_TYPE_COLOR = VG_PAINT_TYPE_COLOR,
  KZS_VG_PAINT_TYPE_LINEAR_GRADIENT = VG_PAINT_TYPE_LINEAR_GRADIENT,
  KZS_VG_PAINT_TYPE_RADIAL_GRADIENT = VG_PAINT_TYPE_RADIAL_GRADIENT,
  KZS_VG_PAINT_TYPE_PATTERN = VG_PAINT_TYPE_PATTERN,

  KZS_VG_PAINT_TYPE_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGPaintType;

typedef enum {
  KZS_VG_COLOR_RAMP_SPREAD_PAD = VG_COLOR_RAMP_SPREAD_PAD,
  KZS_VG_COLOR_RAMP_SPREAD_REPEAT = VG_COLOR_RAMP_SPREAD_REPEAT,
  KZS_VG_COLOR_RAMP_SPREAD_REFLECT = VG_COLOR_RAMP_SPREAD_REFLECT,

  KZS_VG_COLOR_RAMP_SPREAD_MODE_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGColorRampSpreadMode;

typedef enum {
  KZS_VG_TILE_FILL = VG_TILE_FILL,
  KZS_VG_TILE_PAD = VG_TILE_PAD,
  KZS_VG_TILE_REPEAT = VG_TILE_REPEAT,
  KZS_VG_TILE_REFLECT = VG_TILE_REFLECT,

  KZS_VG_TILING_MODE_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGTilingMode;

typedef enum {
  /* RGB{A,X} channel ordering */
  KZS_VG_sRGBX_8888 = VG_sRGBX_8888,
  KZS_VG_sRGBA_8888 = VG_sRGBA_8888,
  KZS_VG_sRGBA_8888_PRE = VG_sRGBA_8888_PRE,
  KZS_VG_sRGB_565 = VG_sRGB_565,
  KZS_VG_sRGBA_5551 = VG_sRGBA_5551,
  KZS_VG_sRGBA_4444 = VG_sRGBA_4444,
  KZS_VG_sL_8 = VG_sL_8,
  KZS_VG_lRGBX_8888 = VG_lRGBX_8888,
  KZS_VG_lRGBA_8888 = VG_lRGBA_8888,
  KZS_VG_lRGBA_8888_PRE = VG_lRGBA_8888_PRE,
  KZS_VG_lL_8 = VG_lL_8,
  KZS_VG_A_8 = VG_A_8,
  KZS_VG_BW_1 = VG_BW_1,
  KZS_VG_A_1 = VG_A_1,
  KZS_VG_A_4 = VG_A_4,

  /* {A,X}RGB channel ordering */
  KZS_VG_sXRGB_8888 = VG_sXRGB_8888,
  KZS_VG_sARGB_8888 = VG_sARGB_8888,
  KZS_VG_sARGB_8888_PRE = VG_sARGB_8888_PRE,
  KZS_VG_sARGB_1555 = VG_sARGB_1555,
  KZS_VG_sARGB_4444 = VG_sARGB_4444,
  KZS_VG_lXRGB_8888 = VG_lXRGB_8888,
  KZS_VG_lARGB_8888 = VG_lARGB_8888,
  KZS_VG_lARGB_8888_PRE = VG_lARGB_8888_PRE,

  /* BGR{A,X} channel ordering */
  KZS_VG_sBGRX_8888 = VG_sBGRX_8888,
  KZS_VG_sBGRA_8888 = VG_sBGRA_8888,
  KZS_VG_sBGRA_8888_PRE = VG_sBGRA_8888_PRE,
  KZS_VG_sBGR_565 = VG_sBGR_565,
  KZS_VG_sBGRA_5551 = VG_sBGRA_5551,
  KZS_VG_sBGRA_4444 = VG_sBGRA_4444,
  KZS_VG_lBGRX_8888 = VG_lBGRX_8888,
  KZS_VG_lBGRA_8888 = VG_lBGRA_8888,
  KZS_VG_lBGRA_8888_PRE = VG_lBGRA_8888_PRE,

  /* {A,X}BGR channel ordering */
  KZS_VG_sXBGR_8888 = VG_sXBGR_8888,
  KZS_VG_sABGR_8888 = VG_sABGR_8888,
  KZS_VG_sABGR_8888_PRE = VG_sABGR_8888_PRE,
  KZS_VG_sABGR_1555 = VG_sABGR_1555,
  KZS_VG_sABGR_4444 = VG_sABGR_4444,
  KZS_VG_lXBGR_8888 = VG_lXBGR_8888,
  KZS_VG_lABGR_8888 = VG_lABGR_8888,
  KZS_VG_lABGR_8888_PRE = VG_lABGR_8888_PRE,

  KZS_VG_IMAGE_FORMAT_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGImageFormat;

typedef enum {
  KZS_VG_IMAGE_QUALITY_NONANTIALIASED = VG_IMAGE_QUALITY_NONANTIALIASED,
  KZS_VG_IMAGE_QUALITY_FASTER = VG_IMAGE_QUALITY_FASTER,
  KZS_VG_IMAGE_QUALITY_BETTER = VG_IMAGE_QUALITY_BETTER,

  KZS_VG_IMAGE_QUALITY_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGImageQuality;

typedef enum {
  KZS_VG_IMAGE_FORMAT = VG_IMAGE_FORMAT,
  KZS_VG_IMAGE_WIDTH = VG_IMAGE_WIDTH,
  KZS_VG_IMAGE_HEIGHT = VG_IMAGE_HEIGHT,

  KZS_VG_IMAGE_PARAM_TYPE_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGImageParamType;

typedef enum {
  KZS_VG_DRAW_IMAGE_NORMAL = VG_DRAW_IMAGE_NORMAL,
  KZS_VG_DRAW_IMAGE_MULTIPLY = VG_DRAW_IMAGE_MULTIPLY,
  KZS_VG_DRAW_IMAGE_STENCIL = VG_DRAW_IMAGE_STENCIL,

  KZS_VG_IMAGE_MODE_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGImageMode;

typedef enum {
  KZS_VG_RED = VG_RED,
  KZS_VG_GREEN = VG_GREEN,
  KZS_VG_BLUE = VG_BLUE,
  KZS_VG_ALPHA = VG_ALPHA,

  KZS_VG_IMAGE_CHANNEL_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGImageChannel;

typedef enum {
  KZS_VG_BLEND_SRC = VG_BLEND_SRC,
  KZS_VG_BLEND_SRC_OVER = VG_BLEND_SRC_OVER,
  KZS_VG_BLEND_DST_OVER = VG_BLEND_DST_OVER,
  KZS_VG_BLEND_SRC_IN = VG_BLEND_SRC_IN,
  KZS_VG_BLEND_DST_IN = VG_BLEND_DST_IN,
  KZS_VG_BLEND_MULTIPLY = VG_BLEND_MULTIPLY,
  KZS_VG_BLEND_SCREEN = VG_BLEND_SCREEN,
  KZS_VG_BLEND_DARKEN = VG_BLEND_DARKEN,
  KZS_VG_BLEND_LIGHTEN = VG_BLEND_LIGHTEN,
  KZS_VG_BLEND_ADDITIVE = VG_BLEND_ADDITIVE,

  KZS_VG_BLEND_MODE_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGBlendMode;

typedef enum {
  KZS_VG_FONT_NUM_GLYPHS = VG_FONT_NUM_GLYPHS,

  KZS_VG_FONT_PARAM_TYPE_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGFontParamType;

typedef enum {
  KZS_VG_IMAGE_FORMAT_QUERY = VG_IMAGE_FORMAT_QUERY,
  KZS_VG_PATH_DATATYPE_QUERY = VG_PATH_DATATYPE_QUERY,

  KZS_VG_HARDWARE_QUERY_TYPE_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGHardwareQueryType;

typedef enum {
  KZS_VG_HARDWARE_ACCELERATED = VG_HARDWARE_ACCELERATED,
  KZS_VG_HARDWARE_UNACCELERATED = VG_HARDWARE_UNACCELERATED,

  KZS_VG_HARDWARE_QUERY_RESULT_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGHardwareQueryResult;

typedef enum {
  KZS_VG_VENDOR = VG_VENDOR,
  KZS_VG_RENDERER = VG_RENDERER,
  KZS_VG_VERSION = VG_VERSION,
  KZS_VG_EXTENSIONS = VG_EXTENSIONS,

  KZS_VG_STRING_ID_FORCE_SIZE = KZS_VG_MAX_ENUM
} KzsVGStringID;


/* Function Prototypes */


KZ_INLINE KzsVGErrorCode kzsVgGetError(void) 
{
KZS_OPENVG_CALL_BEGIN
    return vgGetError();
KZS_OPENVG_CALL_END_RETURN_VALUE(KZS_VG_NO_ERROR)
}

KZ_INLINE void kzsVgFlush(void) 
{
KZS_OPENVG_CALL_BEGIN
    vgFlush();
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgFinish(void) 
{
KZS_OPENVG_CALL_BEGIN
    vgFinish();
KZS_OPENVG_CALL_END
}

/* Getters and Setters */
KZ_INLINE void kzsVgSetf (KzsVGParamType type, kzFloat value) 
{
KZS_OPENVG_CALL_BEGIN
    vgSetf(type, value);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgSeti (KzsVGParamType type, kzInt value) 
{
KZS_OPENVG_CALL_BEGIN
    vgSeti(type, value);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgSetfv(KzsVGParamType type, kzInt count,
                         const kzFloat * values) 
{
KZS_OPENVG_CALL_BEGIN
    vgSetfv(type, count, values);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgSetiv(KzsVGParamType type, kzInt count,
                         const kzInt * values) 
{
KZS_OPENVG_CALL_BEGIN
    vgSetiv(type, count, values);
KZS_OPENVG_CALL_END
}

KZ_INLINE kzFloat kzsVgGetf(KzsVGParamType type) 
{
KZS_OPENVG_CALL_BEGIN
    return vgGetf(type);
KZS_OPENVG_CALL_END_RETURN_VALUE(1.0f)
}

KZ_INLINE kzInt kzsVgGeti(KzsVGParamType type) 
{
KZS_OPENVG_CALL_BEGIN
    return vgGeti(type);
KZS_OPENVG_CALL_END_RETURN_VALUE(0)
}

KZ_INLINE kzInt kzsVgGetVectorSize(KzsVGParamType type) 
{
KZS_OPENVG_CALL_BEGIN
    return vgGetVectorSize(type);
KZS_OPENVG_CALL_END_RETURN_VALUE(0)
}

KZ_INLINE void kzsVgGetfv(KzsVGParamType type, kzInt count, kzFloat * values) 
{
KZS_OPENVG_CALL_BEGIN
    vgGetfv(type, count, values);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgGetiv(KzsVGParamType type, kzInt count, kzInt * values) 
{
KZS_OPENVG_CALL_BEGIN
    vgGetiv(type, count, values);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgSetParameterf(KzsVGHandle object,
                                 kzInt paramType,
                                 kzFloat value) 
{
KZS_OPENVG_CALL_BEGIN
    vgSetParameterf(object, paramType, value);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgSetParameteri(KzsVGHandle object,
                                 kzInt paramType,
                                 kzInt value) 
{
KZS_OPENVG_CALL_BEGIN
    vgSetParameteri(object, paramType, value);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgSetParameterfv(KzsVGHandle object,
                                  kzInt paramType,
                                  kzInt count, const kzFloat * values) 
{
KZS_OPENVG_CALL_BEGIN
    vgSetParameterfv(object, paramType, count, values);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgSetParameteriv(KzsVGHandle object,
                                  kzInt paramType,
                                  kzInt count, const kzInt* values) 
{
KZS_OPENVG_CALL_BEGIN
    vgSetParameteriv(object, paramType, count, values);
KZS_OPENVG_CALL_END
}

KZ_INLINE kzFloat kzsVgGetParameterf(KzsVGHandle object,
                                    kzInt paramType) 
{
KZS_OPENVG_CALL_BEGIN
    return vgGetParameterf(object, paramType);
KZS_OPENVG_CALL_END_RETURN_VALUE(1.0f)
}

KZ_INLINE kzInt kzsVgGetParameteri(KzsVGHandle object,
                                  kzInt paramType)
{
KZS_OPENVG_CALL_BEGIN
    return vgGetParameteri(object, paramType);
KZS_OPENVG_CALL_END_RETURN_VALUE(0)
}

KZ_INLINE kzInt kzsVgGetParameterVectorSize(KzsVGHandle object,
                                           kzInt paramType) 
{
KZS_OPENVG_CALL_BEGIN
    return vgGetParameterVectorSize(object, paramType);
KZS_OPENVG_CALL_END_RETURN_VALUE(0)
}

KZ_INLINE void kzsVgGetParameterfv(KzsVGHandle object,
                                  kzInt paramType,
                                  kzInt count, kzFloat* values) 
{
KZS_OPENVG_CALL_BEGIN
    vgGetParameterfv(object, paramType, count, values);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgGetParameteriv(KzsVGHandle object,
                                  kzInt paramType,
                                  kzInt count, kzInt* values) 
{
KZS_OPENVG_CALL_BEGIN
    vgGetParameteriv(object, paramType, count, values);
KZS_OPENVG_CALL_END
}

/* Matrix Manipulation */
KZ_INLINE void kzsVgLoadIdentity(void) 
{
KZS_OPENVG_CALL_BEGIN
    vgLoadIdentity();
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgLoadMatrix(const kzFloat* m) 
{
KZS_OPENVG_CALL_BEGIN
    vgLoadMatrix(m);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgGetMatrix(kzFloat* m) 
{
KZS_OPENVG_CALL_BEGIN
    vgGetMatrix(m);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgMultMatrix(const kzFloat* m) 
{
KZS_OPENVG_CALL_BEGIN
    vgMultMatrix(m);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgTranslate(kzFloat tx, kzFloat ty) 
{
KZS_OPENVG_CALL_BEGIN
    vgTranslate(tx, ty);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgScale(kzFloat sx, kzFloat sy) 
{
KZS_OPENVG_CALL_BEGIN
    vgScale(sx, sy);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgShear(kzFloat shx, kzFloat shy) 
{
KZS_OPENVG_CALL_BEGIN
    vgShear(shx, shy);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgRotate(kzFloat angle) 
{
KZS_OPENVG_CALL_BEGIN
    vgRotate(angle);
KZS_OPENVG_CALL_END
}

/* Masking and Clearing */
KZ_INLINE void kzsVgMask(KzsVGHandle mask, KzsVGMaskOperation operation,
                                     kzInt x, kzInt y,
                                     kzInt width, kzInt height) 
{
KZS_OPENVG_CALL_BEGIN
    vgMask(mask, operation, x, y, width, height);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgRenderToMask(KzsVGPath path,
                                            kzUint paintModes,
                                            KzsVGMaskOperation operation) 
{
KZS_OPENVG_CALL_BEGIN
    vgRenderToMask(path, paintModes, operation);
KZS_OPENVG_CALL_END
}

KZ_INLINE KzsVGMaskLayer kzsVgCreateMaskLayer(kzInt width, kzInt height) 
{
KZS_OPENVG_CALL_BEGIN
    return vgCreateMaskLayer(width, height);
KZS_OPENVG_CALL_END_RETURN_VALUE(0)
}

KZ_INLINE void kzsVgDestroyMaskLayer(KzsVGMaskLayer maskLayer) 
{
KZS_OPENVG_CALL_BEGIN
    vgDestroyMaskLayer(maskLayer);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgFillMaskLayer(KzsVGMaskLayer maskLayer,
                                             kzInt x, kzInt y,
                                             kzInt width, kzInt height,
                                             kzFloat value) 
{
KZS_OPENVG_CALL_BEGIN
    vgFillMaskLayer(maskLayer, x, y, width, height, value);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgCopyMask(KzsVGMaskLayer maskLayer,
                                        kzInt dx, kzInt dy,
                                        kzInt sx, kzInt sy,
                                        kzInt width, kzInt height) 
{
KZS_OPENVG_CALL_BEGIN
    vgCopyMask(maskLayer, dx, dy, sx, sy, width, height);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgClear(kzInt x, kzInt y, kzInt width, kzInt height) 
{
KZS_OPENVG_CALL_BEGIN
    vgClear(x, y, width, height);
KZS_OPENVG_CALL_END
}

/* Paths */
KZ_INLINE KzsVGPath kzsVgCreatePath(kzInt pathFormat,
                                KzsVGPathDatatype datatype,
                                kzFloat scale, kzFloat bias,
                                kzInt segmentCapacityHint,
                                kzInt coordCapacityHint,
                                kzUint capabilities) 
{
KZS_OPENVG_CALL_BEGIN
    return vgCreatePath(pathFormat, datatype, scale, bias,
            segmentCapacityHint, coordCapacityHint, capabilities);
KZS_OPENVG_CALL_END_RETURN_VALUE(0)
}

KZ_INLINE void kzsVgClearPath(KzsVGPath path, kzUint capabilities) 
{
KZS_OPENVG_CALL_BEGIN
    vgClearPath(path, capabilities);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgDestroyPath(KzsVGPath path) 
{
KZS_OPENVG_CALL_BEGIN
    vgDestroyPath(path);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgRemovePathCapabilities(KzsVGPath path,
                                          kzUint capabilities) 
{
KZS_OPENVG_CALL_BEGIN
    vgRemovePathCapabilities(path, capabilities);
KZS_OPENVG_CALL_END
}

KZ_INLINE kzUint kzsVgGetPathCapabilities(KzsVGPath path) 
{
KZS_OPENVG_CALL_BEGIN
    return vgGetPathCapabilities(path);
KZS_OPENVG_CALL_END_RETURN_VALUE(0)
}

KZ_INLINE void kzsVgAppendPath(KzsVGPath dstPath, KzsVGPath srcPath) 
{
KZS_OPENVG_CALL_BEGIN
    vgAppendPath(dstPath, srcPath);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgAppendPathData(KzsVGPath dstPath,
                                  kzInt numSegments,
                                  const kzU8* pathSegments,
                                  const void* pathData) 
{
KZS_OPENVG_CALL_BEGIN
    vgAppendPathData(dstPath, numSegments, pathSegments, pathData);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgModifyPathCoords(KzsVGPath dstPath, kzInt startIndex,
                                    kzInt numSegments,
                                    const void* pathData) 
{
KZS_OPENVG_CALL_BEGIN
    vgModifyPathCoords(dstPath, startIndex, numSegments, pathData);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgTransformPath(KzsVGPath dstPath, KzsVGPath srcPath) 
{
KZS_OPENVG_CALL_BEGIN
    vgTransformPath(dstPath, srcPath);
KZS_OPENVG_CALL_END
}

KZ_INLINE kzBool kzsVgInterpolatePath(KzsVGPath dstPath,
                                        KzsVGPath startPath,
                                        KzsVGPath endPath,
                                        kzFloat amount) 
{
KZS_OPENVG_CALL_BEGIN
    return vgInterpolatePath(dstPath, startPath, endPath, amount);
KZS_OPENVG_CALL_END_RETURN_VALUE(KZ_FALSE)
}

KZ_INLINE kzFloat kzsVgPathLength(KzsVGPath path,
                                 kzInt startSegment, kzInt numSegments) 
{
KZS_OPENVG_CALL_BEGIN
    return vgPathLength(path, startSegment, numSegments);
KZS_OPENVG_CALL_END_RETURN_VALUE(1.0f)
}

KZ_INLINE void kzsVgPointAlongPath(KzsVGPath path,
                                  kzInt startSegment, kzInt numSegments,
                                  kzFloat distance,
                                  kzFloat* x, kzFloat* y,
                                  kzFloat* tangentX, kzFloat* tangentY) 
{
KZS_OPENVG_CALL_BEGIN
    vgPointAlongPath(path, startSegment, numSegments, distance, x, y, tangentX, tangentY);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgPathBounds(KzsVGPath path,
                              kzFloat* minX, kzFloat* minY,
                              kzFloat* width, kzFloat* height) 
{
KZS_OPENVG_CALL_BEGIN
    vgPathBounds(path, minX, minY, width, height);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgPathTransformedBounds(KzsVGPath path,
                                         kzFloat* minX, kzFloat* minY,
                                         kzFloat* width, kzFloat* height) 
{
KZS_OPENVG_CALL_BEGIN
    vgPathTransformedBounds(path, minX, minY, width, height);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgDrawPath(KzsVGPath path, kzUint paintModes) 
{
KZS_OPENVG_CALL_BEGIN
    vgDrawPath(path, paintModes);
KZS_OPENVG_CALL_END
}

/* Paint */
KZ_INLINE KzsVGPaint kzsVgCreatePaint(void) 
{
KZS_OPENVG_CALL_BEGIN
    return vgCreatePaint();
KZS_OPENVG_CALL_END_RETURN_VALUE(0)
}

KZ_INLINE void kzsVgDestroyPaint(KzsVGPaint paint) 
{
KZS_OPENVG_CALL_BEGIN
    vgDestroyPaint(paint);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgSetPaint(KzsVGPaint paint, kzUint paintModes) 
{
KZS_OPENVG_CALL_BEGIN
    vgSetPaint(paint, paintModes);
KZS_OPENVG_CALL_END
}

KZ_INLINE KzsVGPaint kzsVgGetPaint(KzsVGPaintMode paintMode) 
{
KZS_OPENVG_CALL_BEGIN
    return vgGetPaint(paintMode);
KZS_OPENVG_CALL_END_RETURN_VALUE(0)
}

KZ_INLINE void kzsVgSetColor(KzsVGPaint paint, kzUint rgba) 
{
KZS_OPENVG_CALL_BEGIN
    vgSetColor(paint, rgba);
KZS_OPENVG_CALL_END
}

KZ_INLINE kzUint kzsVgGetColor(KzsVGPaint paint) 
{
KZS_OPENVG_CALL_BEGIN
    return vgGetColor(paint);
KZS_OPENVG_CALL_END_RETURN_VALUE(0)
}

KZ_INLINE void kzsVgPaintPattern(KzsVGPaint paint, KzsVGImage pattern) 
{
KZS_OPENVG_CALL_BEGIN
    vgPaintPattern(paint, pattern);
KZS_OPENVG_CALL_END
}

/* Images */
KZ_INLINE KzsVGImage kzsVgCreateImage(KzsVGImageFormat format,
                                  kzInt width, kzInt height,
                                  kzUint allowedQuality) 
{
KZS_OPENVG_CALL_BEGIN
    return vgCreateImage(format, width, height, allowedQuality);
KZS_OPENVG_CALL_END_RETURN_VALUE(0)
}

KZ_INLINE void kzsVgDestroyImage(KzsVGImage image) 
{
KZS_OPENVG_CALL_BEGIN
    vgDestroyImage(image);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgClearImage(KzsVGImage image,
                              kzInt x, kzInt y, kzInt width, kzInt height) 
{
KZS_OPENVG_CALL_BEGIN
    vgClearImage(image, x, y, width, height);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgImageSubData(KzsVGImage image,
                                const void * data, kzInt dataStride,
                                KzsVGImageFormat dataFormat,
                                kzInt x, kzInt y, kzInt width, kzInt height) 
{
KZS_OPENVG_CALL_BEGIN
    vgImageSubData(image, data, dataStride, dataFormat, x, y, width, height);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgGetImageSubData(KzsVGImage image,
                                   void * data, kzInt dataStride,
                                   KzsVGImageFormat dataFormat,
                                   kzInt x, kzInt y,
                                   kzInt width, kzInt height) 
{
KZS_OPENVG_CALL_BEGIN
    vgGetImageSubData(image, data, dataStride, dataFormat, x, y, width, height);
KZS_OPENVG_CALL_END
}

KZ_INLINE KzsVGImage kzsVgChildImage(KzsVGImage parent,
                                 kzInt x, kzInt y, kzInt width, kzInt height) 
{
KZS_OPENVG_CALL_BEGIN
    return vgChildImage(parent, x, y, width, height);
KZS_OPENVG_CALL_END_RETURN_VALUE(0)
}

KZ_INLINE KzsVGImage kzsVgGetParent(KzsVGImage image) 
{
KZS_OPENVG_CALL_BEGIN
    return vgGetParent(image);
KZS_OPENVG_CALL_END_RETURN_VALUE(0)
} 

KZ_INLINE void kzsVgCopyImage(KzsVGImage dst, kzInt dx, kzInt dy,
                             KzsVGImage src, kzInt sx, kzInt sy,
                             kzInt width, kzInt height,
                             kzBool dither) 
{
KZS_OPENVG_CALL_BEGIN
    vgCopyImage(dst, dx, dy, src, sx, sy, width, height, dither);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgDrawImage(KzsVGImage image) 
{
KZS_OPENVG_CALL_BEGIN
    vgDrawImage(image);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgSetPixels(kzInt dx, kzInt dy,
                             KzsVGImage src, kzInt sx, kzInt sy,
                             kzInt width, kzInt height) 
{
KZS_OPENVG_CALL_BEGIN
    vgSetPixels(dx, dy, src, sx, sy, width, height);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgWritePixels(const void * data, kzInt dataStride,
                               KzsVGImageFormat dataFormat,
                               kzInt dx, kzInt dy,
                               kzInt width, kzInt height) 
{
KZS_OPENVG_CALL_BEGIN
    vgWritePixels(data, dataStride, dataFormat, dx, dy, width, height);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgGetPixels(KzsVGImage dst, kzInt dx, kzInt dy,
                             kzInt sx, kzInt sy,
                             kzInt width, kzInt height) 
{
KZS_OPENVG_CALL_BEGIN
    vgGetPixels(dst, dx, dy, sx, sy, width, height);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgReadPixels(void * data, kzInt dataStride,
                              KzsVGImageFormat dataFormat,
                              kzInt sx, kzInt sy,
                              kzInt width, kzInt height) 
{
KZS_OPENVG_CALL_BEGIN
    vgReadPixels(data, dataStride, dataFormat, sx, sy, width, height);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgCopyPixels(kzInt dx, kzInt dy,
                              kzInt sx, kzInt sy,
                              kzInt width, kzInt height) 
{
KZS_OPENVG_CALL_BEGIN
    vgCopyPixels(dx, dy, sx, sy, width, height);
KZS_OPENVG_CALL_END
}

/* Text */
KZ_INLINE KzsVGFont kzsVgCreateFont(kzInt glyphCapacityHint) 
{
KZS_OPENVG_CALL_BEGIN
    return vgCreateFont(glyphCapacityHint);
KZS_OPENVG_CALL_END_RETURN_VALUE(0)
}

KZ_INLINE void kzsVgDestroyFont(KzsVGFont font) 
{
KZS_OPENVG_CALL_BEGIN
    vgDestroyFont(font);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgSetGlyphToPath(KzsVGFont font,
                                              kzUint glyphIndex,
                                              KzsVGPath path,
                                              kzBool isHinted,
                                              kzFloat glyphOrigin [2],
                                              kzFloat escapement[2]) 
{
KZS_OPENVG_CALL_BEGIN
    vgSetGlyphToPath(font, glyphIndex, path, isHinted, glyphOrigin, escapement);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgSetGlyphToImage(KzsVGFont font,
                                               kzUint glyphIndex,
                                               KzsVGImage image,
                                               kzFloat glyphOrigin [2],
                                               kzFloat escapement[2]) 
{
KZS_OPENVG_CALL_BEGIN
    vgSetGlyphToImage(font, glyphIndex, image, glyphOrigin, escapement);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgClearGlyph(KzsVGFont font,kzUint glyphIndex) 
{
KZS_OPENVG_CALL_BEGIN
    vgClearGlyph(font, glyphIndex);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgDrawGlyph(KzsVGFont font, 
                                         kzUint glyphIndex,
                                         kzUint paintModes,
                                         kzBool allowAutoHinting) 
{
KZS_OPENVG_CALL_BEGIN
    vgDrawGlyph(font, glyphIndex, paintModes, allowAutoHinting);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgDrawGlyphs(KzsVGFont font,
                                          kzInt glyphCount,
                                          kzUint *glyphIndices,
                                          kzFloat *adjustments_x,
                                          kzFloat *adjustments_y,
                                          kzUint paintModes,
                                          kzBool allowAutoHinting) 
{
KZS_OPENVG_CALL_BEGIN
    vgDrawGlyphs(font, glyphCount, glyphIndices, adjustments_x, adjustments_y, paintModes, allowAutoHinting);
KZS_OPENVG_CALL_END
}

/* Image Filters */
KZ_INLINE void kzsVgColorMatrix(KzsVGImage dst, KzsVGImage src,
                               const kzFloat * matrix) 
{
KZS_OPENVG_CALL_BEGIN
    vgColorMatrix(dst, src, matrix);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgConvolve(KzsVGImage dst, KzsVGImage src,
                            kzInt kernelWidth, kzInt kernelHeight,
                            kzInt shiftX, kzInt shiftY,
                            const kzVGshort* kernel,
                            kzFloat scale,
                            kzFloat bias,
                            KzsVGTilingMode tilingMode) 
{
KZS_OPENVG_CALL_BEGIN
    vgConvolve(dst, src, kernelWidth, kernelHeight, shiftX, shiftY, kernel, scale, bias, tilingMode);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgSeparableConvolve(KzsVGImage dst, KzsVGImage src,
                                     kzInt kernelWidth,
                                     kzInt kernelHeight,
                                     kzInt shiftX, kzInt shiftY,
                                     const kzVGshort* kernelX,
                                     const kzVGshort* kernelY,
                                     kzFloat scale,
                                     kzFloat bias,
                                     KzsVGTilingMode tilingMode) 
{
KZS_OPENVG_CALL_BEGIN
    vgSeparableConvolve(dst, src, kernelWidth, kernelHeight, shiftX, shiftY, kernelX, kernelY, scale, bias, tilingMode);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgGaussianBlur(KzsVGImage dst, KzsVGImage src,
                                kzFloat stdDeviationX,
                                kzFloat stdDeviationY,
                                KzsVGTilingMode tilingMode) 
{
KZS_OPENVG_CALL_BEGIN
    vgGaussianBlur(dst, src, stdDeviationX, stdDeviationY, tilingMode);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgLookup(KzsVGImage dst, KzsVGImage src,
                          const kzU8 * redLUT,
                          const kzU8 * greenLUT,
                          const kzU8 * blueLUT,
                          const kzU8 * alphaLUT,
                          kzBool outputLinear,
                          kzBool outputPremultiplied) 
{
KZS_OPENVG_CALL_BEGIN
    vgLookup(dst, src, redLUT, greenLUT, blueLUT, alphaLUT, outputLinear, outputPremultiplied);
KZS_OPENVG_CALL_END
}

KZ_INLINE void kzsVgLookupSingle(KzsVGImage dst, KzsVGImage src,
                                const kzUint* lookupTable,
                                KzsVGImageChannel sourceChannel,
                                kzBool outputLinear,
                                kzBool outputPremultiplied) 
{
KZS_OPENVG_CALL_BEGIN
    vgLookupSingle(dst, src, lookupTable, sourceChannel, outputLinear, outputPremultiplied);
KZS_OPENVG_CALL_END
}

/* Hardware Queries */
KZ_INLINE KzsVGHardwareQueryResult kzsVgHardwareQuery(KzsVGHardwareQueryType key,
                                                  kzInt setting) 
{
KZS_OPENVG_CALL_BEGIN
    return vgHardwareQuery(key,setting);
KZS_OPENVG_CALL_END_RETURN_VALUE(KZS_VG_HARDWARE_UNACCELERATED)
}

/* Renderer and Extension Information */
KZ_INLINE const kzU8* kzsVgGetString(KzsVGStringID name) 
{
KZS_OPENVG_CALL_BEGIN
    return vgGetString(name);
KZS_OPENVG_CALL_END_RETURN_VALUE(KZ_NULL)
}

/*lint --fie*/
#endif

