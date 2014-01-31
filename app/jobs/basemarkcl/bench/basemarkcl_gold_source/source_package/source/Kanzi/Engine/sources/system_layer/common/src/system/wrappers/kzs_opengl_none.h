/**
* \file
* OpenGL ES none wrapper.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_OPENGL_NONE_H
#define KZS_OPENGL_NONE_H


/* System */
#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


#define KZS_EMPTY_OPENGL_FUNCTION


/* OpenGL ES core versions */
#define KZS_GL_VERSION_ES_CM_1_0        1
#define KZS_GL_VERSION_ES_CM_1_1        1

/* ClearBufferMask */
#define KZS_GL_DEPTH_BUFFER_BIT               0x00000100
#define KZS_GL_STENCIL_BUFFER_BIT             0x00000400
#define KZS_GL_COLOR_BUFFER_BIT               0x00004000

/* Boolean */
#define KZS_GL_FALSE                          0
#define KZS_GL_TRUE                           1

/* BeginMode */
#define KZS_GL_POINTS                         0x0000
#define KZS_GL_LINES                          0x0001
#define KZS_GL_LINE_LOOP                      0x0002
#define KZS_GL_LINE_STRIP                     0x0003
#define KZS_GL_TRIANGLES                      0x0004
#define KZS_GL_TRIANGLE_STRIP                 0x0005
#define KZS_GL_TRIANGLE_FAN                   0x0006

/* AlphaFunction */
#define KZS_GL_NEVER                          0x0200
#define KZS_GL_LESS                           0x0201
#define KZS_GL_EQUAL                          0x0202
#define KZS_GL_LEQUAL                         0x0203
#define KZS_GL_GREATER                        0x0204
#define KZS_GL_NOTEQUAL                       0x0205
#define KZS_GL_GEQUAL                         0x0206
#define KZS_GL_ALWAYS                         0x0207

/* BlendingFactorDest */
#define KZS_GL_ZERO                           0
#define KZS_GL_ONE                            1
#define KZS_GL_SRC_COLOR                      0x0300
#define KZS_GL_ONE_MINUS_SRC_COLOR            0x0301
#define KZS_GL_SRC_ALPHA                      0x0302
#define KZS_GL_ONE_MINUS_SRC_ALPHA            0x0303
#define KZS_GL_DST_ALPHA                      0x0304
#define KZS_GL_ONE_MINUS_DST_ALPHA            0x0305

#define KZS_GL_DST_COLOR                      0x0306
#define KZS_GL_ONE_MINUS_DST_COLOR            0x0307
#define KZS_GL_SRC_ALPHA_SATURATE             0x0308

/* TextureEnvParameter */
#define KZS_GL_TEXTURE_ENV_MODE               0x2200
#define KZS_GL_TEXTURE_ENV_COLOR              0x2201

/* TextureEnvTarget */
#define KZS_GL_TEXTURE_ENV                    0x2300

/* ClipPlaneName */
#define KZS_GL_CLIP_PLANE0                    0x3000
#define KZS_GL_CLIP_PLANE1                    0x3001
#define KZS_GL_CLIP_PLANE2                    0x3002
#define KZS_GL_CLIP_PLANE3                    0x3003
#define KZS_GL_CLIP_PLANE4                    0x3004
#define KZS_GL_CLIP_PLANE5                    0x3005

/* CullFaceMode */
#define KZS_GL_FRONT                          0x0404
#define KZS_GL_BACK                           0x0405
#define KZS_GL_FRONT_AND_BACK                 0x0408

/* EnableCap */
#define KZS_GL_FOG                            0x0B60
#define KZS_GL_LIGHTING                       0x0B50
#define KZS_GL_TEXTURE_2D                     0x0DE1
#define KZS_GL_CULL_FACE                      0x0B44
#define KZS_GL_ALPHA_TEST                     0x0BC0
#define KZS_GL_BLEND                          0x0BE2
#define KZS_GL_COLOR_LOGIC_OP                 0x0BF2
#define KZS_GL_DITHER                         0x0BD0
#define KZS_GL_STENCIL_TEST                   0x0B90
#define KZS_GL_DEPTH_TEST                     0x0B71
#define KZS_GL_POINT_SMOOTH                   0x0B10
#define KZS_GL_LINE_SMOOTH                    0x0B20
#define KZS_GL_SCISSOR_TEST                   0x0C11
#define KZS_GL_COLOR_MATERIAL                 0x0B57
#define KZS_GL_NORMALIZE                      0x0BA1
#define KZS_GL_RESCALE_NORMAL                 0x803A
#define KZS_GL_POLYGON_OFFSET_FILL            0x8037
#define KZS_GL_VERTEX_ARRAY                   0x8074
#define KZS_GL_NORMAL_ARRAY                   0x8075
#define KZS_GL_COLOR_ARRAY                    0x8076
#define KZS_GL_TEXTURE_COORD_ARRAY            0x8078
#define KZS_GL_MULTISAMPLE                    0x809D
#define KZS_GL_SAMPLE_ALPHA_TO_COVERAGE       0x809E
#define KZS_GL_SAMPLE_ALPHA_TO_ONE            0x809F
#define KZS_GL_SAMPLE_COVERAGE                0x80A0

/* ErrorCode */
#define KZS_GL_NO_ERROR                       0
#define KZS_GL_INVALID_ENUM                   0x0500
#define KZS_GL_INVALID_VALUE                  0x0501
#define KZS_GL_INVALID_OPERATION              0x0502
#define KZS_GL_STACK_OVERFLOW                 0x0503
#define KZS_GL_STACK_UNDERFLOW                0x0504
#define KZS_GL_OUT_OF_MEMORY                  0x0505

/* FogMode */
/*      KZS_GL_LINEAR */
#define KZS_GL_EXP                            0x0800
#define KZS_GL_EXP2                           0x0801

/* FogParameter */
#define KZS_GL_FOG_DENSITY                    0x0B62
#define KZS_GL_FOG_START                      0x0B63
#define KZS_GL_FOG_END                        0x0B64
#define KZS_GL_FOG_MODE                       0x0B65
#define KZS_GL_FOG_COLOR                      0x0B66

/* FrontFaceDirection */
#define KZS_GL_CW                             0x0900
#define KZS_GL_CCW                            0x0901

/* GetPName */
#define KZS_GL_CURRENT_COLOR                  0x0B00
#define KZS_GL_CURRENT_NORMAL                 0x0B02
#define KZS_GL_CURRENT_TEXTURE_COORDS         0x0B03
#define KZS_GL_POINT_SIZE                     0x0B11
#define KZS_GL_POINT_SIZE_MIN                 0x8126
#define KZS_GL_POINT_SIZE_MAX                 0x8127
#define KZS_GL_POINT_FADE_THRESHOLD_SIZE      0x8128
#define KZS_GL_POINT_DISTANCE_ATTENUATION     0x8129
#define KZS_GL_SMOOTH_POINT_SIZE_RANGE        0x0B12
#define KZS_GL_LINE_WIDTH                     0x0B21
#define KZS_GL_SMOOTH_LINE_WIDTH_RANGE        0x0B22
#define KZS_GL_ALIASED_POINT_SIZE_RANGE       0x846D
#define KZS_GL_ALIASED_LINE_WIDTH_RANGE       0x846E
#define KZS_GL_CULL_FACE_MODE                 0x0B45
#define KZS_GL_FRONT_FACE                     0x0B46
#define KZS_GL_SHADE_MODEL                    0x0B54
#define KZS_GL_DEPTH_RANGE                    0x0B70
#define KZS_GL_DEPTH_WRITEMASK                0x0B72
#define KZS_GL_DEPTH_CLEAR_VALUE              0x0B73
#define KZS_GL_DEPTH_FUNC                     0x0B74
#define KZS_GL_STENCIL_CLEAR_VALUE            0x0B91
#define KZS_GL_STENCIL_FUNC                   0x0B92
#define KZS_GL_STENCIL_VALUE_MASK             0x0B93
#define KZS_GL_STENCIL_FAIL                   0x0B94
#define KZS_GL_STENCIL_PASS_DEPTH_FAIL        0x0B95
#define KZS_GL_STENCIL_PASS_DEPTH_PASS        0x0B96
#define KZS_GL_STENCIL_REF                    0x0B97
#define KZS_GL_STENCIL_WRITEMASK              0x0B98
#define KZS_GL_MATRIX_MODE                    0x0BA0
#define KZS_GL_VIEWPORT                       0x0BA2
#define KZS_GL_MODELVIEW_STACK_DEPTH          0x0BA3
#define KZS_GL_PROJECTION_STACK_DEPTH         0x0BA4
#define KZS_GL_TEXTURE_STACK_DEPTH            0x0BA5
#define KZS_GL_MODELVIEW_MATRIX               0x0BA6
#define KZS_GL_PROJECTION_MATRIX              0x0BA7
#define KZS_GL_TEXTURE_MATRIX                 0x0BA8
#define KZS_GL_ALPHA_TEST_FUNC                0x0BC1
#define KZS_GL_ALPHA_TEST_REF                 0x0BC2
#define KZS_GL_BLEND_DST                      0x0BE0
#define KZS_GL_BLEND_SRC                      0x0BE1
#define KZS_GL_LOGIC_OP_MODE                  0x0BF0
#define KZS_GL_SCISSOR_BOX                    0x0C10
#define KZS_GL_COLOR_CLEAR_VALUE              0x0C22
#define KZS_GL_COLOR_WRITEMASK                0x0C23
#define KZS_GL_UNPACK_ALIGNMENT               0x0CF5
#define KZS_GL_PACK_ALIGNMENT                 0x0D05
#define KZS_GL_MAX_LIGHTS                     0x0D31
#define KZS_GL_MAX_CLIP_PLANES                0x0D32
#define KZS_GL_MAX_TEXTURE_SIZE               0x0D33
#define KZS_GL_MAX_MODELVIEW_STACK_DEPTH      0x0D36
#define KZS_GL_MAX_PROJECTION_STACK_DEPTH     0x0D38
#define KZS_GL_MAX_TEXTURE_STACK_DEPTH        0x0D39
#define KZS_GL_MAX_VIEWPORT_DIMS              0x0D3A
#define KZS_GL_MAX_TEXTURE_UNITS              0x84E2
#define KZS_GL_SUBPIXEL_BITS                  0x0D50
#define KZS_GL_RED_BITS                       0x0D52
#define KZS_GL_GREEN_BITS                     0x0D53
#define KZS_GL_BLUE_BITS                      0x0D54
#define KZS_GL_ALPHA_BITS                     0x0D55
#define KZS_GL_DEPTH_BITS                     0x0D56
#define KZS_GL_STENCIL_BITS                   0x0D57
#define KZS_GL_POLYGON_OFFSET_UNITS           0x2A00
#define KZS_GL_POLYGON_OFFSET_FACTOR          0x8038
#define KZS_GL_TEXTURE_BINDING_2D             0x8069
#define KZS_GL_VERTEX_ARRAY_SIZE              0x807A
#define KZS_GL_VERTEX_ARRAY_TYPE              0x807B
#define KZS_GL_VERTEX_ARRAY_STRIDE            0x807C
#define KZS_GL_NORMAL_ARRAY_TYPE              0x807E
#define KZS_GL_NORMAL_ARRAY_STRIDE            0x807F
#define KZS_GL_COLOR_ARRAY_SIZE               0x8081
#define KZS_GL_COLOR_ARRAY_TYPE               0x8082
#define KZS_GL_COLOR_ARRAY_STRIDE             0x8083
#define KZS_GL_TEXTURE_COORD_ARRAY_SIZE       0x8088
#define KZS_GL_TEXTURE_COORD_ARRAY_TYPE       0x8089
#define KZS_GL_TEXTURE_COORD_ARRAY_STRIDE     0x808A
#define KZS_GL_VERTEX_ARRAY_POINTER           0x808E
#define KZS_GL_NORMAL_ARRAY_POINTER           0x808F
#define KZS_GL_COLOR_ARRAY_POINTER            0x8090
#define KZS_GL_TEXTURE_COORD_ARRAY_POINTER    0x8092
#define KZS_GL_SAMPLE_BUFFERS                 0x80A8
#define KZS_GL_SAMPLES                        0x80A9
#define KZS_GL_SAMPLE_COVERAGE_VALUE          0x80AA
#define KZS_GL_SAMPLE_COVERAGE_INVERT         0x80AB

#define KZS_GL_NUM_COMPRESSED_TEXTURE_FORMATS 0x86A2
#define KZS_GL_COMPRESSED_TEXTURE_FORMATS     0x86A3

/* HintMode */
#define KZS_GL_DONT_CARE                      0x1100
#define KZS_GL_FASTEST                        0x1101
#define KZS_GL_NICEST                         0x1102

/* HintTarget */
#define KZS_GL_PERSPECTIVE_CORRECTION_HINT    0x0C50
#define KZS_GL_POINT_SMOOTH_HINT              0x0C51
#define KZS_GL_LINE_SMOOTH_HINT               0x0C52
#define KZS_GL_FOG_HINT                       0x0C54
#define KZS_GL_GENERATE_MIPMAP_HINT           0x8192

/* LightModelParameter */
#define KZS_GL_LIGHT_MODEL_AMBIENT            0x0B53
#define KZS_GL_LIGHT_MODEL_TWO_SIDE           0x0B52

/* LightParameter */
#define KZS_GL_AMBIENT                        0x1200
#define KZS_GL_DIFFUSE                        0x1201
#define KZS_GL_SPECULAR                       0x1202
#define KZS_GL_POSITION                       0x1203
#define KZS_GL_SPOT_DIRECTION                 0x1204
#define KZS_GL_SPOT_EXPONENT                  0x1205
#define KZS_GL_SPOT_CUTOFF                    0x1206
#define KZS_GL_CONSTANT_ATTENUATION           0x1207
#define KZS_GL_LINEAR_ATTENUATION             0x1208
#define KZS_GL_QUADRATIC_ATTENUATION          0x1209

/* DataType */
#define KZS_GL_BYTE                           0x1400
#define KZS_GL_UNSIGNED_BYTE                  0x1401
#define KZS_GL_SHORT                          0x1402
#define KZS_GL_UNSIGNED_SHORT                 0x1403
#define KZS_GL_FLOAT                          0x1406
#define KZS_GL_FIXED                          0x140C

/* LogicOp */
#define KZS_GL_CLEAR                          0x1500
#define KZS_GL_AND                            0x1501
#define KZS_GL_AND_REVERSE                    0x1502
#define KZS_GL_COPY                           0x1503
#define KZS_GL_AND_INVERTED                   0x1504
#define KZS_GL_NOOP                           0x1505
#define KZS_GL_XOR                            0x1506
#define KZS_GL_OR                             0x1507
#define KZS_GL_NOR                            0x1508
#define KZS_GL_EQUIV                          0x1509
#define KZS_GL_INVERT                         0x150A
#define KZS_GL_OR_REVERSE                     0x150B
#define KZS_GL_COPY_INVERTED                  0x150C
#define KZS_GL_OR_INVERTED                    0x150D
#define KZS_GL_NAND                           0x150E
#define KZS_GL_SET                            0x150F

/* MaterialParameter */
#define KZS_GL_EMISSION                       0x1600
#define KZS_GL_SHININESS                      0x1601
#define KZS_GL_AMBIENT_AND_DIFFUSE            0x1602

/* MatrixMode */
#define KZS_GL_MODELVIEW                      0x1700
#define KZS_GL_PROJECTION                     0x1701
#define KZS_GL_TEXTURE                        0x1702

/* PixelFormat */
#define KZS_GL_ALPHA                          0x1906
#define KZS_GL_RGB                            0x1907
#define KZS_GL_RGBA                           0x1908
#define KZS_GL_LUMINANCE                      0x1909
#define KZS_GL_LUMINANCE_ALPHA                0x190A

/* PixelType */
#define KZS_GL_UNSIGNED_SHORT_4_4_4_4         0x8033
#define KZS_GL_UNSIGNED_SHORT_5_5_5_1         0x8034
#define KZS_GL_UNSIGNED_SHORT_5_6_5           0x8363

/* ShadingModel */
#define KZS_GL_FLAT                           0x1D00
#define KZS_GL_SMOOTH                         0x1D01

/* StencilOp */
#define KZS_GL_KEEP                           0x1E00
#define KZS_GL_REPLACE                        0x1E01
#define KZS_GL_INCR                           0x1E02
#define KZS_GL_DECR                           0x1E03

/* StringName */
#define KZS_GL_VENDOR                         0x1F00
#define KZS_GL_RENDERER                       0x1F01
#define KZS_GL_VERSION                        0x1F02
#define KZS_GL_EXTENSIONS                     0x1F03

/* TextureEnvMode */
#define KZS_GL_MODULATE                       0x2100
#define KZS_GL_DECAL                          0x2101
/*      KZS_GL_BLEND */
#define KZS_GL_ADD                            0x0104
/*      KZS_GL_REPLACE */

/* TextureMagFilter */
#define KZS_GL_NEAREST                        0x2600
#define KZS_GL_LINEAR                         0x2601

/* TextureMinFilter */
#define KZS_GL_NEAREST_MIPMAP_NEAREST         0x2700
#define KZS_GL_LINEAR_MIPMAP_NEAREST          0x2701
#define KZS_GL_NEAREST_MIPMAP_LINEAR          0x2702
#define KZS_GL_LINEAR_MIPMAP_LINEAR           0x2703

/* TextureParameterName */
#define KZS_GL_TEXTURE_MAG_FILTER             0x2800
#define KZS_GL_TEXTURE_MIN_FILTER             0x2801
#define KZS_GL_TEXTURE_WRAP_S                 0x2802
#define KZS_GL_TEXTURE_WRAP_T                 0x2803
#define KZS_GL_GENERATE_MIPMAP                0x8191

/* TextureUnit */
#define KZS_GL_TEXTURE0                       0x84C0
#define KZS_GL_TEXTURE1                       0x84C1
#define KZS_GL_TEXTURE2                       0x84C2
#define KZS_GL_TEXTURE3                       0x84C3
#define KZS_GL_TEXTURE4                       0x84C4
#define KZS_GL_TEXTURE5                       0x84C5
#define KZS_GL_TEXTURE6                       0x84C6
#define KZS_GL_TEXTURE7                       0x84C7
#define KZS_GL_TEXTURE8                       0x84C8
#define KZS_GL_TEXTURE9                       0x84C9
#define KZS_GL_TEXTURE10                      0x84CA
#define KZS_GL_TEXTURE11                      0x84CB
#define KZS_GL_TEXTURE12                      0x84CC
#define KZS_GL_TEXTURE13                      0x84CD
#define KZS_GL_TEXTURE14                      0x84CE
#define KZS_GL_TEXTURE15                      0x84CF
#define KZS_GL_TEXTURE16                      0x84D0
#define KZS_GL_TEXTURE17                      0x84D1
#define KZS_GL_TEXTURE18                      0x84D2
#define KZS_GL_TEXTURE19                      0x84D3
#define KZS_GL_TEXTURE20                      0x84D4
#define KZS_GL_TEXTURE21                      0x84D5
#define KZS_GL_TEXTURE22                      0x84D6
#define KZS_GL_TEXTURE23                      0x84D7
#define KZS_GL_TEXTURE24                      0x84D8
#define KZS_GL_TEXTURE25                      0x84D9
#define KZS_GL_TEXTURE26                      0x84DA
#define KZS_GL_TEXTURE27                      0x84DB
#define KZS_GL_TEXTURE28                      0x84DC
#define KZS_GL_TEXTURE29                      0x84DD
#define KZS_GL_TEXTURE30                      0x84DE
#define KZS_GL_TEXTURE31                      0x84DF
#define KZS_GL_ACTIVE_TEXTURE                 0x84E0
#define KZS_GL_CLIENT_ACTIVE_TEXTURE          0x84E1

/* TextureWrapMode */
#define KZS_GL_REPEAT                         0x2901
#define KZS_GL_CLAMP_TO_EDGE                  0x812F

/* LightName */
#define KZS_GL_LIGHT0                         0x4000
#define KZS_GL_LIGHT1                         0x4001
#define KZS_GL_LIGHT2                         0x4002
#define KZS_GL_LIGHT3                         0x4003
#define KZS_GL_LIGHT4                         0x4004
#define KZS_GL_LIGHT5                         0x4005
#define KZS_GL_LIGHT6                         0x4006
#define KZS_GL_LIGHT7                         0x4007

/* Buffer Objects */
#define KZS_GL_ARRAY_BUFFER                   0x8892
#define KZS_GL_ELEMENT_ARRAY_BUFFER           0x8893

#define KZS_GL_ARRAY_BUFFER_BINDING               0x8894
#define KZS_GL_ELEMENT_ARRAY_BUFFER_BINDING       0x8895
#define KZS_GL_VERTEX_ARRAY_BUFFER_BINDING        0x8896
#define KZS_GL_NORMAL_ARRAY_BUFFER_BINDING        0x8897
#define KZS_GL_COLOR_ARRAY_BUFFER_BINDING         0x8898
#define KZS_GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING 0x889A

#define KZS_GL_STATIC_DRAW                    0x88E4
#define KZS_GL_DYNAMIC_DRAW                   0x88E8

#define KZS_GL_BUFFER_SIZE                    0x8764
#define KZS_GL_BUFFER_USAGE                   0x8765

/* Texture combine + dot3 */
#define KZS_GL_SUBTRACT                       0x84E7
#define KZS_GL_COMBINE                        0x8570
#define KZS_GL_COMBINE_RGB                    0x8571
#define KZS_GL_COMBINE_ALPHA                  0x8572
#define KZS_GL_RGB_SCALE                      0x8573
#define KZS_GL_ADD_SIGNED                     0x8574
#define KZS_GL_INTERPOLATE                    0x8575
#define KZS_GL_CONSTANT                       0x8576
#define KZS_GL_PRIMARY_COLOR                  0x8577
#define KZS_GL_PREVIOUS                       0x8578
#define KZS_GL_OPERAND0_RGB                   0x8590
#define KZS_GL_OPERAND1_RGB                   0x8591
#define KZS_GL_OPERAND2_RGB                   0x8592
#define KZS_GL_OPERAND0_ALPHA                 0x8598
#define KZS_GL_OPERAND1_ALPHA                 0x8599
#define KZS_GL_OPERAND2_ALPHA                 0x859A

#define KZS_GL_ALPHA_SCALE                    0x0D1C

#define KZS_GL_SRC0_RGB                       0x8580
#define KZS_GL_SRC1_RGB                       0x8581
#define KZS_GL_SRC2_RGB                       0x8582
#define KZS_GL_SRC0_ALPHA                     0x8588
#define KZS_GL_SRC1_ALPHA                     0x8589
#define KZS_GL_SRC2_ALPHA                     0x858A

#define KZS_GL_DOT3_RGB                       0x86AE
#define KZS_GL_DOT3_RGBA                      0x86AF

/*------------------------------------------------------------------------*
* required OES extension tokens
* _OES should be removed for compatibility with ES2 features
*------------------------------------------------------------------------*/

/* GL_OES_compressed_ETC1_RGB8_texture */
#define KZS_GL_ETC1_RGB8_OES                                        0x8D64

/* OES_read_format */
#ifndef KZS_GL_OES_read_format
#define KZS_GL_IMPLEMENTATION_COLOR_READ_TYPE_OES                   0x8B9A
#define KZS_GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES                 0x8B9B
#endif

/* KZS_GL_OES_compressed_paletted_texture */
#ifndef KZS_GL_OES_compressed_paletted_texture
#define KZS_GL_PALETTE4_RGB8_OES                                    0x8B90
#define KZS_GL_PALETTE4_RGBA8_OES                                   0x8B91
#define KZS_GL_PALETTE4_R5_G6_B5_OES                                0x8B92
#define KZS_GL_PALETTE4_RGBA4_OES                                   0x8B93
#define KZS_GL_PALETTE4_RGB5_A1_OES                                 0x8B94
#define KZS_GL_PALETTE8_RGB8_OES                                    0x8B95
#define KZS_GL_PALETTE8_RGBA8_OES                                   0x8B96
#define KZS_GL_PALETTE8_R5_G6_B5_OES                                0x8B97
#define KZS_GL_PALETTE8_RGBA4_OES                                   0x8B98
#define KZS_GL_PALETTE8_RGB5_A1_OES                                 0x8B99
#endif

/* OES_point_size_array */
#ifndef KZS_GL_OES_point_size_array
#define KZS_GL_POINT_SIZE_ARRAY_OES                                 0x8B9C
#define KZS_GL_POINT_SIZE_ARRAY_TYPE_OES                            0x898A
#define KZS_GL_POINT_SIZE_ARRAY_STRIDE_OES                          0x898B
#define KZS_GL_POINT_SIZE_ARRAY_POINTER_OES                         0x898C
#define KZS_GL_POINT_SIZE_ARRAY_BUFFER_BINDING_OES                  0x8B9F
#endif

/* KZS_GL_OES_point_sprite */
#ifndef KZS_GL_OES_point_sprite
#define KZS_GL_POINT_SPRITE_OES                                     0x8861
#define KZS_GL_COORD_REPLACE_OES                                    0x8862
#endif

/* OES_framebuffer_object */
#ifndef KZS_GL_OES_framebuffer_object
#define    KZS_GL_NONE                                                    0
#define    KZS_GL_FRAMEBUFFER                                            0
#define    KZS_GL_RENDERBUFFER                                            0
#define    KZS_GL_RGBA4                                                0
#define    KZS_GL_RGB5_A1                                                0
#define    KZS_GL_RGB565                                                0
#define    KZS_GL_DEPTH_COMPONENT16                                    0
#define    KZS_GL_RENDERBUFFER_WIDTH                                    0
#define    KZS_GL_RENDERBUFFER_HEIGHT                                    0
#define    KZS_GL_RENDERBUFFER_INTERNAL_FORMAT                            0
#define    KZS_GL_RENDERBUFFER_RED_SIZE                                0
#define    KZS_GL_RENDERBUFFER_GREEN_SIZE                                0
#define    KZS_GL_RENDERBUFFER_BLUE_SIZE                                0
#define    KZS_GL_RENDERBUFFER_ALPHA_SIZE                                0
#define    KZS_GL_RENDERBUFFER_DEPTH_SIZE                                0
#define    KZS_GL_RENDERBUFFER_STENCIL_SIZE                            0
#define    KZS_GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE                    0
#define    KZS_GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME                    0
#define    KZS_GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL                    0
#define    KZS_GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE            0
#define    KZS_GL_COLOR_ATTACHMENT0                                    0
#define    KZS_GL_DEPTH_ATTACHMENT                                        0
#define    KZS_GL_STENCIL_ATTACHMENT                                    0
#define    KZS_GL_FRAMEBUFFER_COMPLETE                                    0
#define    KZS_GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT                    1
#define    KZS_GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT            2
#define    KZS_GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS                    3
#define    KZS_GL_FRAMEBUFFER_INCOMPLETE_FORMATS                        4
#define    KZS_GL_FRAMEBUFFER_INCOMPLETE_UNSUPPORTED                    5
#define    KZS_GL_FRAMEBUFFER_BINDING                                    6
#define    KZS_GL_RENDERBUFFER_BINDING                                    7
#define    KZS_GL_MAX_RENDERBUFFER_SIZE                                8
#define    KZS_GL_INVALID_FRAMEBUFFER_OPERATION                        0
#endif

/* KZS_GL_OES_texture_cube_map */
#ifndef KZS_GL_OES_texture_cube_map
#define    KZS_GL_NORMAL_MAP                                            0xFFFF
#define    KZS_GL_REFLECTION_MAP                                        0xFFFF
#define    KZS_GL_TEXTURE_CUBE_MAP                                        0xFFFF
#define    KZS_GL_TEXTURE_BINDING_CUBE_MAP                                0xFFFF
#define    KZS_GL_TEXTURE_CUBE_MAP_POSITIVE_X                            0xFFFF
#define    KZS_GL_TEXTURE_CUBE_MAP_NEGATIVE_X                            0xFFFF
#define    KZS_GL_TEXTURE_CUBE_MAP_POSITIVE_Y                            0xFFFF
#define    KZS_GL_TEXTURE_CUBE_MAP_NEGATIVE_Y                            0xFFFF
#define    KZS_GL_TEXTURE_CUBE_MAP_POSITIVE_Z                            0xFFFF
#define    KZS_GL_TEXTURE_CUBE_MAP_NEGATIVE_Z                            0xFFFF
#define    KZS_GL_MAX_CUBE_MAP_TEXTURE_SIZE                            0xFFFF
#define    KZS_GL_TEXTURE_GEN_MODE                                        0xFFFF
#define    KZS_GL_TEXTURE_GEN_STR                                        0xFFFF
#endif


KZ_INLINE void kzsGlAlphaFunc(kzUint func, kzFloat ref)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlClearColor(kzFloat red, kzFloat green, kzFloat blue, kzFloat alpha)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlClearDepthf(kzFloat depth)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlClipPlanef(kzUint plane, const kzFloat *equation)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlColor4f(kzFloat red, kzFloat green, kzFloat blue, kzFloat alpha)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlDepthRangef(kzFloat zNear, kzFloat zFar)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlFogf(kzUint pname, kzFloat param)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlFogfv(kzUint pname, const kzFloat* params)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlFrustumf(kzFloat left, kzFloat right, kzFloat bottom, kzFloat top, kzFloat zNear, kzFloat zFar)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlGetClipPlanef(kzUint pname, kzFloat eqn[4])
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlGetFloatv(kzUint pname, kzFloat *params)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlGetLightfv(kzUint light, kzUint pname, kzFloat *params)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlGetMaterialfv(kzUint face, kzUint pname, kzFloat *params)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlGetTexEnvfv(kzUint env, kzUint pname, kzFloat *params)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlGetTexParameterfv(kzUint target, kzUint pname, kzFloat *params)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlLightModelf(kzUint pname, kzFloat param)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlLightModelfv(kzUint pname, const kzFloat *params)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlLightf(kzUint light, kzUint pname, kzFloat param)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlLightfv(kzUint light, kzUint pname, const kzFloat *params)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlLineWidth(kzFloat width)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlLoadMatrixf(const kzFloat *m)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlMaterialf(kzUint face, kzUint pname, kzFloat param)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlMaterialfv(kzUint face, kzUint pname, const kzFloat *params)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlMultMatrixf(const kzFloat *m)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlMultiTexCoord4f(kzUint target, kzFloat s, kzFloat t, kzFloat r, kzFloat q)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlNormal3f(kzFloat nx, kzFloat ny, kzFloat nz)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlOrthof(kzFloat left, kzFloat right, kzFloat bottom, kzFloat top, kzFloat zNear, kzFloat zFar)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlPointParameterf(kzUint pname, kzFloat param)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlPointParameterfv(kzUint pname, const kzFloat *params)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlPointSize(kzFloat size)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlPolygonOffset(kzFloat factor, kzFloat units)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlRotatef(kzFloat angle, kzFloat x, kzFloat y, kzFloat z)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlScalef(kzFloat x, kzFloat y, kzFloat z)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlTexEnvf(kzUint target, kzUint pname, kzFloat param)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlTexEnvfv(kzUint target, kzUint pname, const kzFloat *params)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlTexParameterf(kzUint target, kzUint pname, kzFloat param)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlTexParameterfv(kzUint target, kzUint pname, const kzFloat *params)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlTranslatef(kzFloat x, kzFloat y, kzFloat z)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlActiveTexture(kzUint texture)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlBindBuffer(kzUint target, kzUint buffer)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlBindTexture(kzUint target, kzUint texture)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlBlendFunc(kzUint sfactor, kzUint dfactor)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlBufferData(kzUint target, kzInt size, const void *data, kzUint usage)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlBufferSubData(kzUint target, kzInt offset, kzInt size, const void *data)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlClear(kzUint mask)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlClearStencil(kzInt s)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlClientActiveTexture(kzUint texture)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlColor4ub(kzU8 red, kzU8 green, kzU8 blue, kzU8 alpha)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlColorMask(kzBool red, kzBool green, kzBool blue, kzBool alpha)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlColorPointer(kzInt size, kzUint type, kzInt stride, const void *pointer)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlCompressedTexImage2D(kzUint target, kzInt level, kzUint internalformat, kzInt width, kzInt height, kzInt border, kzInt imageSize, const void *data)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlCompressedTexSubImage2D(kzUint target, kzInt level, kzInt xoffset, kzInt yoffset, kzInt width, kzInt height, kzUint format, kzInt imageSize, const void *data)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlCopyTexImage2D(kzUint target, kzInt level, kzUint internalformat, kzInt x, kzInt y, kzInt width, kzInt height, kzInt border)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlCopyTexSubImage2D(kzUint target, kzInt level, kzInt xoffset, kzInt yoffset, kzInt x, kzInt y, kzInt width, kzInt height)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlCullFace(kzUint mode)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlDeleteBuffers(kzInt n, const kzUint *buffers)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlDeleteTextures(kzInt n, const kzUint *textures)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlDepthFunc(kzUint func)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlDepthMask(kzBool flag)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlDisable(kzUint cap)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlDisableClientState(kzUint array)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlDrawArrays(kzUint mode, kzInt first, kzInt count)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlDrawElements(kzUint mode, kzInt count, kzUint type, const void *indices)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlEnable(kzUint cap)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlEnableClientState(kzUint array)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlFinish(void)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlFlush(void)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlFrontFace(kzUint mode)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlGetBooleanv(kzUint pname, kzBool *params)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlGetBufferParameteriv(kzUint target, kzUint pname, kzInt *params)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlGenBuffers(kzInt n, kzUint *buffers)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlGenTextures(kzInt n, kzUint *textures)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE kzUint kzsGlGetError(void)
{
    KZS_EMPTY_OPENGL_FUNCTION;
    return 0;
}

KZ_INLINE void kzsGlGetIntegerv(kzUint pname, kzInt *params)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlGetPointerv(kzUint pname, void **params)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE kzString kzsGlGetString(kzUint name)
{
    KZS_EMPTY_OPENGL_FUNCTION;
    return KZ_NULL;
}

KZ_INLINE void kzsGlGetTexEnviv(kzUint env, kzUint pname, kzInt *params)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlGetTexParameteriv(kzUint target, kzUint pname, kzInt *params)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlHint(kzUint target, kzUint mode)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE kzBool kzsGlIsBuffer(kzUint buffer)
{
    KZS_EMPTY_OPENGL_FUNCTION;
    return KZ_FALSE;
}

KZ_INLINE kzBool kzsGlIsEnabled(kzUint cap)
{
    KZS_EMPTY_OPENGL_FUNCTION;
    return KZ_FALSE;
}

KZ_INLINE kzBool kzsGlIsTexture(kzUint texture)
{
    KZS_EMPTY_OPENGL_FUNCTION;
    return KZ_FALSE;
}

KZ_INLINE void kzsGlLoadIdentity(void)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlLogicOp(kzUint opcode)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlMatrixMode(kzUint mode)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlNormalPointer(kzUint type, kzInt stride, const void *pointer)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlPixelStorei(kzUint pname, kzInt param)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlPushMatrix(void)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlPopMatrix(void)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlReadPixels(kzInt x, kzInt y, kzInt width, kzInt height, kzUint format, kzUint type, void *pixels)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlSampleCoverage(kzFloat value, kzBool invert)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlScissor(kzInt x, kzInt y, kzInt width, kzInt height)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlShadeModel(kzUint mode)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlStencilFunc(kzUint func, kzInt ref, kzUint mask)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlStencilMask(kzUint mask)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlStencilOp(kzUint fail, kzUint zfail, kzUint zpass)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlTexCoordPointer(kzInt size, kzUint type, kzInt stride, const void *pointer)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlTexEnvi(kzUint target, kzUint pname, kzInt param)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlTexEnviv(kzUint target, kzUint pname, const kzInt *params)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlTexImage2D(kzUint target, kzInt level, kzUint internalformat, kzInt width, kzInt height, kzInt border, kzUint format, kzUint type, const void *pixels)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlTexParameteri(kzUint target, kzUint pname, kzInt param)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlTexParameteriv(kzUint target, kzUint pname, const kzInt *params)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlTexSubImage2D(kzUint target, kzInt level, kzInt xoffset, kzInt yoffset, kzInt width, kzInt height, kzUint format, kzUint type, const void *pixels)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlVertexPointer(kzInt size, kzUint type, kzInt stride, const void *pointer)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlViewport(kzInt x, kzInt y, kzInt width, kzInt height)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

/* Extension functions */
KZ_INLINE kzBool kzsGlIsRenderbuffer(kzUint renderbuffer)
{
    KZS_EMPTY_OPENGL_FUNCTION;
    return KZ_FALSE;
}
KZ_INLINE void kzsGlBindRenderbuffer(kzUint target, kzUint renderbuffer)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlDeleteRenderbuffers(kzInt n, const kzUint* renderbuffers)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlGenRenderbuffers(kzInt n, kzUint* renderbuffers)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlRenderbufferStorage(kzUint target, kzUint internalformat, kzInt width, kzInt height)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlGetRenderbufferParameteriv(kzUint target, kzUint pname, kzInt* params)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE kzBool kzsGlIsFramebuffer(kzUint framebuffer)
{
    KZS_EMPTY_OPENGL_FUNCTION;
    return KZ_FALSE;
}

KZ_INLINE void kzsGlBindFramebuffer(kzUint target, kzUint framebuffer)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlDeleteFramebuffers(kzInt n, const kzUint* framebuffers)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlGenFramebuffers(kzInt n, kzUint* framebuffers)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE kzUint kzsGlCheckFramebufferStatus(kzUint target)
{
    KZS_EMPTY_OPENGL_FUNCTION;
    return 0;
}

KZ_INLINE void kzsGlFramebufferRenderbuffer(kzUint target, kzUint attachment, kzUint renderbuffertarget, kzUint renderbuffer)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlFramebufferTexture2D(kzUint target, kzUint attachment, kzUint textarget, kzUint texture, kzInt level)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlGetFramebufferAttachmentParameteriv(kzUint target, kzUint attachment, kzUint pname, kzInt* params)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void kzsGlGenerateMipmap(kzUint target)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void glTexGenf(kzInt coord, kzInt pname, kzFloat param)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void glTexGenfv(kzInt coord, kzInt pname, const kzFloat *params)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void glTexGeni(kzInt coord, kzInt pname, kzInt param)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void glTexGeniv(kzInt coord, kzInt pname, const kzInt *params)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void glGetTexGenfv(kzInt coord, kzInt pname, kzFloat *params)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

KZ_INLINE void glGetTexGeniv(kzInt coord, kzInt pname, kzInt *params)
{
    KZS_EMPTY_OPENGL_FUNCTION;
}

#define PFNGLISRENDERBUFFEROESPROC (void*)
#define PFNGLBINDRENDERBUFFEROESPROC (void*)
#define PFNGLDELETERENDERBUFFERSOESPROC (void*)
#define PFNGLGENRENDERBUFFERSOESPROC (void*)
#define PFNGLRENDERBUFFERSTORAGEOESPROC (void*)
#define PFNGLGETRENDERBUFFERPARAMETERIVOESPROC (void*)
#define PFNGLISFRAMEBUFFEROESPROC (void*)
#define PFNGLBINDFRAMEBUFFEROESPROC (void*)
#define PFNGLDELETEFRAMEBUFFERSOESPROC (void*)
#define PFNGLGENFRAMEBUFFERSOESPROC (void*)
#define PFNGLCHECKFRAMEBUFFERSTATUSOESPROC (void*)
#define PFNGLFRAMEBUFFERRENDERBUFFEROESPROC (void*)
#define PFNGLFRAMEBUFFERTEXTURE2DOESPROC (void*)
#define PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVOESPROC (void*)
#define PFNGLGENERATEMIPMAPOESPROC (void*)

#define PFNGLTEXGENFOESPROC 
#define PFNGLTEXGENFVOESPROC (void*)
#define PFNGLTEXGENIOESPROC (void*)
#define PFNGLTEXGENIVOESPROC (void*)
#define PFNGLTEXGENXOESPROC (void*)
#define PFNGLTEXGENXVOESPROC (void*)
#define PFNGLGETTEXGENFVOESPROC (void*)
#define PFNGLGETTEXGENIVOESPROC (void*)
#define PFNGLGETTEXGENXVOESPROC (void*)

typedef void (*PFNGLGENPTR) (kzUint coord);

PFNGLGENPTR glTexGenfOES;
PFNGLGENPTR glTexGenfvOES;
PFNGLGENPTR glTexGeniOES;


#endif
