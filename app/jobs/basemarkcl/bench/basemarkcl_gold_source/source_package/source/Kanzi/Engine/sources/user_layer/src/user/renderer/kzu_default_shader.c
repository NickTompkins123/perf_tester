/**
* \file
* Specifies a shader program codes for default shaders.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_default_shader.h"


const kzString KZU_DEFAULT_VERTEX_SHADER_SOURCE =
    "attribute vec3 kzPosition;\n"
    "uniform highp mat4 kzProjectionCameraWorldMatrix;\n"
    "void main()\n"
    "{\n"
    "   precision mediump float;\n"
    "   gl_Position = kzProjectionCameraWorldMatrix * vec4(kzPosition.xyz, 1.0);\n"
    "}\n";

const kzString KZU_DEFAULT_FRAGMENT_SHADER_SOURCE =
    "void main()\n"
    "{\n"
    "   gl_FragColor.rgb = vec3(0.0, 1.0, 0.0);\n"
    "   gl_FragColor.a = 1.0;\n"
    "}\n";

const kzString KZU_ERROR_VERTEX_SHADER_SOURCE =
    "attribute vec3 kzPosition;\n"
    "uniform highp mat4 kzProjectionCameraWorldMatrix;\n"
    "void main()\n"
    "{\n"
    "   precision mediump float;\n"
    "   gl_Position = kzProjectionCameraWorldMatrix * vec4(kzPosition.xyz, 1.0);\n"
    "}\n";

const kzString KZU_ERROR_FRAGMENT_SHADER_SOURCE =
    "void main()\n"
    "{\n"
    "   gl_FragColor.rgb = vec3(1.0, 0.0, 0.0);\n"
    "   gl_FragColor.a = 1.0;\n"
    "}\n";


/* TODO: Use KzProjectionCameraWorldMatrix instead of three matrix operations. */
const kzString KZU_SOLID_COLOR_VERTEX_SHADER_SOURCE =
    "attribute vec3 kzPosition;\n"
    "uniform highp mat4 kzWorldMatrix;\n"
    "uniform highp mat4 kzCameraMatrix;\n"
    "uniform highp mat4 kzProjectionMatrix;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = kzProjectionMatrix * kzCameraMatrix * kzWorldMatrix * vec4(kzPosition.xyz, 1.0);\n"
    "}\n";

const kzString KZU_SOLID_COLOR_FRAGMENT_SHADER_SOURCE =
    "uniform mediump vec4 Emissive;\n"
    "void main()\n"
    "{\n"
    "   gl_FragColor.rgba = Emissive;\n"
    "}\n";

/* TODO: Use KzProjectionCameraWorldMatrix instead of three matrix operations. */
const kzString KZU_TEXTURED_VERTEX_SHADER_SOURCE =
    "attribute highp vec3 kzPosition;"
    "attribute highp vec2 kzTextureCoordinate0;"
    "uniform highp mat4 kzWorldMatrix;"
    "uniform highp mat4 kzCameraMatrix;"
    "uniform highp mat4 kzProjectionMatrix;"
    "varying mediump vec2 vTexCoord;"
    "void main()"
    "{"
    "   vec4 positionWorld = kzWorldMatrix * vec4(kzPosition, 1.0);"
    "   vTexCoord = kzTextureCoordinate0;"
    "   gl_Position = kzProjectionMatrix * kzCameraMatrix * positionWorld;"
    "}";

const kzString KZU_TEXTURED_FRAGMENT_SHADER_SOURCE =
    "uniform sampler2D kzTexture;"
    "varying mediump vec2 vTexCoord;"
    "void main()"
    "{"
    "   precision mediump float;"
    "   gl_FragColor.rgba = texture2D(kzTexture, vTexCoord).rgba;"
    "}";
