/**
 * \file
 * Application framework splash screen.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kza_splash.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/renderer/kzc_renderer.h>
#include <core/resource_manager/texture/kzc_texture_descriptor.h>
#include <core/resource_manager/texture/kzc_resource_texture.h>
#include <core/resource_manager/shader/kzc_resource_shader.h>
#include <core/util/collection/kzc_dynamic_array.h>

#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_opengl_base.h>
#include <system/wrappers/kzs_opengl.h>


extern const kzU8 kzaKanziLogoTop[];
extern const kzU8 kzaKanziLogoBottom[];
extern const kzU8 kzaKanziLogoBg[];


struct KzaSplashScreen
{
    kzUint textureHandleTop;
    kzUint textureHandleBottom;
    kzUint textureHandleBackground;
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    struct KzcShader* textureShader;
#endif
};

/** Create splash screen structure. */
kzsError kzaSplashScreenCreate(const struct KzcMemoryManager* memoryManager, struct KzcResourceManager* resourceManager, 
                               struct KzcRenderer* renderer, struct KzaSplashScreen** out_splash)
{
    kzsError result;
    struct KzaSplashScreen* splash;

    result = kzcMemoryAllocVariable(memoryManager, splash, "Splash screen structure");
    kzsErrorForward(result);

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    {
        struct KzcShader* textureShader;
        kzString fontVertexShaderCode =
            "attribute highp vec3 kzPosition;"
            "attribute highp vec2 kzTextureCoordinate0;"
            "uniform highp mat4 kzWorldMatrix;"
            "uniform highp mat4 kzProjectionMatrix;"
            "varying mediump vec2 vTexCoord;"
            "void main()"
            "{"
            "   vec4 positionWorld = kzWorldMatrix * vec4(kzPosition, 1.0);"
            "   vTexCoord = kzTextureCoordinate0;"
            "   gl_Position = kzProjectionMatrix * positionWorld;"
            "}";

        kzString fontFragmentShaderCode = 
            "uniform sampler2D kzTexture;"
            "varying mediump vec2 vTexCoord;"
            "void main()"
            "{"
            "   gl_FragColor = texture2D(kzTexture, vTexCoord);"
            "}";

        /* Compile shader */
        {
            kzBool createSuccess;
            struct KzcDynamicArray* attributeList;
            kzString attributes[] = {
                "kzPosition",
                "kzTextureCoordinate0"
            };
            result = kzcDynamicArrayCreate(memoryManager, &attributeList);
            kzsErrorForward(result);
            result = kzcDynamicArrayAdd(attributeList, (kzMutableString)attributes[0]);
            kzsErrorForward(result);
            result = kzcDynamicArrayAdd(attributeList, (kzMutableString)attributes[1]);
            kzsErrorForward(result);
            result = kzcShaderSourceCreate(resourceManager, KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY, 
                                           fontVertexShaderCode, fontFragmentShaderCode, attributeList,
                                           &createSuccess, &textureShader);
            kzsErrorForward(result);
            result = kzcDynamicArrayDelete(attributeList);
            kzsErrorForward(result);
        }
        splash->textureShader = textureShader;
    }
#else
    KZ_UNUSED_PARAMETER(memoryManager);
    KZ_UNUSED_PARAMETER(resourceManager);
#endif

    {
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
        result = kzcShaderApply(splash->textureShader, renderer);
        kzsErrorForward(result);
        kzcRendererSetUniformTexture(renderer, "kzTexture");
#endif

        /* Create top image */
        kzsGlGenTextures(1, &splash->textureHandleTop);
        result = kzcRendererApplyTexture(renderer, splash->textureHandleTop, KZC_RENDERER_TEXTURE_TYPE_2D, 512, 128);
        kzsErrorForward(result);
        kzsGlTexImage2D(KZS_GL_TEXTURE_2D, 0, KZS_GL_RGBA, (kzInt)512, (kzInt)128, 0, KZS_GL_RGBA,
            KZS_GL_UNSIGNED_BYTE, kzaKanziLogoTop);
        kzsGlTexParameteri(KZS_GL_TEXTURE_2D, KZS_GL_TEXTURE_MIN_FILTER, KZS_GL_LINEAR);
        kzsGlTexParameteri(KZS_GL_TEXTURE_2D, KZS_GL_TEXTURE_MAG_FILTER, KZS_GL_LINEAR);

        /* Create bottom image */
        kzsGlGenTextures(1, &splash->textureHandleBottom);
        result = kzcRendererApplyTexture(renderer, splash->textureHandleBottom, KZC_RENDERER_TEXTURE_TYPE_2D, 512, 64);
        kzsErrorForward(result);
        kzsGlTexImage2D(KZS_GL_TEXTURE_2D, 0, KZS_GL_RGBA, (kzInt)512, (kzInt)64, 0, KZS_GL_RGBA,
            KZS_GL_UNSIGNED_BYTE, kzaKanziLogoBottom);
        kzsGlTexParameteri(KZS_GL_TEXTURE_2D, KZS_GL_TEXTURE_MIN_FILTER, KZS_GL_LINEAR);
        kzsGlTexParameteri(KZS_GL_TEXTURE_2D, KZS_GL_TEXTURE_MAG_FILTER, KZS_GL_LINEAR);

        /* Create background image */
        kzsGlGenTextures(1, &splash->textureHandleBackground);
        result = kzcRendererApplyTexture(renderer, splash->textureHandleBackground, KZC_RENDERER_TEXTURE_TYPE_2D, 256, 256);
        kzsErrorForward(result);
        kzsGlTexImage2D(KZS_GL_TEXTURE_2D, 0, KZS_GL_LUMINANCE, (kzInt)256, (kzInt)256, 0, KZS_GL_LUMINANCE,
            KZS_GL_UNSIGNED_BYTE, kzaKanziLogoBg);
        kzsGlTexParameteri(KZS_GL_TEXTURE_2D, KZS_GL_TEXTURE_MIN_FILTER, KZS_GL_NEAREST);
        kzsGlTexParameteri(KZS_GL_TEXTURE_2D, KZS_GL_TEXTURE_MAG_FILTER, KZS_GL_NEAREST);

    }

    *out_splash = splash;
    kzsSuccess();
}


kzsError kzaSplashScreenDelete(struct KzaSplashScreen* splash, struct KzcRenderer* renderer)
{
    kzsError result;

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    result = kzcShaderDelete(splash->textureShader);
    kzsErrorForward(result);
#endif
    /* Clean-up */
    kzsGlDeleteTextures(1, &splash->textureHandleBackground);
    kzsGlDeleteTextures(1, &splash->textureHandleTop);
    kzsGlDeleteTextures(1, &splash->textureHandleBottom);

    result = kzcRendererReset(renderer);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(splash);
    kzsErrorForward(result);
    kzsSuccess();
}


kzsError kzaSplashScreenRender(const struct KzaSplashScreen* splash, struct KzcRenderer* renderer, const struct KzsWindow* window)
{
    kzsError result;
    kzUint windowWidth;
    kzUint windowHeight;
   
    {
        struct KzcColorRGBA clearColor = kzcColorRGBA(1.0f, 1.0f, 1.0f, 0.0f);
        kzcRendererSetClearColor(renderer, clearColor);
        kzcRendererResetClearTarget(renderer);
        kzcRendererAddClearTarget(renderer, KZC_RENDERER_CLEAR_TARGET_COLOR);
        kzcRendererClear(renderer); 
    }
    windowWidth = kzsWindowGetWidth(window);
    windowHeight = kzsWindowGetHeight(window);
    kzcRendererSetViewport(renderer, 0, 0, windowWidth, windowHeight);
 
    /* Render */
    {
        kzFloat positions[18];
        kzFloat uvs[12];
        kzFloat viewport[4];
        kzUint offset;

        kzFloat x1 = 0.0f;
        kzFloat y1 = 0.0f;
        kzFloat x2 = (kzFloat)windowWidth;
        kzFloat y2 = (kzFloat)windowHeight;
 
        struct KzcMatrix4x4 originalProjection = kzcRendererGetMatrix(renderer, KZC_RENDERER_MATRIX_PROJECTION);
        struct KzcMatrix4x4 originalWorld = kzcRendererGetMatrix(renderer, KZC_RENDERER_MATRIX_WORLD);
        struct KzcMatrix4x4 originalCamera = kzcRendererGetMatrix(renderer, KZC_RENDERER_MATRIX_CAMERA);
        struct KzcMatrix4x4 projection;

        /* Create texture */
        kzcRendererSetActiveTextureUnit(renderer, 0);

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
        result = kzcShaderApply(splash->textureShader, renderer);
        kzsErrorForward(result);
        kzcRendererSetUniformTexture(renderer, "kzTexture");
#endif
        kzsGlPixelStorei(KZS_GL_UNPACK_ALIGNMENT, 1);
 
        /* Setup material */
        kzcRendererBeginMaterialSetup(renderer);
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
        kzcRendererSetUniformTexture(renderer, "kzTexture");
#elif KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
        kzcRendererSetEmissiveColor(renderer, KZC_COLOR_WHITE);
#endif
        kzcRendererSetBlendMode(renderer, KZC_RENDERER_BLEND_MODE_ALPHA);
        kzcRendererSetCullMode(renderer, KZC_RENDERER_CULL_MODE_NONE);
        kzcRendererDisableState(renderer, KZC_RENDERER_STATE_DEPTH_TEST);
        kzcRendererDisableState(renderer, KZC_RENDERER_STATE_DEPTH_WRITE);
        result = kzcRendererApplyTexture(renderer, splash->textureHandleBackground, KZC_RENDERER_TEXTURE_TYPE_2D, 256, 256);
        kzsErrorForward(result);
        kzcRendererEndMaterialSetup(renderer);
 
        /* Setup viewport */
        kzcRendererGetViewport(renderer, &viewport[0], &viewport[1], &viewport[2], &viewport[3]);
        kzcMatrix4x4CreateOrthogonalProjection(&projection, 
            viewport[0], viewport[0] + viewport[2], viewport[1],
            viewport[1] + viewport[3], -1.0f, 1.0f);
 
        /* Setup matrices */
        kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_PROJECTION, &projection);
        kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_CAMERA, &KZC_MATRIX4X4_IDENTITY);
        kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_WORLD, &KZC_MATRIX4X4_IDENTITY);
 
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
        kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_PROJECTION_CAMERA_WORLD, &projection);
#endif
        result = kzcRendererApplyTransformation(renderer);
        kzsErrorForward(result);
 
        /* Disable index and vertex buffers */
        kzcRendererSetBuffer(renderer, KZC_RENDERER_BUFFER_INDEX, KZC_RENDERER_BUFFER_VALUE_NONE);
        kzcRendererSetBuffer(renderer, KZC_RENDERER_BUFFER_VERTEX, KZC_RENDERER_BUFFER_VALUE_NONE);
 
        /* Background */
        {

            offset = 0;
            positions[offset++] = x1; positions[offset++] = y1; positions[offset++] = 0.0f;
            positions[offset++] = x1; positions[offset++] = y2; positions[offset++] = 0.0f;
            positions[offset++] = x2; positions[offset++] = y1; positions[offset++] = 0.0f;
            positions[offset++] = x1; positions[offset++] = y2; positions[offset++] = 0.0f;
            positions[offset++] = x2; positions[offset++] = y2; positions[offset++] = 0.0f;
            positions[offset++] = x2; positions[offset++] = y1; positions[offset++] = 0.0f;

            offset = 0;
            uvs[offset++] = 0.0f; uvs[offset++] = 1.0f;
            uvs[offset++] = 0.0f; uvs[offset++] = 0.0f;
            uvs[offset++] = 1.0f; uvs[offset++] = 1.0f;
            uvs[offset++] = 0.0f; uvs[offset++] = 0.0f;
            uvs[offset++] = 1.0f; uvs[offset++] = 0.0f;
            uvs[offset++] = 1.0f; uvs[offset++] = 1.0f;

            kzcRendererBeginVertexArray(renderer, KZC_VERTEX_ARRAY_POSITION | KZC_VERTEX_ARRAY_TEXTURE_COORDINATE);
            kzcRendererSetVertexArrayData(renderer, KZC_VERTEX_ARRAY_POSITION, positions);
            kzcRendererSetVertexArrayData(renderer, KZC_VERTEX_ARRAY_TEXTURE_COORDINATE, uvs);
            kzcRendererEndVertexArray(renderer, KZC_PRIMITIVE_TYPE_TRIANGLES, 6);
        }

        /* Top */
        {
            x1 = 0.0f;
            y1 = (kzFloat)windowHeight - 128.0f;
            x2 = 512.0f;
            y2 = (kzFloat)windowHeight;

            result = kzcRendererApplyTexture(renderer, splash->textureHandleTop, KZC_RENDERER_TEXTURE_TYPE_2D, 512, 128);
            kzsErrorForward(result);

            offset = 0;
            positions[offset++] = x1; positions[offset++] = y1; positions[offset++] = 0.0f;
            positions[offset++] = x1; positions[offset++] = y2; positions[offset++] = 0.0f;
            positions[offset++] = x2; positions[offset++] = y1; positions[offset++] = 0.0f;
            positions[offset++] = x1; positions[offset++] = y2; positions[offset++] = 0.0f;
            positions[offset++] = x2; positions[offset++] = y2; positions[offset++] = 0.0f;
            positions[offset++] = x2; positions[offset++] = y1; positions[offset++] = 0.0f;

            offset = 0;
            uvs[offset++] = 0.0f; uvs[offset++] = 1.0f;
            uvs[offset++] = 0.0f; uvs[offset++] = 0.0f;
            uvs[offset++] = 1.0f; uvs[offset++] = 1.0f;
            uvs[offset++] = 0.0f; uvs[offset++] = 0.0f;
            uvs[offset++] = 1.0f; uvs[offset++] = 0.0f;
            uvs[offset++] = 1.0f; uvs[offset++] = 1.0f;

            kzcRendererBeginVertexArray(renderer, KZC_VERTEX_ARRAY_POSITION | KZC_VERTEX_ARRAY_TEXTURE_COORDINATE);
            kzcRendererSetVertexArrayData(renderer, KZC_VERTEX_ARRAY_POSITION, positions);
            kzcRendererSetVertexArrayData(renderer, KZC_VERTEX_ARRAY_TEXTURE_COORDINATE, uvs);
            kzcRendererEndVertexArray(renderer, KZC_PRIMITIVE_TYPE_TRIANGLES, 6);
        }

        /* Bottom */
        {
            x1 = 0.0f;
            y1 = 0.0f;
            x2 = (kzFloat)512;
            y2 = (kzFloat)64;

            result = kzcRendererApplyTexture(renderer, splash->textureHandleBottom, KZC_RENDERER_TEXTURE_TYPE_2D, 512, 64);
            kzsErrorForward(result);

            offset = 0;
            positions[offset++] = x1; positions[offset++] = y1; positions[offset++] = 0.0f;
            positions[offset++] = x1; positions[offset++] = y2; positions[offset++] = 0.0f;
            positions[offset++] = x2; positions[offset++] = y1; positions[offset++] = 0.0f;
            positions[offset++] = x1; positions[offset++] = y2; positions[offset++] = 0.0f;
            positions[offset++] = x2; positions[offset++] = y2; positions[offset++] = 0.0f;
            positions[offset++] = x2; positions[offset++] = y1; positions[offset++] = 0.0f;

            offset = 0;
            uvs[offset++] = 0.0f; uvs[offset++] = 1.0f;
            uvs[offset++] = 0.0f; uvs[offset++] = 0.0f;
            uvs[offset++] = 1.0f; uvs[offset++] = 1.0f;
            uvs[offset++] = 0.0f; uvs[offset++] = 0.0f;
            uvs[offset++] = 1.0f; uvs[offset++] = 0.0f;
            uvs[offset++] = 1.0f; uvs[offset++] = 1.0f;

            kzcRendererBeginVertexArray(renderer, KZC_VERTEX_ARRAY_POSITION | KZC_VERTEX_ARRAY_TEXTURE_COORDINATE);
            kzcRendererSetVertexArrayData(renderer, KZC_VERTEX_ARRAY_POSITION, positions);
            kzcRendererSetVertexArrayData(renderer, KZC_VERTEX_ARRAY_TEXTURE_COORDINATE, uvs);
            kzcRendererEndVertexArray(renderer, KZC_PRIMITIVE_TYPE_TRIANGLES, 6);
        }

        /* Restore matrices */
        kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_PROJECTION, &originalProjection);
        kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_WORLD, &originalWorld);
        kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_CAMERA, &originalCamera); 
    }
  
    kzsSuccess();
}
