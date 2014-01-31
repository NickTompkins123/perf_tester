/**
* \file
* OpenGL ES 1.1 wrappers.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_OPENGLES11_H
#define KZS_OPENGLES11_H


#include "kzs_opengl_base.h"

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>

#include <GLES/gl.h>
#include <GLES/glext.h>

/** If this is enabled, then all OpenGL calls will be checked for errors. */
#ifdef _DEBUG
#define DEBUG_OPENGL 0
#else
#define DEBUG_OPENGL 0
#endif

#if DEBUG_OPENGL
#include <system/debug/kzs_log.h>


static void kzsGlErrorNotify(GLenum error)
{
    if(error == GL_INVALID_VALUE)
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "GL_INVALID_VALUE");
    }
    else if(error == GL_INVALID_ENUM)
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "GL_INVALID_ENUM");
    }
    else if(error == GL_INVALID_OPERATION)
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "GL_INVALID_OPERATION");
    }
    else if(error == GL_STACK_OVERFLOW)
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "GL_STACK_OVERFLOW");
    }
    else if(error == GL_STACK_UNDERFLOW)
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "GL_STACK_UNDERFLOW");
    }
    else if(error == GL_OUT_OF_MEMORY)
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "GL_OUT_OF_MEMORY");
    }
    else
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "Unknown OpenGL error");
    }
}

static void kzsGlErrorTest(void)
{
    GLenum error;
    do {
        error = glGetError();
        if (error != GL_NO_ERROR)
        {
            kzsGlErrorNotify(error);
            kzsDebugBreak();
        }
    } while (error != GL_NO_ERROR);
}
#else
#define kzsGlErrorTest()
#endif


KZ_INLINE GLboolean kzsGetGLBoolean_private(kzBool value)
{
    return value ? GL_TRUE : GL_FALSE;
}

/* OpenGL ES core versions */
#define KZS_GL_VERSION_ES_CM_1_0        GL_VERSION_ES_CM_1_0
#define KZS_GL_VERSION_ES_CM_1_1        GL_VERSION_ES_CM_1_1

/* ClearBufferMask */
#define KZS_GL_DEPTH_BUFFER_BIT               GL_DEPTH_BUFFER_BIT
#define KZS_GL_STENCIL_BUFFER_BIT             GL_STENCIL_BUFFER_BIT
#define KZS_GL_COLOR_BUFFER_BIT               GL_COLOR_BUFFER_BIT

/* Boolean */
#define KZS_GL_FALSE                          GL_FALSE
#define KZS_GL_TRUE                           GL_TRUE

/* BeginMode */
#define KZS_GL_POINTS                         GL_POINTS
#define KZS_GL_LINES                          GL_LINES
#define KZS_GL_LINE_LOOP                      GL_LINE_LOOP
#define KZS_GL_LINE_STRIP                     GL_LINE_STRIP
#define KZS_GL_TRIANGLES                      GL_TRIANGLES
#define KZS_GL_TRIANGLE_STRIP                 GL_TRIANGLE_STRIP
#define KZS_GL_TRIANGLE_FAN                   GL_TRIANGLE_FAN

/* AlphaFunction */
#define KZS_GL_NEVER                          GL_NEVER
#define KZS_GL_LESS                           GL_LESS
#define KZS_GL_EQUAL                          GL_EQUAL
#define KZS_GL_LEQUAL                         GL_LEQUAL
#define KZS_GL_GREATER                        GL_GREATER
#define KZS_GL_NOTEQUAL                       GL_NOTEQUAL
#define KZS_GL_GEQUAL                         GL_GEQUAL
#define KZS_GL_ALWAYS                         GL_ALWAYS

/* BlendingFactorDest */
#define KZS_GL_ZERO                           GL_ZERO
#define KZS_GL_ONE                            GL_ONE
#define KZS_GL_SRC_COLOR                      GL_SRC_COLOR
#define KZS_GL_ONE_MINUS_SRC_COLOR            GL_ONE_MINUS_SRC_COLOR
#define KZS_GL_SRC_ALPHA                      GL_SRC_ALPHA
#define KZS_GL_ONE_MINUS_SRC_ALPHA            GL_ONE_MINUS_SRC_ALPHA
#define KZS_GL_DST_ALPHA                      GL_DST_ALPHA
#define KZS_GL_ONE_MINUS_DST_ALPHA            GL_ONE_MINUS_DST_ALPHA

#define KZS_GL_DST_COLOR                      GL_DST_COLOR
#define KZS_GL_ONE_MINUS_DST_COLOR            GL_ONE_MINUS_DST_COLOR
#define KZS_GL_SRC_ALPHA_SATURATE             GL_SRC_ALPHA_SATURATE

/* ClipPlaneName */
#define KZS_GL_CLIP_PLANE0                    GL_CLIP_PLANE0
#define KZS_GL_CLIP_PLANE1                    GL_CLIP_PLANE1
#define KZS_GL_CLIP_PLANE2                    GL_CLIP_PLANE2
#define KZS_GL_CLIP_PLANE3                    GL_CLIP_PLANE3
#define KZS_GL_CLIP_PLANE4                    GL_CLIP_PLANE4
#define KZS_GL_CLIP_PLANE5                    GL_CLIP_PLANE5

/* CullFaceMode */
#define KZS_GL_FRONT                          GL_FRONT
#define KZS_GL_BACK                           GL_BACK
#define KZS_GL_FRONT_AND_BACK                 GL_FRONT_AND_BACK

/* EnableCap */
#define KZS_GL_FOG                            GL_FOG
#define KZS_GL_LIGHTING                       GL_LIGHTING
#define KZS_GL_TEXTURE_2D                     GL_TEXTURE_2D
#define KZS_GL_CULL_FACE                      GL_CULL_FACE
#define KZS_GL_ALPHA_TEST                     GL_ALPHA_TEST
#define KZS_GL_BLEND                          GL_BLEND
#define KZS_GL_COLOR_LOGIC_OP                 GL_COLOR_LOGIC_OP
#define KZS_GL_DITHER                         GL_DITHER
#define KZS_GL_STENCIL_TEST                   GL_STENCIL_TEST
#define KZS_GL_DEPTH_TEST                     GL_DEPTH_TEST
#define KZS_GL_POINT_SMOOTH                   GL_POINT_SMOOTH
#define KZS_GL_LINE_SMOOTH                    GL_LINE_SMOOTH
#define KZS_GL_SCISSOR_TEST                   GL_SCISSOR_TEST
#define KZS_GL_COLOR_MATERIAL                 GL_COLOR_MATERIAL
#define KZS_GL_NORMALIZE                      GL_NORMALIZE
#define KZS_GL_RESCALE_NORMAL                 GL_RESCALE_NORMAL
#define KZS_GL_POLYGON_OFFSET_FILL            GL_POLYGON_OFFSET_FILL
#define KZS_GL_VERTEX_ARRAY                   GL_VERTEX_ARRAY
#define KZS_GL_NORMAL_ARRAY                   GL_NORMAL_ARRAY
#define KZS_GL_COLOR_ARRAY                    GL_COLOR_ARRAY
#define KZS_GL_TEXTURE_COORD_ARRAY            GL_TEXTURE_COORD_ARRAY
#define KZS_GL_MULTISAMPLE                    GL_MULTISAMPLE
#define KZS_GL_SAMPLE_ALPHA_TO_COVERAGE       GL_SAMPLE_ALPHA_TO_COVERAGE
#define KZS_GL_SAMPLE_ALPHA_TO_ONE            GL_SAMPLE_ALPHA_TO_ONE
#define KZS_GL_SAMPLE_COVERAGE                GL_SAMPLE_COVERAGE

/* ErrorCode */
#define KZS_GL_NO_ERROR                       GL_NO_ERROR
#define KZS_GL_INVALID_ENUM                   GL_INVALID_ENUM
#define KZS_GL_INVALID_VALUE                  GL_INVALID_VALUE
#define KZS_GL_INVALID_OPERATION              GL_INVALID_OPERATION
#define KZS_GL_STACK_OVERFLOW                 GL_STACK_OVERFLOW
#define KZS_GL_STACK_UNDERFLOW                GL_STACK_UNDERFLOW
#define KZS_GL_OUT_OF_MEMORY                  GL_OUT_OF_MEMORY

/* FogMode */
/*      KZS_GL_LINEAR */
#define KZS_GL_EXP                            GL_EXP
#define KZS_GL_EXP2                           GL_EXP2

/* FogParameter */
#define KZS_GL_FOG_DENSITY                    GL_FOG_DENSITY
#define KZS_GL_FOG_START                      GL_FOG_START
#define KZS_GL_FOG_END                        GL_FOG_END
#define KZS_GL_FOG_MODE                       GL_FOG_MODE
#define KZS_GL_FOG_COLOR                      GL_FOG_COLOR

/* FrontFaceDirection */
#define KZS_GL_CW                             GL_CW
#define KZS_GL_CCW                            GL_CCW

/* GetPName */
#define KZS_GL_CURRENT_COLOR                  GL_CURRENT_COLOR
#define KZS_GL_CURRENT_NORMAL                 GL_CURRENT_NORMAL
#define KZS_GL_CURRENT_TEXTURE_COORDS         GL_CURRENT_TEXTURE_COORDS
#define KZS_GL_POINT_SIZE                     GL_POINT_SIZE
#define KZS_GL_POINT_SIZE_MIN                 GL_POINT_SIZE_MIN
#define KZS_GL_POINT_SIZE_MAX                 GL_POINT_SIZE_MAX
#define KZS_GL_POINT_FADE_THRESHOLD_SIZE      GL_POINT_FADE_THRESHOLD_SIZE
#define KZS_GL_POINT_DISTANCE_ATTENUATION     GL_POINT_DISTANCE_ATTENUATION
#define KZS_GL_SMOOTH_POINT_SIZE_RANGE        GL_SMOOTH_POINT_SIZE_RANGE
#define KZS_GL_LINE_WIDTH                     GL_LINE_WIDTH
#define KZS_GL_SMOOTH_LINE_WIDTH_RANGE        GL_SMOOTH_LINE_WIDTH_RANGE
#define KZS_GL_ALIASED_POINT_SIZE_RANGE       GL_ALIASED_POINT_SIZE_RANGE
#define KZS_GL_ALIASED_LINE_WIDTH_RANGE       GL_ALIASED_LINE_WIDTH_RANGE
#define KZS_GL_CULL_FACE_MODE                 GL_CULL_FACE_MODE
#define KZS_GL_FRONT_FACE                     GL_FRONT_FACE
#define KZS_GL_SHADE_MODEL                    GL_SHADE_MODEL
#define KZS_GL_DEPTH_RANGE                    GL_DEPTH_RANGE
#define KZS_GL_DEPTH_WRITEMASK                GL_DEPTH_WRITEMASK
#define KZS_GL_DEPTH_CLEAR_VALUE              GL_DEPTH_CLEAR_VALUE
#define KZS_GL_DEPTH_FUNC                     GL_DEPTH_FUNC
#define KZS_GL_STENCIL_CLEAR_VALUE            GL_STENCIL_CLEAR_VALUE
#define KZS_GL_STENCIL_FUNC                   GL_STENCIL_FUNC
#define KZS_GL_STENCIL_VALUE_MASK             GL_STENCIL_VALUE_MASK
#define KZS_GL_STENCIL_FAIL                   GL_STENCIL_FAIL
#define KZS_GL_STENCIL_PASS_DEPTH_FAIL        GL_STENCIL_PASS_DEPTH_FAIL
#define KZS_GL_STENCIL_PASS_DEPTH_PASS        GL_STENCIL_PASS_DEPTH_PASS
#define KZS_GL_STENCIL_REF                    GL_STENCIL_REF
#define KZS_GL_STENCIL_WRITEMASK              GL_STENCIL_WRITEMASK
#define KZS_GL_MATRIX_MODE                    GL_MATRIX_MODE
#define KZS_GL_VIEWPORT                       GL_VIEWPORT
#define KZS_GL_MODELVIEW_STACK_DEPTH          GL_MODELVIEW_STACK_DEPTH
#define KZS_GL_PROJECTION_STACK_DEPTH         GL_PROJECTION_STACK_DEPTH
#define KZS_GL_TEXTURE_STACK_DEPTH            GL_TEXTURE_STACK_DEPTH
#define KZS_GL_MODELVIEW_MATRIX               GL_MODELVIEW_MATRIX
#define KZS_GL_PROJECTION_MATRIX              GL_PROJECTION_MATRIX
#define KZS_GL_TEXTURE_MATRIX                 GL_TEXTURE_MATRIX
#define KZS_GL_ALPHA_TEST_FUNC                GL_ALPHA_TEST_FUNC
#define KZS_GL_ALPHA_TEST_REF                 GL_ALPHA_TEST_REF
#define KZS_GL_BLEND_DST                      GL_BLEND_DST
#define KZS_GL_BLEND_SRC                      GL_BLEND_SRC
#define KZS_GL_LOGIC_OP_MODE                  GL_LOGIC_OP_MODE
#define KZS_GL_SCISSOR_BOX                    GL_SCISSOR_BOX
#define KZS_GL_COLOR_CLEAR_VALUE              GL_COLOR_CLEAR_VALUE
#define KZS_GL_COLOR_WRITEMASK                GL_COLOR_WRITEMASK
#define KZS_GL_UNPACK_ALIGNMENT               GL_UNPACK_ALIGNMENT
#define KZS_GL_PACK_ALIGNMENT                 GL_PACK_ALIGNMENT
#define KZS_GL_MAX_LIGHTS                     GL_MAX_LIGHTS
#define KZS_GL_MAX_CLIP_PLANES                GL_MAX_CLIP_PLANES
#define KZS_GL_MAX_TEXTURE_SIZE               GL_MAX_TEXTURE_SIZE
#define KZS_GL_MAX_MODELVIEW_STACK_DEPTH      GL_MAX_MODELVIEW_STACK_DEPTH
#define KZS_GL_MAX_PROJECTION_STACK_DEPTH     GL_MAX_PROJECTION_STACK_DEPTH
#define KZS_GL_MAX_TEXTURE_STACK_DEPTH        GL_MAX_TEXTURE_STACK_DEPTH
#define KZS_GL_MAX_VIEWPORT_DIMS              GL_MAX_VIEWPORT_DIMS
#define KZS_GL_MAX_TEXTURE_UNITS              GL_MAX_TEXTURE_UNITS
#define KZS_GL_SUBPIXEL_BITS                  GL_SUBPIXEL_BITS
#define KZS_GL_RED_BITS                       GL_RED_BITS
#define KZS_GL_GREEN_BITS                     GL_GREEN_BITS
#define KZS_GL_BLUE_BITS                      GL_BLUE_BITS
#define KZS_GL_ALPHA_BITS                     GL_ALPHA_BITS
#define KZS_GL_DEPTH_BITS                     GL_DEPTH_BITS
#define KZS_GL_STENCIL_BITS                   GL_STENCIL_BITS
#define KZS_GL_POLYGON_OFFSET_UNITS           GL_POLYGON_OFFSET_UNITS
#define KZS_GL_POLYGON_OFFSET_FACTOR          GL_POLYGON_OFFSET_FACTOR
#define KZS_GL_TEXTURE_BINDING_2D             GL_TEXTURE_BINDING_2D
#define KZS_GL_VERTEX_ARRAY_SIZE              GL_VERTEX_ARRAY_SIZE
#define KZS_GL_VERTEX_ARRAY_TYPE              GL_VERTEX_ARRAY_TYPE
#define KZS_GL_VERTEX_ARRAY_STRIDE            GL_VERTEX_ARRAY_STRIDE
#define KZS_GL_NORMAL_ARRAY_TYPE              GL_NORMAL_ARRAY_TYPE
#define KZS_GL_NORMAL_ARRAY_STRIDE            GL_NORMAL_ARRAY_STRIDE
#define KZS_GL_COLOR_ARRAY_SIZE               GL_COLOR_ARRAY_SIZE
#define KZS_GL_COLOR_ARRAY_TYPE               GL_COLOR_ARRAY_TYPE
#define KZS_GL_COLOR_ARRAY_STRIDE             GL_COLOR_ARRAY_STRIDE
#define KZS_GL_TEXTURE_COORD_ARRAY_SIZE       GL_TEXTURE_COORD_ARRAY_SIZE
#define KZS_GL_TEXTURE_COORD_ARRAY_TYPE       GL_TEXTURE_COORD_ARRAY_TYPE
#define KZS_GL_TEXTURE_COORD_ARRAY_STRIDE     GL_TEXTURE_COORD_ARRAY_STRIDE
#define KZS_GL_VERTEX_ARRAY_POINTER           GL_VERTEX_ARRAY_POINTER
#define KZS_GL_NORMAL_ARRAY_POINTER           GL_NORMAL_ARRAY_POINTER
#define KZS_GL_COLOR_ARRAY_POINTER            GL_COLOR_ARRAY_POINTER
#define KZS_GL_TEXTURE_COORD_ARRAY_POINTER    GL_TEXTURE_COORD_ARRAY_POINTER
#define KZS_GL_SAMPLE_BUFFERS                 GL_SAMPLE_BUFFERS
#define KZS_GL_SAMPLES                        GL_SAMPLES
#define KZS_GL_SAMPLE_COVERAGE_VALUE          GL_SAMPLE_COVERAGE_VALUE
#define KZS_GL_SAMPLE_COVERAGE_INVERT         GL_SAMPLE_COVERAGE_INVERT

#define KZS_GL_NUM_COMPRESSED_TEXTURE_FORMATS GL_NUM_COMPRESSED_TEXTURE_FORMATS
#define KZS_GL_COMPRESSED_TEXTURE_FORMATS     GL_COMPRESSED_TEXTURE_FORMATS

/* HintMode */
#define KZS_GL_DONT_CARE                      GL_DONT_CARE
#define KZS_GL_FASTEST                        GL_FASTEST
#define KZS_GL_NICEST                         GL_NICEST

/* HintTarget */
#define KZS_GL_PERSPECTIVE_CORRECTION_HINT    GL_PERSPECTIVE_CORRECTION_HINT
#define KZS_GL_POINT_SMOOTH_HINT              GL_POINT_SMOOTH_HINT
#define KZS_GL_LINE_SMOOTH_HINT               GL_LINE_SMOOTH_HINT
#define KZS_GL_FOG_HINT                       GL_FOG_HINT
#define KZS_GL_GENERATE_MIPMAP_HINT           GL_GENERATE_MIPMAP_HINT

/* LightModelParameter */
#define KZS_GL_LIGHT_MODEL_AMBIENT            GL_LIGHT_MODEL_AMBIENT 
#define KZS_GL_LIGHT_MODEL_TWO_SIDE           GL_LIGHT_MODEL_TWO_SIDE

/* LightParameter */
#define KZS_GL_AMBIENT                        GL_AMBIENT
#define KZS_GL_DIFFUSE                        GL_DIFFUSE
#define KZS_GL_SPECULAR                       GL_SPECULAR
#define KZS_GL_POSITION                       GL_POSITION
#define KZS_GL_SPOT_DIRECTION                 GL_SPOT_DIRECTION
#define KZS_GL_SPOT_EXPONENT                  GL_SPOT_EXPONENT
#define KZS_GL_SPOT_CUTOFF                    GL_SPOT_CUTOFF
#define KZS_GL_CONSTANT_ATTENUATION           GL_CONSTANT_ATTENUATION
#define KZS_GL_LINEAR_ATTENUATION             GL_LINEAR_ATTENUATION
#define KZS_GL_QUADRATIC_ATTENUATION          GL_QUADRATIC_ATTENUATION

/* DataType */
#define KZS_GL_BYTE                           GL_BYTE
#define KZS_GL_UNSIGNED_BYTE                  GL_UNSIGNED_BYTE
#define KZS_GL_SHORT                          GL_SHORT
#define KZS_GL_UNSIGNED_SHORT                 GL_UNSIGNED_SHORT
#define KZS_GL_FLOAT                          GL_FLOAT
#define KZS_GL_FIXED                          GL_FIXED

/* LogicOp */
#define KZS_GL_CLEAR                          GL_CLEAR
#define KZS_GL_AND                            GL_AND
#define KZS_GL_AND_REVERSE                    GL_AND_REVERSE
#define KZS_GL_COPY                           GL_COPY
#define KZS_GL_AND_INVERTED                   GL_AND_INVERTED 
#define KZS_GL_NOOP                           GL_NOOP
#define KZS_GL_XOR                            GL_XOR
#define KZS_GL_OR                             GL_OR
#define KZS_GL_NOR                            GL_NOR
#define KZS_GL_EQUIV                          GL_EQUIV
#define KZS_GL_INVERT                         GL_INVERT
#define KZS_GL_OR_REVERSE                     GL_OR_REVERSE
#define KZS_GL_COPY_INVERTED                  GL_COPY_INVERTED
#define KZS_GL_OR_INVERTED                    GL_OR_INVERTED
#define KZS_GL_NAND                           GL_NAND
#define KZS_GL_SET                            GL_SET

/* MaterialParameter */
#define KZS_GL_EMISSION                       GL_EMISSION
#define KZS_GL_SHININESS                      GL_SHININESS
#define KZS_GL_AMBIENT_AND_DIFFUSE            GL_AMBIENT_AND_DIFFUSE

/* MatrixMode */
#define KZS_GL_MODELVIEW                      GL_MODELVIEW
#define KZS_GL_PROJECTION                     GL_PROJECTION
#define KZS_GL_TEXTURE                        GL_TEXTURE

/* PixelFormat */
#define KZS_GL_ALPHA                          GL_ALPHA
#define KZS_GL_RGB                            GL_RGB
#define KZS_GL_RGBA                           GL_RGBA
#define KZS_GL_LUMINANCE                      GL_LUMINANCE
#define KZS_GL_LUMINANCE_ALPHA                GL_LUMINANCE_ALPHA

/* PixelType */
#define KZS_GL_UNSIGNED_SHORT_4_4_4_4         GL_UNSIGNED_SHORT_4_4_4_4
#define KZS_GL_UNSIGNED_SHORT_5_5_5_1         GL_UNSIGNED_SHORT_5_5_5_1
#define KZS_GL_UNSIGNED_SHORT_5_6_5           GL_UNSIGNED_SHORT_5_6_5

/* ShadingModel */
#define KZS_GL_FLAT                           GL_FLAT
#define KZS_GL_SMOOTH                         GL_SMOOTH

/* StencilOp */
#define KZS_GL_KEEP                           GL_KEEP
#define KZS_GL_REPLACE                        GL_REPLACE
#define KZS_GL_INCR                           GL_INCR
#define KZS_GL_DECR                           GL_DECR

/* StringName */
#define KZS_GL_VENDOR                         GL_VENDOR
#define KZS_GL_RENDERER                       GL_RENDERER
#define KZS_GL_VERSION                        GL_VERSION
#define KZS_GL_EXTENSIONS                     GL_EXTENSIONS

/* TextureEnvMode */
#define KZS_GL_MODULATE                       GL_MODULATE
#define KZS_GL_DECAL                          GL_DECAL
/*      KZS_GL_BLEND */
#define KZS_GL_ADD                            GL_ADD
/*      KZS_GL_REPLACE */

/* TextureEnvParameter */
#define KZS_GL_TEXTURE_ENV_MODE               GL_TEXTURE_ENV_MODE
#define KZS_GL_TEXTURE_ENV_COLOR              GL_TEXTURE_ENV_COLOR

/* TextureEnvTarget */
#define KZS_GL_TEXTURE_ENV                    GL_TEXTURE_ENV

/* TextureMagFilter */
#define KZS_GL_NEAREST                        GL_NEAREST
#define KZS_GL_LINEAR                         GL_LINEAR

/* TextureMinFilter */
#define KZS_GL_NEAREST_MIPMAP_NEAREST         GL_NEAREST_MIPMAP_NEAREST
#define KZS_GL_LINEAR_MIPMAP_NEAREST          GL_LINEAR_MIPMAP_NEAREST
#define KZS_GL_NEAREST_MIPMAP_LINEAR          GL_NEAREST_MIPMAP_LINEAR
#define KZS_GL_LINEAR_MIPMAP_LINEAR           GL_LINEAR_MIPMAP_LINEAR

/* TextureParameterName */
#define KZS_GL_TEXTURE_MAG_FILTER             GL_TEXTURE_MAG_FILTER
#define KZS_GL_TEXTURE_MIN_FILTER             GL_TEXTURE_MIN_FILTER
#define KZS_GL_TEXTURE_WRAP_S                 GL_TEXTURE_WRAP_S
#define KZS_GL_TEXTURE_WRAP_T                 GL_TEXTURE_WRAP_T
#define KZS_GL_GENERATE_MIPMAP                GL_GENERATE_MIPMAP

/* TextureUnit */
#define KZS_GL_TEXTURE0                       GL_TEXTURE0
#define KZS_GL_TEXTURE1                       GL_TEXTURE1
#define KZS_GL_TEXTURE2                       GL_TEXTURE2
#define KZS_GL_TEXTURE3                       GL_TEXTURE3
#define KZS_GL_TEXTURE4                       GL_TEXTURE4
#define KZS_GL_TEXTURE5                       GL_TEXTURE5
#define KZS_GL_TEXTURE6                       GL_TEXTURE6
#define KZS_GL_TEXTURE7                       GL_TEXTURE7
#define KZS_GL_TEXTURE8                       GL_TEXTURE8
#define KZS_GL_TEXTURE9                       GL_TEXTURE9
#define KZS_GL_TEXTURE10                      GL_TEXTURE10
#define KZS_GL_TEXTURE11                      GL_TEXTURE11
#define KZS_GL_TEXTURE12                      GL_TEXTURE12
#define KZS_GL_TEXTURE13                      GL_TEXTURE13
#define KZS_GL_TEXTURE14                      GL_TEXTURE14
#define KZS_GL_TEXTURE15                      GL_TEXTURE15
#define KZS_GL_TEXTURE16                      GL_TEXTURE16
#define KZS_GL_TEXTURE17                      GL_TEXTURE17
#define KZS_GL_TEXTURE18                      GL_TEXTURE18
#define KZS_GL_TEXTURE19                      GL_TEXTURE19
#define KZS_GL_TEXTURE20                      GL_TEXTURE20
#define KZS_GL_TEXTURE21                      GL_TEXTURE21
#define KZS_GL_TEXTURE22                      GL_TEXTURE22
#define KZS_GL_TEXTURE23                      GL_TEXTURE23
#define KZS_GL_TEXTURE24                      GL_TEXTURE24
#define KZS_GL_TEXTURE25                      GL_TEXTURE25
#define KZS_GL_TEXTURE26                      GL_TEXTURE26
#define KZS_GL_TEXTURE27                      GL_TEXTURE27
#define KZS_GL_TEXTURE28                      GL_TEXTURE28
#define KZS_GL_TEXTURE29                      GL_TEXTURE29
#define KZS_GL_TEXTURE30                      GL_TEXTURE30
#define KZS_GL_TEXTURE31                      GL_TEXTURE31
#define KZS_GL_ACTIVE_TEXTURE                 GL_ACTIVE_TEXTURE
#define KZS_GL_CLIENT_ACTIVE_TEXTURE          GL_CLIENT_ACTIVE_TEXTURE

/* TextureWrapMode */
#define KZS_GL_REPEAT                         GL_REPEAT
#define KZS_GL_CLAMP_TO_EDGE                  GL_CLAMP_TO_EDGE

/* LightName */
#define KZS_GL_LIGHT0                         GL_LIGHT0
#define KZS_GL_LIGHT1                         GL_LIGHT1
#define KZS_GL_LIGHT2                         GL_LIGHT2
#define KZS_GL_LIGHT3                         GL_LIGHT3
#define KZS_GL_LIGHT4                         GL_LIGHT4
#define KZS_GL_LIGHT5                         GL_LIGHT5
#define KZS_GL_LIGHT6                         GL_LIGHT6
#define KZS_GL_LIGHT7                         GL_LIGHT7

/* Buffer Objects */
#define KZS_GL_ARRAY_BUFFER                   GL_ARRAY_BUFFER
#define KZS_GL_ELEMENT_ARRAY_BUFFER           GL_ELEMENT_ARRAY_BUFFER

#define KZS_GL_ARRAY_BUFFER_BINDING               GL_ARRAY_BUFFER_BINDING
#define KZS_GL_ELEMENT_ARRAY_BUFFER_BINDING       GL_ELEMENT_ARRAY_BUFFER_BINDING
#define KZS_GL_VERTEX_ARRAY_BUFFER_BINDING        GL_VERTEX_ARRAY_BUFFER_BINDING
#define KZS_GL_NORMAL_ARRAY_BUFFER_BINDING        GL_NORMAL_ARRAY_BUFFER_BINDING
#define KZS_GL_COLOR_ARRAY_BUFFER_BINDING         GL_COLOR_ARRAY_BUFFER_BINDING
#define KZS_GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING

#define KZS_GL_STATIC_DRAW                    GL_STATIC_DRAW
#define KZS_GL_DYNAMIC_DRAW                   GL_DYNAMIC_DRAW

#define KZS_GL_BUFFER_SIZE                    GL_BUFFER_SIZE
#define KZS_GL_BUFFER_USAGE                   GL_BUFFER_USAGE

/* Texture combine + dot3 */
#define KZS_GL_SUBTRACT                       GL_SUBTRACT
#define KZS_GL_COMBINE                        GL_COMBINE
#define KZS_GL_COMBINE_RGB                    GL_COMBINE_RGB
#define KZS_GL_COMBINE_ALPHA                  GL_COMBINE_ALPHA
#define KZS_GL_RGB_SCALE                      GL_RGB_SCALE
#define KZS_GL_ADD_SIGNED                     GL_ADD_SIGNED
#define KZS_GL_INTERPOLATE                    GL_INTERPOLATE
#define KZS_GL_CONSTANT                       GL_CONSTANT
#define KZS_GL_PRIMARY_COLOR                  GL_PRIMARY_COLOR
#define KZS_GL_PREVIOUS                       GL_PREVIOUS
#define KZS_GL_OPERAND0_RGB                   GL_OPERAND0_RGB
#define KZS_GL_OPERAND1_RGB                   GL_OPERAND1_RGB
#define KZS_GL_OPERAND2_RGB                   GL_OPERAND2_RGB
#define KZS_GL_OPERAND0_ALPHA                 GL_OPERAND0_ALPHA
#define KZS_GL_OPERAND1_ALPHA                 GL_OPERAND1_ALPHA
#define KZS_GL_OPERAND2_ALPHA                 GL_OPERAND2_ALPHA

#define KZS_GL_ALPHA_SCALE                    GL_ALPHA_SCALE

#define KZS_GL_SRC0_RGB                       GL_SRC0_RGB
#define KZS_GL_SRC1_RGB                       GL_SRC1_RGB
#define KZS_GL_SRC2_RGB                       GL_SRC2_RGB
#define KZS_GL_SRC0_ALPHA                     GL_SRC0_ALPHA
#define KZS_GL_SRC1_ALPHA                     GL_SRC1_ALPHA
#define KZS_GL_SRC2_ALPHA                     GL_SRC2_ALPHA

#define KZS_GL_DOT3_RGB                       GL_DOT3_RGB
#define KZS_GL_DOT3_RGBA                      GL_DOT3_RGBA

/*------------------------------------------------------------------------*
* required OES extension tokens
* _OES should be removed for compatibility with ES2 features
*------------------------------------------------------------------------*/

/* GL_OES_compressed_ETC1_RGB8_texture */
#define KZS_GL_ETC1_RGB8_OES                                        GL_ETC1_RGB8_OES


/* OES_read_format */
#ifndef KZS_GL_OES_read_format
#define KZS_GL_IMPLEMENTATION_COLOR_READ_TYPE_OES                   GL_IMPLEMENTATION_COLOR_READ_TYPE_OES
#define KZS_GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES                 GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES
#endif

/* KZS_GL_OES_compressed_paletted_texture */
#ifndef KZS_GL_OES_compressed_paletted_texture
#define KZS_GL_PALETTE4_RGB8_OES                                    GL_PALETTE4_RGB8_OES
#define KZS_GL_PALETTE4_RGBA8_OES                                   GL_PALETTE4_RGBA8_OES
#define KZS_GL_PALETTE4_R5_G6_B5_OES                                GL_PALETTE4_R5_G6_B5_OES
#define KZS_GL_PALETTE4_RGBA4_OES                                   GL_PALETTE4_RGBA4_OES
#define KZS_GL_PALETTE4_RGB5_A1_OES                                 GL_PALETTE4_RGB5_A1_OES
#define KZS_GL_PALETTE8_RGB8_OES                                    GL_PALETTE8_RGB8_OES
#define KZS_GL_PALETTE8_RGBA8_OES                                   GL_PALETTE8_RGBA8_OES
#define KZS_GL_PALETTE8_R5_G6_B5_OES                                GL_PALETTE8_R5_G6_B5_OES
#define KZS_GL_PALETTE8_RGBA4_OES                                   GL_PALETTE8_RGBA4_OES
#define KZS_GL_PALETTE8_RGB5_A1_OES                                 GL_PALETTE8_RGB5_A1_OES
#endif

/* OES_point_size_array */
#ifndef KZS_GL_OES_point_size_array
#define KZS_GL_POINT_SIZE_ARRAY_OES                                 GL_POINT_SIZE_ARRAY_OES
#define KZS_GL_POINT_SIZE_ARRAY_TYPE_OES                            GL_POINT_SIZE_ARRAY_TYPE_OES
#define KZS_GL_POINT_SIZE_ARRAY_STRIDE_OES                          GL_POINT_SIZE_ARRAY_STRIDE_OES
#define KZS_GL_POINT_SIZE_ARRAY_POINTER_OES                         GL_POINT_SIZE_ARRAY_POINTER_OES
#define KZS_GL_POINT_SIZE_ARRAY_BUFFER_BINDING_OES                  GL_POINT_SIZE_ARRAY_BUFFER_BINDING_OES
#endif

/* KZS_GL_OES_point_sprite */
#ifndef KZS_GL_OES_point_sprite
#define KZS_GL_POINT_SPRITE_OES                                     GL_POINT_SPRITE_OES
#define KZS_GL_COORD_REPLACE_OES                                    GL_COORD_REPLACE_OES
#endif

/* OES_framebuffer_object */
#ifndef KZS_GL_OES_framebuffer_object
#define KZS_GL_NONE                                                 GL_NONE_OES
#define KZS_GL_FRAMEBUFFER                                          GL_FRAMEBUFFER_OES
#define KZS_GL_RENDERBUFFER                                         GL_RENDERBUFFER_OES
#define KZS_GL_RGBA4                                                GL_RGBA4_OES
#define KZS_GL_RGB5_A1                                              GL_RGB5_A1_OES
#define KZS_GL_RGB565                                                GL_RGB565_OES
#define KZS_GL_DEPTH_COMPONENT16                                    GL_DEPTH_COMPONENT16_OES
#define KZS_GL_RENDERBUFFER_WIDTH                                    GL_RENDERBUFFER_WIDTH_OES
#define KZS_GL_RENDERBUFFER_HEIGHT                                    GL_RENDERBUFFER_HEIGHT_OES
#define KZS_GL_RENDERBUFFER_INTERNAL_FORMAT                            GL_RENDERBUFFER_INTERNAL_FORMAT_OES
#define KZS_GL_RENDERBUFFER_RED_SIZE                                GL_RENDERBUFFER_RED_SIZE_OES
#define KZS_GL_RENDERBUFFER_GREEN_SIZE                                GL_RENDERBUFFER_GREEN_SIZE_OES
#define KZS_GL_RENDERBUFFER_BLUE_SIZE                                GL_RENDERBUFFER_BLUE_SIZE_OES
#define KZS_GL_RENDERBUFFER_ALPHA_SIZE                                GL_RENDERBUFFER_ALPHA_SIZE_OES
#define KZS_GL_RENDERBUFFER_DEPTH_SIZE                                GL_RENDERBUFFER_DEPTH_SIZE_OES
#define KZS_GL_RENDERBUFFER_STENCIL_SIZE                            GL_RENDERBUFFER_STENCIL_SIZE_OES
#define KZS_GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE                    GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_OES
#define KZS_GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME                    GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_OES
#define KZS_GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL                 GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_OES
#define KZS_GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE         GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_OES
#define KZS_GL_COLOR_ATTACHMENT0                                    GL_COLOR_ATTACHMENT0_OES
#define KZS_GL_DEPTH_ATTACHMENT                                     GL_DEPTH_ATTACHMENT_OES
#define KZS_GL_STENCIL_ATTACHMENT                                   GL_STENCIL_ATTACHMENT_OES
#define KZS_GL_FRAMEBUFFER_COMPLETE                                 GL_FRAMEBUFFER_COMPLETE_OES
#define KZS_GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT                    GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_OES
#define KZS_GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT            GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_OES
#define KZS_GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS                    GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_OES
#define KZS_GL_FRAMEBUFFER_INCOMPLETE_FORMATS                       GL_FRAMEBUFFER_INCOMPLETE_FORMATS_OES
#define KZS_GL_FRAMEBUFFER_INCOMPLETE_UNSUPPORTED                   GL_FRAMEBUFFER_UNSUPPORTED_OES
#define KZS_GL_FRAMEBUFFER_BINDING                                  GL_FRAMEBUFFER_BINDING_OES
#define KZS_GL_RENDERBUFFER_BINDING                                 GL_RENDERBUFFER_BINDING_OES
#define KZS_GL_MAX_RENDERBUFFER_SIZE                                GL_MAX_RENDERBUFFER_SIZE_OES
#define KZS_GL_INVALID_FRAMEBUFFER_OPERATION                        GL_INVALID_FRAMEBUFFER_OPERATION_OES
#endif

#ifndef GL_GLEXT_PROTOTYPES
/* Framebuffer functions */
PFNGLISRENDERBUFFEROESPROC glIsRenderbufferOES;
PFNGLBINDRENDERBUFFEROESPROC glBindRenderbufferOES;
PFNGLDELETERENDERBUFFERSOESPROC glDeleteRenderbuffersOES;
PFNGLGENRENDERBUFFERSOESPROC glGenRenderbuffersOES;
PFNGLRENDERBUFFERSTORAGEOESPROC glRenderbufferStorageOES;
PFNGLGETRENDERBUFFERPARAMETERIVOESPROC glGetRenderbufferParameterivOES;
PFNGLISFRAMEBUFFEROESPROC glIsFramebufferOES;
PFNGLBINDFRAMEBUFFEROESPROC glBindFramebufferOES;
PFNGLDELETEFRAMEBUFFERSOESPROC glDeleteFramebuffersOES;
PFNGLGENFRAMEBUFFERSOESPROC glGenFramebuffersOES;
PFNGLCHECKFRAMEBUFFERSTATUSOESPROC glCheckFramebufferStatusOES;
PFNGLFRAMEBUFFERRENDERBUFFEROESPROC glFramebufferRenderbufferOES;
PFNGLFRAMEBUFFERTEXTURE2DOESPROC glFramebufferTexture2DOES;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVOESPROC glGetFramebufferAttachmentParameterivOES;
PFNGLGENERATEMIPMAPOESPROC glGenerateMipmapOES;
#endif

/* KZS_GL_OES_texture_cube_map */
#ifndef KZS_GL_OES_texture_cube_map
#define KZS_GL_NORMAL_MAP                                           GL_NORMAL_MAP_OES
#define KZS_GL_REFLECTION_MAP                                       GL_REFLECTION_MAP_OES
#define KZS_GL_TEXTURE_CUBE_MAP                                     GL_TEXTURE_CUBE_MAP_OES
#define KZS_GL_TEXTURE_BINDING_CUBE_MAP                             GL_TEXTURE_BINDING_CUBE_MAP_OES
#define KZS_GL_TEXTURE_CUBE_MAP_POSITIVE_X                          GL_TEXTURE_CUBE_MAP_POSITIVE_X_OES
#define KZS_GL_TEXTURE_CUBE_MAP_NEGATIVE_X                          GL_TEXTURE_CUBE_MAP_NEGATIVE_X_OES
#define KZS_GL_TEXTURE_CUBE_MAP_POSITIVE_Y                          GL_TEXTURE_CUBE_MAP_POSITIVE_Y_OES
#define KZS_GL_TEXTURE_CUBE_MAP_NEGATIVE_Y                          GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_OES
#define KZS_GL_TEXTURE_CUBE_MAP_POSITIVE_Z                          GL_TEXTURE_CUBE_MAP_POSITIVE_Z_OES
#define KZS_GL_TEXTURE_CUBE_MAP_NEGATIVE_Z                          GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_OES
#define KZS_GL_MAX_CUBE_MAP_TEXTURE_SIZE                            GL_MAX_CUBE_MAP_TEXTURE_SIZE_OES
#define KZS_GL_TEXTURE_GEN_MODE                                     GL_TEXTURE_GEN_MODE_OES
#define KZS_GL_TEXTURE_GEN_STR                                      GL_TEXTURE_GEN_STR_OES
#endif

#ifndef GL_GLEXT_PROTOTYPES
/* Texture cube functions */
PFNGLTEXGENFOESPROC glTexGenfOES;
PFNGLTEXGENFVOESPROC glTexGenfvOES;
PFNGLTEXGENIOESPROC glTexGeniOES;
PFNGLTEXGENIVOESPROC glTexGenivOES;
PFNGLTEXGENXOESPROC glTexGenxOES;
PFNGLTEXGENXVOESPROC glTexGenxvOES;
PFNGLGETTEXGENFVOESPROC glGetTexGenfvOES;
PFNGLGETTEXGENIVOESPROC glGetTexGenivOES;
PFNGLGETTEXGENXVOESPROC glGetTexGenxvOES;
#endif


KZ_INLINE void kzsGlAlphaFunc(kzUint func, kzFloat ref)
{
KZS_OPENGL_CALL_BEGIN
    glAlphaFunc(func, ref);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlClearColor(kzFloat red, kzFloat green, kzFloat blue, kzFloat alpha)
{
KZS_OPENGL_CALL_BEGIN
    glClearColor(red, green, blue, alpha);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlClearDepthf(kzFloat depth)
{
KZS_OPENGL_CALL_BEGIN
    glClearDepthf(depth);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlClipPlanef(kzUint plane, const kzFloat *equation)
{
KZS_OPENGL_CALL_BEGIN
    glClipPlanef(plane, equation);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlColor4f(kzFloat red, kzFloat green, kzFloat blue, kzFloat alpha)
{
KZS_OPENGL_CALL_BEGIN
    glColor4f(red, green, blue, alpha);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlDepthRangef(kzFloat zNear, kzFloat zFar)
{
KZS_OPENGL_CALL_BEGIN
    glDepthRangef(zNear, zFar);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlFogf(kzUint pname, kzFloat param)
{
KZS_OPENGL_CALL_BEGIN
    glFogf(pname, param);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlFogfv(kzUint pname, const kzFloat* params)
{
KZS_OPENGL_CALL_BEGIN
    glFogfv(pname, params);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlFrustumf(kzFloat left, kzFloat right, kzFloat bottom, kzFloat top, kzFloat zNear, kzFloat zFar)
{
KZS_OPENGL_CALL_BEGIN
    glFrustumf(left, right, bottom, top, zNear, zFar);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlGetClipPlanef(kzUint pname, kzFloat eqn[4])
{
KZS_OPENGL_CALL_BEGIN
    glGetClipPlanef(pname, eqn);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlGetFloatv(kzUint pname, kzFloat *params)
{
KZS_OPENGL_CALL_BEGIN
    glGetFloatv(pname, params);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlGetLightfv(kzUint light, kzUint pname, kzFloat *params)
{
KZS_OPENGL_CALL_BEGIN
    glGetLightfv(light, pname, params);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlGetMaterialfv(kzUint face, kzUint pname, kzFloat *params)
{
KZS_OPENGL_CALL_BEGIN
    glGetMaterialfv(face, pname, params);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlGetTexEnvfv(kzUint env, kzUint pname, kzFloat *params)
{
KZS_OPENGL_CALL_BEGIN
    glGetTexEnvfv(env, pname, params);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlGetTexParameterfv(kzUint target, kzUint pname, kzFloat *params)
{
KZS_OPENGL_CALL_BEGIN
    glGetTexParameterfv(target, pname, params);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlLightModelf(kzUint pname, kzFloat param)
{
KZS_OPENGL_CALL_BEGIN
    glLightModelf(pname, param);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlLightModelfv(kzUint pname, const kzFloat *params)
{
KZS_OPENGL_CALL_BEGIN
    glLightModelfv(pname, params);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlLightf(kzUint light, kzUint pname, kzFloat param)
{
KZS_OPENGL_CALL_BEGIN
    glLightf(light, pname, param);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlLightfv(kzUint light, kzUint pname, const kzFloat *params)
{
KZS_OPENGL_CALL_BEGIN
    glLightfv(light, pname, params);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlLineWidth(kzFloat width)
{
KZS_OPENGL_CALL_BEGIN
    glLineWidth(width);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlLoadMatrixf(const kzFloat *m)
{
KZS_OPENGL_CALL_BEGIN
    glLoadMatrixf(m);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlMaterialf(kzUint face, kzUint pname, kzFloat param)
{
KZS_OPENGL_CALL_BEGIN
    glMaterialf(face, pname, param);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlMaterialfv(kzUint face, kzUint pname, const kzFloat *params)
{
KZS_OPENGL_CALL_BEGIN
    glMaterialfv(face, pname, params);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlMultMatrixf(const kzFloat *m)
{
KZS_OPENGL_CALL_BEGIN
    glMultMatrixf(m);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlMultiTexCoord4f(kzUint target, kzFloat s, kzFloat t, kzFloat r, kzFloat q)
{
KZS_OPENGL_CALL_BEGIN
    glMultiTexCoord4f(target, s, t, r, q);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlNormal3f(kzFloat nx, kzFloat ny, kzFloat nz)
{
KZS_OPENGL_CALL_BEGIN
    glNormal3f(nx, ny, nz);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlOrthof(kzFloat left, kzFloat right, kzFloat bottom, kzFloat top, kzFloat zNear, kzFloat zFar)
{
KZS_OPENGL_CALL_BEGIN
    glOrthof(left, right, bottom, top, zNear, zFar);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlPointParameterf(kzUint pname, kzFloat param)
{
KZS_OPENGL_CALL_BEGIN
    glPointParameterf(pname, param);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlPointParameterfv(kzUint pname, const kzFloat *params)
{
KZS_OPENGL_CALL_BEGIN
    glPointParameterfv(pname, params);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlPointSize(kzFloat size)
{
KZS_OPENGL_CALL_BEGIN
    glPointSize(size);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlPolygonOffset(kzFloat factor, kzFloat units)
{
KZS_OPENGL_CALL_BEGIN
    glPolygonOffset(factor, units);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlRotatef(kzFloat angle, kzFloat x, kzFloat y, kzFloat z)
{
KZS_OPENGL_CALL_BEGIN
    glRotatef(angle, x, y, z);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlScalef(kzFloat x, kzFloat y, kzFloat z)
{
KZS_OPENGL_CALL_BEGIN
    glScalef(x, y, z);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlTexEnvf(kzUint target, kzUint pname, kzFloat param)
{
KZS_OPENGL_CALL_BEGIN
    glTexEnvf(target, pname, param);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlTexEnvfv(kzUint target, kzUint pname, const kzFloat *params)
{
KZS_OPENGL_CALL_BEGIN
    glTexEnvfv(target, pname, params);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlTexParameterf(kzUint target, kzUint pname, kzFloat param)
{
KZS_OPENGL_CALL_BEGIN
    glTexParameterf(target, pname, param);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlTexParameterfv(kzUint target, kzUint pname, const kzFloat *params)
{
KZS_OPENGL_CALL_BEGIN
    glTexParameterfv(target, pname, params);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlTranslatef(kzFloat x, kzFloat y, kzFloat z)
{
KZS_OPENGL_CALL_BEGIN
    glTranslatef(x, y, z);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlActiveTexture(kzUint texture)
{
KZS_OPENGL_CALL_BEGIN
    glActiveTexture(texture);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlBindBuffer(kzUint target, kzUint buffer)
{
KZS_OPENGL_CALL_BEGIN
    glBindBuffer(target, buffer);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlBindTexture(kzUint target, kzUint texture)
{
KZS_OPENGL_CALL_BEGIN
    glBindTexture(target, texture);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlBlendFunc(kzUint sfactor, kzUint dfactor)
{
KZS_OPENGL_CALL_BEGIN
    glBlendFunc(sfactor, dfactor);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlBufferData(kzUint target, kzInt size, const void *data, kzUint usage)
{
KZS_OPENGL_CALL_BEGIN
    glBufferData(target, size, data, usage);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlBufferSubData(kzUint target, kzInt offset, kzInt size, const void *data)
{
KZS_OPENGL_CALL_BEGIN
    glBufferSubData(target, offset, size, data);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlClear(kzUint mask)
{
KZS_OPENGL_CALL_BEGIN
    glClear(mask);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlClearStencil(kzInt s)
{
KZS_OPENGL_CALL_BEGIN
    glClearStencil(s);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlClientActiveTexture(kzUint texture)
{
KZS_OPENGL_CALL_BEGIN
    glClientActiveTexture(texture);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlColor4ub(kzU8 red, kzU8 green, kzU8 blue, kzU8 alpha)
{
KZS_OPENGL_CALL_BEGIN
    glColor4ub(red, green, blue, alpha);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlColorMask(kzBool red, kzBool green, kzBool blue, kzBool alpha)
{
KZS_OPENGL_CALL_BEGIN
    glColorMask(kzsGetGLBoolean_private(red), kzsGetGLBoolean_private(green), kzsGetGLBoolean_private(blue), kzsGetGLBoolean_private(alpha));
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlColorPointer(kzInt size, kzUint type, kzInt stride, const void *pointer)
{
KZS_OPENGL_CALL_BEGIN
    glColorPointer(size, type, stride, pointer);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlCompressedTexImage2D(kzUint target, kzInt level, kzUint internalformat, kzInt width, kzInt height, kzInt border, kzInt imageSize, const void *data)
{
KZS_OPENGL_CALL_BEGIN
    glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlCompressedTexSubImage2D(kzUint target, kzInt level, kzInt xoffset, kzInt yoffset, kzInt width, kzInt height, kzUint format, kzInt imageSize, const void *data)
{
KZS_OPENGL_CALL_BEGIN
    glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlCopyTexImage2D(kzUint target, kzInt level, kzUint internalformat, kzInt x, kzInt y, kzInt width, kzInt height, kzInt border)
{
KZS_OPENGL_CALL_BEGIN
    glCopyTexImage2D(target, level, internalformat, x, y, width, height, border);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlCopyTexSubImage2D(kzUint target, kzInt level, kzInt xoffset, kzInt yoffset, kzInt x, kzInt y, kzInt width, kzInt height)
{
KZS_OPENGL_CALL_BEGIN
    glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlCullFace(kzUint mode)
{
KZS_OPENGL_CALL_BEGIN
    glCullFace(mode);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlDeleteBuffers(kzInt n, const kzUint *buffers)
{
KZS_OPENGL_CALL_BEGIN
    glDeleteBuffers(n, buffers);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlDeleteTextures(kzInt n, const kzUint *textures)
{
KZS_OPENGL_CALL_BEGIN
    glDeleteTextures(n, textures);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlDepthFunc(kzUint func)
{
KZS_OPENGL_CALL_BEGIN
    glDepthFunc(func);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlDepthMask(kzBool flag)
{
KZS_OPENGL_CALL_BEGIN
    glDepthMask(kzsGetGLBoolean_private(flag));
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlDisable(kzUint cap)
{
KZS_OPENGL_CALL_BEGIN
    glDisable(cap);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlDisableClientState(kzUint array)
{
KZS_OPENGL_CALL_BEGIN
    glDisableClientState(array);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlDrawArrays(kzUint mode, kzInt first, kzInt count)
{
KZS_OPENGL_CALL_BEGIN
    glDrawArrays(mode, first, count);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlDrawElements(kzUint mode, kzInt count, kzUint type, const void *indices)
{
KZS_OPENGL_CALL_BEGIN
    glDrawElements(mode, count, type, indices);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlEnable(kzUint cap)
{
KZS_OPENGL_CALL_BEGIN
    glEnable(cap);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlEnableClientState(kzUint array)
{
KZS_OPENGL_CALL_BEGIN
    glEnableClientState(array);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlFinish(void)
{
KZS_OPENGL_CALL_BEGIN
    glFinish();
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlFlush(void)
{
KZS_OPENGL_CALL_BEGIN
    glFlush();
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlFrontFace(kzUint mode)
{
KZS_OPENGL_CALL_BEGIN
    glFrontFace(mode);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlGetBooleanv(kzUint pname, GLboolean *params) /* params is of type GLboolean, because this type cannot be casted to kzBool */
{
KZS_OPENGL_CALL_BEGIN
    glGetBooleanv(pname, params);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlGetBufferParameteriv(kzUint target, kzUint pname, kzInt *params)
{
KZS_OPENGL_CALL_BEGIN
    glGetBufferParameteriv(target, pname, params);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlGenBuffers(kzInt n, kzUint *buffers)
{
KZS_OPENGL_CALL_BEGIN
    glGenBuffers(n, buffers);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlGenTextures(kzInt n, kzUint *textures)
{
KZS_OPENGL_CALL_BEGIN
    glGenTextures(n, textures);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE kzUint kzsGlGetError(void)
{
KZS_OPENGL_CALL_BEGIN
    return glGetError();
KZS_OPENGL_CALL_END_RETURN_VALUE(KZS_GL_NO_ERROR)
}

KZ_INLINE void kzsGlGetIntegerv(kzUint pname, kzInt *params)
{
KZS_OPENGL_CALL_BEGIN
    glGetIntegerv(pname, params);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlGetPointerv(kzUint pname, void **params)
{
KZS_OPENGL_CALL_BEGIN
    glGetPointerv(pname, params);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE kzString kzsGlGetString(kzUint name)
{
    kzString returnValue = "";
KZS_OPENGL_CALL_BEGIN
    returnValue = (kzString)glGetString(name);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END_RETURN_VALUE(returnValue)
}

KZ_INLINE void kzsGlGetTexEnviv(kzUint env, kzUint pname, kzInt *params)
{
KZS_OPENGL_CALL_BEGIN
    glGetTexEnviv(env, pname, params);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlGetTexParameteriv(kzUint target, kzUint pname, kzInt *params)
{
KZS_OPENGL_CALL_BEGIN
    glGetTexParameteriv(target, pname, params);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlHint(kzUint target, kzUint mode)
{
KZS_OPENGL_CALL_BEGIN
    glHint(target, mode);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE kzBool kzsGlIsBuffer(kzUint buffer)
{
    kzBool isBuffer = KZ_FALSE;
KZS_OPENGL_CALL_BEGIN
    isBuffer = ((glIsBuffer(buffer) == GL_TRUE) ? KZ_TRUE : KZ_FALSE);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END_RETURN_VALUE(isBuffer)
}

KZ_INLINE kzBool kzsGlIsEnabled(kzUint cap)
{
    kzBool isEnabled = KZ_FALSE;
KZS_OPENGL_CALL_BEGIN
    isEnabled = ((glIsEnabled(cap) == GL_TRUE) ? KZ_TRUE : KZ_FALSE);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END_RETURN_VALUE(isEnabled)
}

KZ_INLINE kzBool kzsGlIsTexture(kzUint texture)
{
    kzBool isTexture = KZ_FALSE;
KZS_OPENGL_CALL_BEGIN
    isTexture = ((glIsTexture(texture) == GL_TRUE) ? KZ_TRUE : KZ_FALSE);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END_RETURN_VALUE(isTexture)
}

KZ_INLINE void kzsGlLoadIdentity(void)
{
KZS_OPENGL_CALL_BEGIN
    glLoadIdentity();
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlLogicOp(kzUint opcode)
{
KZS_OPENGL_CALL_BEGIN
    glLogicOp(opcode);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlMatrixMode(kzUint mode)
{
KZS_OPENGL_CALL_BEGIN
    glMatrixMode(mode);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlNormalPointer(kzUint type, kzInt stride, const void *pointer)
{
KZS_OPENGL_CALL_BEGIN
    glNormalPointer(type, stride, pointer);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlPixelStorei(kzUint pname, kzInt param)
{
KZS_OPENGL_CALL_BEGIN
    glPixelStorei(pname, param);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlPushMatrix(void)
{
KZS_OPENGL_CALL_BEGIN
    glPushMatrix();
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlPopMatrix(void)
{
KZS_OPENGL_CALL_BEGIN
    glPopMatrix();
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlReadPixels(kzInt x, kzInt y, kzInt width, kzInt height, kzUint format, kzUint type, void *pixels)
{
KZS_OPENGL_CALL_BEGIN
    glReadPixels(x, y, width, height, format, type, pixels);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlSampleCoverage(kzFloat value, kzBool invert)
{
KZS_OPENGL_CALL_BEGIN
    glSampleCoverage(value, kzsGetGLBoolean_private(invert));
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlScissor(kzInt x, kzInt y, kzInt width, kzInt height)
{
KZS_OPENGL_CALL_BEGIN
    glScissor(x, y, width, height);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlShadeModel(kzUint mode)
{
KZS_OPENGL_CALL_BEGIN
    glShadeModel(mode);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlStencilFunc(kzUint func, kzInt ref, kzUint mask)
{
KZS_OPENGL_CALL_BEGIN
    glStencilFunc(func, ref, mask);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlStencilMask(kzUint mask)
{
KZS_OPENGL_CALL_BEGIN
    glStencilMask(mask);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlStencilOp(kzUint fail, kzUint zfail, kzUint zpass)
{
KZS_OPENGL_CALL_BEGIN
    glStencilOp(fail, zfail, zpass);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlTexCoordPointer(kzInt size, kzUint type, kzInt stride, const void *pointer)
{
KZS_OPENGL_CALL_BEGIN
    glTexCoordPointer(size, type, stride, pointer);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlTexEnvi(kzUint target, kzUint pname, kzInt param)
{
KZS_OPENGL_CALL_BEGIN
    glTexEnvi(target, pname, param);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlTexEnviv(kzUint target, kzUint pname, const kzInt *params)
{
KZS_OPENGL_CALL_BEGIN
    glTexEnviv(target, pname, params);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlTexImage2D(kzUint target, kzInt level, kzUint internalformat, kzInt width, kzInt height, kzInt border, kzUint format, kzUint type, const void *pixels)
{
KZS_OPENGL_CALL_BEGIN
    glTexImage2D(target, level, (GLint)internalformat, width, height, border, format, type, pixels);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlTexParameteri(kzUint target, kzUint pname, kzInt param)
{
KZS_OPENGL_CALL_BEGIN
    glTexParameteri(target, pname, param);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlTexParameteriv(kzUint target, kzUint pname, const kzInt *params)
{
KZS_OPENGL_CALL_BEGIN
    glTexParameteriv(target, pname, params);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlTexSubImage2D(kzUint target, kzInt level, kzInt xoffset, kzInt yoffset, kzInt width, kzInt height, kzUint format, kzUint type, const void *pixels)
{
KZS_OPENGL_CALL_BEGIN
    glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlVertexPointer(kzInt size, kzUint type, kzInt stride, const void *pointer)
{
KZS_OPENGL_CALL_BEGIN
    glVertexPointer(size, type, stride, pointer);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlViewport(kzInt x, kzInt y, kzInt width, kzInt height)
{
KZS_OPENGL_CALL_BEGIN
    glViewport(x, y, width, height);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

/* Extension functions */
KZ_INLINE GLboolean kzsGlIsRenderbuffer(kzUint renderbuffer)
{
    GLboolean result = GL_FALSE;
KZS_OPENGL_CALL_BEGIN
    kzsAssertText(glIsRenderbufferOES!=KZ_NULL, "Extension function not initialized!");
    result = glIsRenderbufferOES(renderbuffer);
KZS_OPENGL_CALL_END_RETURN_VALUE(result)
}

KZ_INLINE void kzsGlBindRenderbuffer(kzUint target, kzUint renderbuffer)
{
KZS_OPENGL_CALL_BEGIN
    kzsAssertText(glBindRenderbufferOES!=KZ_NULL, "Extension function not initialized!");
    glBindRenderbufferOES(target, renderbuffer);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlDeleteRenderbuffers(kzInt n, const kzUint* renderbuffers)
{
KZS_OPENGL_CALL_BEGIN
    kzsAssertText(glDeleteRenderbuffersOES!=KZ_NULL, "Extension function not initialized!");
    glDeleteRenderbuffersOES(n, renderbuffers);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlGenRenderbuffers(kzInt n, kzUint* renderbuffers)
{
KZS_OPENGL_CALL_BEGIN
    kzsAssertText(glGenRenderbuffersOES!=KZ_NULL, "Extension function not initialized!");
    glGenRenderbuffersOES(n, renderbuffers);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}
KZ_INLINE void kzsGlRenderbufferStorage(kzUint target, kzUint internalformat, kzInt width, kzInt height)
{
KZS_OPENGL_CALL_BEGIN
    kzsAssertText(glRenderbufferStorageOES!=KZ_NULL, "Extension function not initialized!");
    glRenderbufferStorageOES(target, internalformat, width, height);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlGetRenderbufferParameteriv(kzUint target, kzUint pname, GLint* params)
{
KZS_OPENGL_CALL_BEGIN
    kzsAssertText(glGetRenderbufferParameterivOES!=KZ_NULL, "Extension function not initialized!");
    glGetRenderbufferParameterivOES(target, pname, params);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE kzBool kzsGlIsFramebuffer(kzUint framebuffer)
{
    kzBool result = KZ_FALSE;
KZS_OPENGL_CALL_BEGIN
    kzsAssertText(glIsFramebufferOES!=KZ_NULL, "Extension function not initialized!");
    result = glIsFramebufferOES(framebuffer) == GL_TRUE ? KZ_TRUE : KZ_FALSE;
KZS_OPENGL_CALL_END_RETURN_VALUE(result)
}

KZ_INLINE void kzsGlBindFramebuffer(kzUint target, kzUint framebuffer)
{
KZS_OPENGL_CALL_BEGIN
    kzsAssertText(glBindFramebufferOES != KZ_NULL, "Extension function not initialized!");
    glBindFramebufferOES(target, framebuffer);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlDeleteFramebuffers(kzInt n, const kzUint* framebuffers)
{
KZS_OPENGL_CALL_BEGIN
    kzsAssertText(glDeleteFramebuffersOES!=KZ_NULL, "Extension function not initialized!");
    glDeleteFramebuffersOES(n, framebuffers);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlGenFramebuffers(kzInt n, kzUint* framebuffers)
{
KZS_OPENGL_CALL_BEGIN
    kzsAssertText(glGenFramebuffersOES!=KZ_NULL, "Extension function not initialized!");
    glGenFramebuffersOES(n, framebuffers);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE kzUint kzsGlCheckFramebufferStatus(kzUint target)
{
    kzUint result = 0;
KZS_OPENGL_CALL_BEGIN
    kzsAssertText(glCheckFramebufferStatusOES!=KZ_NULL, "Extension function not initialized!");
    result = glCheckFramebufferStatusOES(target);
KZS_OPENGL_CALL_END_RETURN_VALUE(result)
}

KZ_INLINE void kzsGlFramebufferRenderbuffer(kzUint target, kzUint attachment, kzUint renderbuffertarget, kzUint renderbuffer)
{
KZS_OPENGL_CALL_BEGIN
    kzsAssertText(glFramebufferRenderbufferOES!=KZ_NULL, "Extension function not initialized!");
    glFramebufferRenderbufferOES(target, attachment, renderbuffertarget, renderbuffer);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlFramebufferTexture2D(kzUint target, kzUint attachment, kzUint textarget, kzUint texture, GLint level)
{
KZS_OPENGL_CALL_BEGIN
    kzsAssertText(glFramebufferTexture2DOES!=KZ_NULL, "Extension function not initialized!");
    glFramebufferTexture2DOES(target, attachment, textarget, texture, level);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlGetFramebufferAttachmentParameteriv(kzUint target, kzUint attachment, kzUint pname, GLint* params)
{
KZS_OPENGL_CALL_BEGIN
    kzsAssertText(glGetFramebufferAttachmentParameterivOES!=KZ_NULL, "Extension function not initialized!");
    glGetFramebufferAttachmentParameterivOES(target, attachment, pname, params);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void kzsGlGenerateMipmap(kzUint target)
{
KZS_OPENGL_CALL_BEGIN
    glGenerateMipmapOES(target);
    kzsGlErrorTest();
KZS_OPENGL_CALL_END
}

KZ_INLINE void glTexGenf(kzInt coord, kzInt pname, kzFloat param)
{
    KZS_NOT_IMPLEMENTED_YET;
}

KZ_INLINE void glTexGenfv(kzInt coord, kzInt pname, const kzFloat *params)
{
    KZS_NOT_IMPLEMENTED_YET;
}

KZ_INLINE void glTexGeni(kzInt coord, kzInt pname, kzInt param)
{
    KZS_NOT_IMPLEMENTED_YET;
}

KZ_INLINE void glTexGeniv(kzInt coord, kzInt pname, const kzInt *params)
{
    KZS_NOT_IMPLEMENTED_YET;
}

KZ_INLINE void glGetTexGenfv(kzInt coord, kzInt pname, kzFloat *params)
{
    KZS_NOT_IMPLEMENTED_YET;
}

KZ_INLINE void glGetTexGeniv(kzInt coord, kzInt pname, kzInt *params)
{
    KZS_NOT_IMPLEMENTED_YET;
}


#endif
