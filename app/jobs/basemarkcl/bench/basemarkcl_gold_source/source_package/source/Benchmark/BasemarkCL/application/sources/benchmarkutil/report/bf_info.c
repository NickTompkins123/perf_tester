/**
* \file
* OpenCL benchmark reporting info. 
* Collects information about the platform.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "bf_info.h"

#include <benchmarkutil/report/xml/bf_xml_document.h>
#include <benchmarkutil/report/xml/bf_xml_node.h>
#include <benchmarkutil/report/xml/bf_xml_attribute.h>
#include <benchmarkutil/report/bf_report.h>

#include <clutil/clu_platform.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/settings/kzc_settings.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/util/string/kzc_string.h>
#include <core/util/string/kzc_string_buffer.h>
#include <core/debug/kzc_log.h>

#include <system/wrappers/kzs_math.h>
#include <system/wrappers/kzs_opengl.h>
#include <system/debug/kzs_log.h>
#include <system/display/kzs_surface.h>
#include <system/kzs_types.h>

kzsError bfInfoAddInteger(const struct KzcMemoryManager* memoryManager, const struct XMLNode* parent, kzString name, kzInt value)
{
    kzsError result;
    struct XMLNode* node;

    result = XMLNodeCreateInteger(memoryManager, name, value, &node);
    kzsErrorForward(result);
    result = XMLNodeAddChild(parent, node);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError bfInfoAddScalar(const struct KzcMemoryManager* memoryManager, const struct XMLNode* parent, kzString name, kzFloat value)
{
    kzsError result;
    struct XMLNode* node;
    result = XMLNodeCreateFloat(memoryManager, name, value, &node);
    kzsErrorForward(result);
    result = XMLNodeAddChild(parent, node);
    kzsErrorForward(result);
    kzsSuccess();
}

kzsError bfInfoUpdateSurface(const struct XMLNode* node, const struct KzsSurface* surface)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager;
    struct KzsSurfaceProperties surfaceProperties;

    kzsAssert(kzcIsValidPointer(node));

    memoryManager = kzcMemoryGetManager(node);

    /* Surface can be null in no-opengl version. */
    if(surface != KZ_NULL)
    {
        kzsSurfaceGetUsedSurfaceProperties(surface, &surfaceProperties);

        result = bfInfoAddInteger(memoryManager, node, "surface_red_bits", surfaceProperties.bitsColorR);
        kzsErrorForward(result);
        result = bfInfoAddInteger(memoryManager, node, "surface_green_bits", surfaceProperties.bitsColorG);
        kzsErrorForward(result);
        result = bfInfoAddInteger(memoryManager, node, "surface_blue_bits", surfaceProperties.bitsColorB);
        kzsErrorForward(result);
        result = bfInfoAddInteger(memoryManager, node, "surface_alpha_bits", surfaceProperties.bitsAlpha);
        kzsErrorForward(result);
        result = bfInfoAddInteger(memoryManager, node, "surface_depths_bits", surfaceProperties.bitsDepthBuffer);
        kzsErrorForward(result);
        result = bfInfoAddInteger(memoryManager, node, "surface_stencil_bits", surfaceProperties.bitsStencil);
        kzsErrorForward(result);
        if(surfaceProperties.antiAliasing < 0)
        {
            result = bfInfoAddInteger(memoryManager, node, "surface_multisample", surfaceProperties.antiAliasing);
            kzsErrorForward(result);
        }
    }
    else
    {
        struct XMLNode* child;
        memoryManager = kzcMemoryGetManager(node);
        result = XMLNodeCreateString(memoryManager, "Surface", "No rendering surface used.", &child);
        kzsErrorForward(result);
        result = XMLNodeAddChild(node, child);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError bfInfoUpdateGL(struct XMLNode* node)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager;
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    struct XMLNode* configurationNode;
    kzString vendorName; /**< OpenGL Vendor. */
    kzString rendererName; /**< Renderer name. */
    kzString versionNumber; /**< Version number. */
    kzString extensions; /**< Available extensions. */

    kzInt redBits; /**< Surface red bits. */
    kzInt greenBits; /**< Surface green bits. */
    kzInt blueBits; /**< Surface blue bits. */
    kzInt alphaBits; /**< Surface alpha bits. */
    kzInt depthBits; /**< Depth bits. */
    kzInt stencilBits; /**< Stencil bits. */
    /* TODO : get these. */
    /*kzInt fsaaSamples;*/ /**< Full screen anti alias samples 0 = no multisample. */
    /*kzInt currentSwapControl;*/ /**< Swap control 0 = disabled 1 = enabled */

    kzInt maxTextureSize; /**< Maximum texture size. */
    kzInt maxTextureUnits; /**< Maximum texture units. */

    kzInt maxViewportDimensions[2]; /**< Maximum viewport size. */

    kzInt compressedTextureFormatCount; /**< Amount of available texture compression formats. */
    kzInt* compressedTextureFormats; /**< Texture compression formats. */

    kzInt maxVertexAttributes; /**< Maximum vertex attributes. */
    kzInt maxVertexUniformVectors; /**< Maximum uniforms for vertex shader. */
    kzInt maxFragmentShaderUniformVectors; /**< Maximum uniforms for fragment shader. */
    kzInt maxVaryingVectors; /**< Maximum varying vectors. */
    kzInt maxVertexShaderTextureUnits; /**< Maximum texture image units that can be used to access textures maps in vertex shader. */

    kzInt maxRenderBufferSize; /**< Maximum renderbuffer size OpenGL can handle. */
    kzInt maxCubemapTextureSize; /**< Maximum cubemap texture size. */

    kzsAssert(kzcIsValidPointer(node));

    memoryManager = kzcMemoryGetManager(node);

    /* Get device information */
    vendorName = (kzString)(kzsGlGetString(KZS_GL_VENDOR)); 
    rendererName = (kzString)(kzsGlGetString(KZS_GL_RENDERER)); 
    versionNumber = (kzString)(kzsGlGetString(KZS_GL_VERSION)); 
    extensions = (kzString)(kzsGlGetString(KZS_GL_EXTENSIONS)); 


    {
        struct XMLNode* child;
        result = XMLNodeCreateString(memoryManager, "vendor", vendorName, &child);
        kzsErrorForward(result);
        result = XMLNodeAddChild(node, child);
        kzsErrorForward(result);
    }
    {
        struct XMLNode* child;
        result = XMLNodeCreateString(memoryManager, "version", versionNumber, &child);
        kzsErrorForward(result);
        result = XMLNodeAddChild(node, child);
        kzsErrorForward(result);
    }
    {
        struct XMLNode* child;
        result = XMLNodeCreateString(memoryManager, "renderer", rendererName, &child);
        kzsErrorForward(result);
        result = XMLNodeAddChild(node, child);
        kzsErrorForward(result);
    }
    {
        struct XMLNode* child;
        result = XMLNodeCreateString(memoryManager, "extensions", extensions, &child);
        kzsErrorForward(result);
        result = XMLNodeAddChild(node, child);
        kzsErrorForward(result);
    }

    {
        result = XMLNodeCreateContainer(memoryManager, "configuration", &configurationNode);
        kzsErrorForward(result);
        result = XMLNodeAddChild(node, configurationNode);
        kzsErrorForward(result);
    }

    /* Get surface bit info */
    kzsGlGetIntegerv(KZS_GL_RED_BITS, &redBits); 
    result = bfInfoAddInteger(memoryManager, configurationNode, "gl_red_bits", redBits);
    kzsErrorForward(result);
    kzsGlGetIntegerv(KZS_GL_GREEN_BITS, &greenBits); 
    result = bfInfoAddInteger(memoryManager, configurationNode, "gl_green_bits", greenBits);
    kzsErrorForward(result);
    kzsGlGetIntegerv(KZS_GL_BLUE_BITS, &blueBits); 
    result = bfInfoAddInteger(memoryManager, configurationNode, "gl_blue_bits", blueBits);
    kzsErrorForward(result);
    kzsGlGetIntegerv(KZS_GL_ALPHA_BITS, &alphaBits); 
    result = bfInfoAddInteger(memoryManager, configurationNode, "gl_alpha_bits", alphaBits);
    kzsErrorForward(result);
    kzsGlGetIntegerv(KZS_GL_DEPTH_BITS, &depthBits); 
    result = bfInfoAddInteger(memoryManager, configurationNode, "gl_depths_bits", depthBits);
    kzsErrorForward(result);
    kzsGlGetIntegerv(KZS_GL_STENCIL_BITS, &stencilBits); 
    result = bfInfoAddInteger(memoryManager, configurationNode, "gl_stencil_bits", stencilBits);
    kzsErrorForward(result);

    kzsGlGetIntegerv(KZS_GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    result = bfInfoAddInteger(memoryManager, configurationNode, "gl_max_texture_size", maxTextureSize);
    kzsErrorForward(result);
#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
    kzsGlGetIntegerv(KZS_GL_MAX_TEXTURE_UNITS, &maxTextureUnits);
#else
    kzsGlGetIntegerv(KZS_GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
#endif
    result = bfInfoAddInteger(memoryManager, configurationNode, "gl_max_texture_units", maxTextureUnits);
    kzsErrorForward(result);
    
    kzsGlGetIntegerv(KZS_GL_MAX_VIEWPORT_DIMS, &maxViewportDimensions[0]);
    {
        struct XMLNode* viewport;
        result = XMLNodeCreateContainer(memoryManager, "gl_max_viewport_dims", &viewport);
        kzsErrorForward(result);
        result = XMLNodeAddChild(configurationNode, viewport);
        kzsErrorForward(result);
        result = bfInfoAddInteger(memoryManager, viewport, "width", maxViewportDimensions[0]);
        kzsErrorForward(result);
        result = bfInfoAddInteger(memoryManager, viewport, "height", maxViewportDimensions[1]);
        kzsErrorForward(result);
    }


    kzsGlGetIntegerv(KZS_GL_NUM_COMPRESSED_TEXTURE_FORMATS, &compressedTextureFormatCount);
    {
        struct XMLNode* compressedTextureFormatsContainer;
        result = XMLNodeCreateContainer(memoryManager, "gl_num_compressed_texture_formats", &compressedTextureFormatsContainer);
        kzsErrorForward(result);
        result = XMLNodeAddChild(configurationNode, compressedTextureFormatsContainer);
        kzsErrorForward(result);

        {
            struct XMLAttribute* attribute;
            result = XMLAttributeCreateInteger(memoryManager, "count", compressedTextureFormatCount, &attribute);
            kzsErrorForward(result);
            result = XMLNodeAddAttribute(compressedTextureFormatsContainer, attribute);
            kzsErrorForward(result);
        }

        if(compressedTextureFormatCount > 0)
        {
            kzInt i;
            result = kzcMemoryAllocArray(memoryManager, compressedTextureFormats, (kzUint)compressedTextureFormatCount, "Compressed texture formats array.");
            kzsErrorForward(result);
            kzsGlGetIntegerv(KZS_GL_COMPRESSED_TEXTURE_FORMATS, &compressedTextureFormats[0]);
            for(i = 0; i < compressedTextureFormatCount; ++i)
            {
                result = bfInfoAddInteger(memoryManager, compressedTextureFormatsContainer, "format", compressedTextureFormats[i]);
                kzsErrorForward(result);
            }
        }
    }

    maxFragmentShaderUniformVectors = 0;
    maxVertexUniformVectors = 0;
    maxVaryingVectors = 0;
#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_2_0
    kzsGlGetIntegerv(KZS_GL_MAX_VERTEX_UNIFORM_VECTORS, &maxVertexUniformVectors);
    result = bfInfoAddInteger(memoryManager, configurationNode, "gl_max_vertex_uniform_vectors", maxVertexUniformVectors);
    kzsErrorForward(result);
    kzsGlGetIntegerv(KZS_GL_MAX_FRAGMENT_UNIFORM_VECTORS, &maxFragmentShaderUniformVectors);    
    result = bfInfoAddInteger(memoryManager, configurationNode, "gl_max_fragment_uniform_vectors", maxFragmentShaderUniformVectors);
    kzsErrorForward(result);
    kzsGlGetIntegerv(KZS_GL_MAX_VARYING_VECTORS, &maxVaryingVectors);
    result = bfInfoAddInteger(memoryManager, configurationNode, "gl_max_varying_vectors", maxVaryingVectors);
    kzsErrorForward(result);
#endif
    kzsGlGetIntegerv(KZS_GL_MAX_VERTEX_ATTRIBS, &maxVertexAttributes);
    result = bfInfoAddInteger(memoryManager, configurationNode, "gl_max_vertex_attribs", maxVertexAttributes);
    kzsErrorForward(result);
    kzsGlGetIntegerv(KZS_GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &maxVertexShaderTextureUnits);
    result = bfInfoAddInteger(memoryManager, configurationNode, "gl_max_vertex_texture_image_units", maxVertexShaderTextureUnits);
    kzsErrorForward(result);
    
    kzsGlGetIntegerv(KZS_GL_MAX_RENDERBUFFER_SIZE, &maxRenderBufferSize);
    result = bfInfoAddInteger(memoryManager, configurationNode, "gl_max_renderbuffer_size", maxRenderBufferSize);
    kzsErrorForward(result);
    kzsGlGetIntegerv(KZS_GL_MAX_CUBE_MAP_TEXTURE_SIZE, &maxCubemapTextureSize);
    result = bfInfoAddInteger(memoryManager, configurationNode, "gl_max_cube_map_texture_size", maxCubemapTextureSize);
    kzsErrorForward(result);



    /* Print info to log. */
    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "");
    kzsErrorForward(result);
    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "OpenGL info: ");
    kzsErrorForward(result);
    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, vendorName);
    kzsErrorForward(result);
    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, rendererName);
    kzsErrorForward(result);
    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, versionNumber);
    kzsErrorForward(result);

    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Buffer R/G/B/A S/D sizes:");
    kzsErrorForward(result);
    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "%d/%d/%d/%d %d/%d", redBits, greenBits, blueBits, alphaBits, stencilBits, depthBits);
    kzsErrorForward(result);

    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Max vertex attribs: %d", maxVertexAttributes);
    kzsErrorForward(result);
    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Max vertex uniform vectors: %d", maxVertexUniformVectors);
    kzsErrorForward(result);
    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Max varying vectors: %d", maxVaryingVectors);
    kzsErrorForward(result);
    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Max vertex texture image units: %d", maxVertexShaderTextureUnits);
    kzsErrorForward(result);
    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Max fragment uniform vectors: %d", maxFragmentShaderUniformVectors);
    kzsErrorForward(result);

    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Max renderbuffer size: %d", maxRenderBufferSize);
    kzsErrorForward(result);
    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Max cube map texture size: %d", maxCubemapTextureSize);
    kzsErrorForward(result);

    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Max viewport dimensions: %d %d", maxViewportDimensions[0], maxViewportDimensions[1]);
    kzsErrorForward(result);

    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Max texture size: %d", maxTextureSize);
    kzsErrorForward(result);
    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Max texture units: %d", maxTextureUnits);
    kzsErrorForward(result);

    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Compressed texture formats: %d", compressedTextureFormatCount);
    kzsErrorForward(result);
    {
        kzInt n = 0; 
        for(; n < compressedTextureFormatCount; ++n)
        {
            result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, " Format %d: %d", n, compressedTextureFormats[n]);
            kzsErrorForward(result);
        }
    }
    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "");
    kzsErrorForward(result);


    if(compressedTextureFormatCount > 0)
    {
        result = kzcMemoryFreeArray(compressedTextureFormats);
        kzsErrorForward(result);
    }
#else
    {
        struct XMLNode* child;
        memoryManager = kzcMemoryGetManager(node);
        result = XMLNodeCreateString(memoryManager, "OpenGL", "No OpenGL used.", &child);
        kzsErrorForward(result);
        result = XMLNodeAddChild(node, child);
        kzsErrorForward(result);
    }
#endif
    kzsSuccess();
}

kzsError bfInfoUpdateCL(struct XMLNode* node, struct CluInfo* cluInfo, struct XMLNode* profileNode)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager;
    kzsAssert(kzcIsValidPointer(node));
    kzsAssert(kzcIsValidPointer(cluInfo));

    memoryManager = kzcMemoryGetManager(node);

    /* Add info about selected platform and device. */
    {
        struct XMLNode* platformNode;
        struct XMLNode* deviceNode;
        result = XMLNodeCreateInteger(memoryManager, "selectedPlatform", cluInfo->usedPlatform, &platformNode);
        kzsErrorForward(result);
        result = XMLNodeCreateInteger(memoryManager, "selectedDevice", cluInfo->usedDevice, &deviceNode);
        kzsErrorForward(result);
        result = XMLNodeAddChild(node, platformNode);
        kzsErrorForward(result);
        result = XMLNodeAddChild(node, deviceNode);
        kzsErrorForward(result);
    }

    {
        struct XMLNode* platformsContainer;
        result = XMLNodeCreateContainer(memoryManager, "platforms", &platformsContainer);
        kzsErrorForward(result);
        result = XMLNodeAddChild(node, platformsContainer);
        kzsErrorForward(result);

        {
            struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(cluInfo->platforms);
            while(kzcDynamicArrayIterate(it))
            {
                struct CluPlatformInfo* info = kzcDynamicArrayIteratorGetValue(it);

                struct XMLNode* platformContainer;
                struct XMLAttribute* attribute;
                result = XMLNodeCreateContainer(memoryManager, "platform", &platformContainer);
                kzsErrorForward(result);
                result = XMLNodeAddChild(platformsContainer, platformContainer);
                kzsErrorForward(result);
                result = XMLAttributeCreateInteger(memoryManager, "id", info->id, &attribute);
                kzsErrorForward(result);
                result = XMLNodeAddAttribute(platformContainer, attribute);
                kzsErrorForward(result);


                {
                    struct XMLNode* child;
                    result = XMLNodeCreateString(memoryManager, "name", info->name, &child);
                    kzsErrorForward(result);
                    result = XMLNodeAddChild(platformContainer, child);
                    kzsErrorForward(result);
                }
                {
                    struct XMLNode* child;
                    result = XMLNodeCreateString(memoryManager, "version", info->version, &child);
                    kzsErrorForward(result);
                    result = XMLNodeAddChild(platformContainer, child);
                    kzsErrorForward(result);
                }
                {
                    struct XMLNode* child;
                    result = XMLNodeCreateString(memoryManager, "vendor", info->vendor, &child);
                    kzsErrorForward(result);
                    result = XMLNodeAddChild(platformContainer, child);
                    kzsErrorForward(result);
                }
                {
                    struct XMLNode* child;
                    result = XMLNodeCreateString(memoryManager, "profile", info->profile, &child);
                    kzsErrorForward(result);
                    result = XMLNodeAddChild(platformContainer, child);
                    kzsErrorForward(result);
                    
                    result = XMLNodeSetString(profileNode, info->profile);
                    kzsErrorForward(result);
                }

                {
                    struct XMLNode* devicesContainer;
                    result = XMLNodeCreateContainer(memoryManager, "devices", &devicesContainer);
                    kzsErrorForward(result);
                    result = XMLNodeAddChild(platformContainer, devicesContainer);
                    kzsErrorForward(result);

                    /* Devices. */
                    {
                        struct KzcDynamicArrayIterator itdevices = kzcDynamicArrayGetIterator(info->devices);
                        while(kzcDynamicArrayIterate(itdevices))
                        {
                            struct CluDeviceInfo* device = kzcDynamicArrayIteratorGetValue(itdevices);

                            struct XMLNode* deviceContainer;
                            struct XMLAttribute* deviceAttribute;
                            result = XMLNodeCreateContainer(memoryManager, "device", &deviceContainer);
                            kzsErrorForward(result);
                            result = XMLNodeAddChild(devicesContainer, deviceContainer);
                            kzsErrorForward(result);
                            result = XMLAttributeCreateInteger(memoryManager, "id", device->id, &deviceAttribute);
                            kzsErrorForward(result);
                            result = XMLNodeAddAttribute(deviceContainer, deviceAttribute);
                            kzsErrorForward(result);

                            {
                                struct XMLNode* child;
                                result = XMLNodeCreateString(memoryManager, "name", device->name, &child);
                                kzsErrorForward(result);
                                result = XMLNodeAddChild(deviceContainer, child);
                                kzsErrorForward(result);
                            }
                            {
                                struct XMLNode* child;
                                result = XMLNodeCreateString(memoryManager, "profile", device->deviceType, &child);
                                kzsErrorForward(result);
                                result = XMLNodeAddChild(deviceContainer, child);
                                kzsErrorForward(result);
                            }
                            {
                                struct XMLNode* child;
                                result = XMLNodeCreateString(memoryManager, "driverVersion", device->driverVersion, &child);
                                kzsErrorForward(result);
                                result = XMLNodeAddChild(deviceContainer, child);
                                kzsErrorForward(result);
                            }
                            {
                                struct XMLNode* child;
                                result = XMLNodeCreateString(memoryManager, "vendor", device->vendor, &child);
                                kzsErrorForward(result);
                                result = XMLNodeAddChild(deviceContainer, child);
                                kzsErrorForward(result);
                            }
                            {
                                struct XMLNode* child;
                                result = XMLNodeCreateString(memoryManager, "vendorId", device->vendorId, &child);
                                kzsErrorForward(result);
                                result = XMLNodeAddChild(deviceContainer, child);
                                kzsErrorForward(result);
                            }

                            {
                                struct XMLNode* settingsContainer;
                                result = XMLNodeCreateContainer(memoryManager, "deviceInfo", &settingsContainer);
                                kzsErrorForward(result);
                                result = XMLNodeAddChild(deviceContainer, settingsContainer);
                                kzsErrorForward(result);

                                {
                                    struct KzcHashMapIterator it2 = kzcHashMapGetIterator(device->deviceConfig);
                                    while(kzcHashMapIterate(it2))
                                    {
                                        kzString key = (kzString)kzcHashMapIteratorGetKey(it2);
                                        kzString value = (kzString)kzcHashMapIteratorGetValue(it2);

                                        {
                                            struct XMLNode* child;
                                            result = XMLNodeCreateString(memoryManager, key, value, &child);
                                            kzsErrorForward(result);
                                            result = XMLNodeAddChild(settingsContainer, child);
                                            kzsErrorForward(result);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    kzsSuccess();
}

static kzsError bfInfoUpdateConfigurationNode_internal(struct KzcSettingNode* node, struct XMLNode* xmlNode)
{
    kzsError result;

    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(node);
    struct KzcHashMap* settingDictionary = kzcSettingNodeGetDictionary(node);

    /* TODO: As of now all crashes here have the low 4 bytes of settingDictionary as 0, indicating that the bug lies in the union of ints and pointers. */
    if((kzInt)settingDictionary != 0)
    {
        struct KzcHashMapIterator it = kzcHashMapGetIterator(settingDictionary);

        while(kzcHashMapIterate(it))
        {
            struct KzcSettingNode* child = kzcHashMapIteratorGetValue(it);
            kzString name = (kzString)kzcHashMapIteratorGetKey(it);
            kzMutableString valueString;
            enum KzcSettingType type;

            type = kzcSettingNodeGetType(node, name);

            if(type != KZC_SETTING_SUBNODE)
            {
                kzInt intValue;
                kzFloat floatValue;
                if(type == KZC_SETTING_STRING)
                {
                    kzString outString;
                    if(kzcSettingNodeGetString(node, name, &outString))
                    {
                       result = kzcStringCopy(memoryManager, outString, &valueString);
                       kzsErrorForward(result);
                    }
                    else
                    {
                        valueString = KZ_NULL;
                        kzsAssert(KZ_FALSE);
                    }
                }
                else
                if(type == KZC_SETTING_INTEGER)
                {
                    if(kzcSettingNodeGetInteger(node, name, &intValue))
                    {
                        result = kzcStringFormat(memoryManager, "%d", &valueString, intValue);
                        kzsErrorForward(result);
                    }
                    else
                    {
                        valueString = KZ_NULL;
                        kzsAssert(KZ_FALSE);
                    }
                }
                else
                if(type == KZC_SETTING_FLOAT)
                {
                    if(kzcSettingNodeGetFloat(node, name, &floatValue))
                    {
                        result = kzcStringFormat(memoryManager, "%f", &valueString, floatValue);
                        kzsErrorForward(result);
                    }
                    else
                    {
                        valueString = KZ_NULL;
                        kzsAssert(KZ_FALSE);
                    }
                }
                else
                {
                    valueString = KZ_NULL;
                }

                {
                    struct XMLNode* newChild;
                    result = XMLNodeCreateString(memoryManager, name, valueString, &newChild);
                    kzsErrorForward(result);
                    result = XMLNodeAddChild(xmlNode, newChild);
                    kzsErrorForward(result);
                }


                if(type != KZC_SETTING_STRING)
                {
                    result = bfInfoUpdateConfigurationNode_internal(child, xmlNode);
                    kzsErrorForward(result);
                }

                result = kzcStringDelete(valueString);
                kzsErrorForward(result); 
            }
        }
    }

    kzsSuccess();
}

kzsError bfInfoUpdateConfiguration(struct XMLNode* node, struct KzcSettingContainer* settingContainer)
{
    kzsError result;
    struct KzcSettingNode* root;
    
    kzsAssert(kzcIsValidPointer(node));
    kzsAssert(kzcIsValidPointer(settingContainer));

    root = kzcSettingContainerGetRoot(settingContainer);
    result = bfInfoUpdateConfigurationNode_internal(root, node);
    kzsErrorForward(result);

    kzsSuccess();
}


static void bfInfoSwap_internal(kzUint *a, kzUint *b)
{
    kzUint t=*a; 
    *a=*b; 
    *b=t;
}

static void bfInfoSort_internal(kzUint* array, kzUint start, kzUint end)
{
    if (end > start + 1)
    {
        kzUint piv = array[start];
        kzUint l = start + 1;
        kzUint r = end;
        while(l < r)
        {
            if(array[l] <= piv)
            {
                l++;
            }
            else
            {
                bfInfoSwap_internal(&array[l], &array[--r]);
            }
        }
        bfInfoSwap_internal(&array[--l], &array[start]);
        bfInfoSort_internal(array, start, l);
        bfInfoSort_internal(array, r, end);
    }
}

static kzFloat bfInfoCalculateMedian_internal(kzUint* frameTimes, kzUint frameCount) 
{
    kzFloat returnValue;
    kzUint count = frameCount;
    kzUint* values = frameTimes;

    bfInfoSort_internal(values, 0, count);

    if(count % 2 == 0) 
    {
        returnValue = ((values[count / 2] + values[count / 2 - 1]) / 2.0f);
    }
    else 
    {
        returnValue = (kzFloat)values[count / 2];
    }

    return returnValue;
}


kzsError bfInfoUpdateSceneResults(struct XMLNode* node, struct BfReportLogger* logger, kzString sceneName, kzString sceneCategory, kzBool validData, 
                                  kzBool binaryKernel, kzFloat scoreWeightFactor)
{
    kzsError result;
    kzUint i;
    kzUint amount = 0;
    kzUint highest = KZ_UINT_MINIMUM;
    kzUint smallest = KZ_UINT_MAXIMUM;
    kzFloat geometricMean = 1.0f;
    kzFloat medianValue;
    kzUint* frameTimes = bfReportLoggerGetFrameTimesArray(logger);
    kzUint frameCount = bfReportLoggerGetFrameCount(logger);
    kzFloat geometricMeanExponent = 1.0f / (kzFloat)frameCount;
    kzFloat average;
    kzFloat score = 0.0f;
    kzMutableString frameTimeString;
    struct KzcStringBuffer* stringBuffer;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(node);

    kzsAssert(frameTimes != KZ_NULL);

    result = kzcStringBufferCreate(memoryManager, frameCount * 5, &stringBuffer);
    kzsErrorForward(result);
    for(i = 0; i < frameCount - 1; ++i)
    {
        result = kzcStringBufferAppendFormat(stringBuffer, "%u, ", frameTimes[i]);
        kzsErrorForward(result);
    }
    result = kzcStringBufferAppendFormat(stringBuffer, "%u", frameTimes[frameCount - 1]);
    kzsErrorForward(result);
    result = kzcStringBufferToString(memoryManager, stringBuffer, &frameTimeString);
    kzsErrorForward(result);


    for(i = 0; i < frameCount; ++i)
    {
        kzUint frameTime = frameTimes[i];
        highest = kzsMaxU(frameTime, highest);
        smallest = kzsMinU(frameTime, smallest);
        amount += frameTime;
        geometricMean *= kzsPowf((kzFloat)frameTime, geometricMeanExponent);
    }

    medianValue = bfInfoCalculateMedian_internal(frameTimes, frameCount);
    average = amount / (kzFloat)(frameCount);
    
    if(frameCount > 0 && geometricMean > 0.0f)
    {
        score = kzsPowf(geometricMean, -1.0f) * 1000000.0f;
    }

    kzcLogDebug("Score  %f (us)", score);
    kzcLogDebug("Average duration of frame %f (us)", average);
    kzcLogDebug("Geometric mean of frame duration %f (us)", geometricMean);
    kzcLogDebug("Median %f (us)", medianValue);
    kzcLogDebug("Fastest frame %u (us)", smallest);
    kzcLogDebug("Slowest frame %u (us)", highest);
    kzcLogDebug("Binary kernel %u", binaryKernel);

    {
        struct XMLNode* testNode;
        struct XMLAttribute* nameAttribute;
        struct XMLAttribute* categoryAttribute;
        struct XMLAttribute* scoreAttribute;
        struct XMLAttribute* validAttribute;

        result = XMLNodeCreateContainer(memoryManager, "test", &testNode);
        kzsErrorForward(result);
        result = XMLNodeAddChild(node, testNode);
        kzsErrorForward(result);
        result = XMLAttributeCreateString(memoryManager, "name", sceneName, &nameAttribute);
        kzsErrorForward(result);
        result = XMLNodeAddAttribute(testNode, nameAttribute);
        kzsErrorForward(result);
        result = XMLAttributeCreateString(memoryManager, "category", sceneCategory, &categoryAttribute);
        kzsErrorForward(result);
        result = XMLNodeAddAttribute(testNode, categoryAttribute);
        kzsErrorForward(result);
        result = XMLAttributeCreateInteger(memoryManager, "isPartOfOverallScore", bfReportLoggerIsPartOfOverallScore(logger)?1:0, &scoreAttribute);
        kzsErrorForward(result);
        result = XMLNodeAddAttribute(testNode, scoreAttribute);
        kzsErrorForward(result);
        result = bfInfoAddScalar(memoryManager, testNode, "score", score);
        kzsErrorForward(result);
        result = XMLAttributeCreateInteger(memoryManager, "validData", validData?1:0, &validAttribute);
        kzsErrorForward(result);
        result = XMLNodeAddAttribute(testNode, validAttribute);
        kzsErrorForward(result);

        result = bfInfoAddInteger(memoryManager, testNode, "binaryKernels", binaryKernel?1:0);
        kzsErrorForward(result); 
        result = bfInfoAddScalar(memoryManager, testNode, "scoreScaleFactor", scoreWeightFactor);
        kzsErrorForward(result);
        result = bfInfoAddInteger(memoryManager, testNode, "slowest", highest);
        kzsErrorForward(result);
        result = bfInfoAddInteger(memoryManager, testNode, "fastest", smallest);
        kzsErrorForward(result); 
        result = bfInfoAddScalar(memoryManager, testNode, "average", average);
        kzsErrorForward(result);
        result = bfInfoAddScalar(memoryManager, testNode, "median", medianValue);
        kzsErrorForward(result);
        result = bfInfoAddScalar(memoryManager, testNode, "geometricMean", geometricMean);
        kzsErrorForward(result);

        {
            {
                struct XMLNode* frameTimeNode;
                result = XMLNodeCreateString(memoryManager, "frameTimes", frameTimeString, &frameTimeNode);
                kzsErrorForward(result);
                result = XMLNodeAddChild(testNode, frameTimeNode);
                kzsErrorForward(result);
            }
        }
    }

    result = kzcStringBufferDelete(stringBuffer);
    kzsErrorForward(result);
    result = kzcStringDelete(frameTimeString);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError bfInfoUpdateOverallScore(const struct XMLDocument* xmlDocument, struct XMLNode* scoreNode, struct XMLNode* testsParentNode)
{
    kzsError result;
    kzBool fullProfile;
    kzUint i;
    kzBool allTestsHaveValidConfiguration = KZ_TRUE;
    kzBool allTestsHaveOnlineKernels = KZ_TRUE;
    kzBool validTestSuite = KZ_FALSE;
    struct XMLNode* overallScoreNode;
    struct XMLNode* categoryScoreNode[4];
    kzString benchmarkScoreNodeName = "score";
    kzString scoreNodeNames[4] = {"physicsTestScore", "generalTestScore", "imageTestScore", "videoTestScore"};
    kzFloat categoryGeometricMean[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    kzFloat scoreDivider = 0.0f;
    kzFloat scoreValue = 0.0;
    kzFloat finalScore;
    for(i = 0; i < 4; ++i)
    {
        result = XMLNodeGetChildNode(scoreNode, scoreNodeNames[i], &categoryScoreNode[i]);
        kzsErrorForward(result);
        if(categoryScoreNode[i] == KZ_NULL)
        {
            result = XMLNodeCreateFloat(kzcMemoryGetManager(scoreNode), scoreNodeNames[i], 0.0f, &categoryScoreNode[i]);
            kzsErrorForward(result);
            result = XMLNodeAddChild(scoreNode, categoryScoreNode[i]);
            kzsErrorForward(result);
        }
    }
   
    {
        kzUint count = 0;
        kzUint categoryTestCount[4] = {0, 0, 0, 0};
        kzString categoryNames[4] = {"Physics", "General", "Image manipulation", "Video manipulation"};
        kzFloat categoryScoreExponent[4];
        struct KzcDynamicArrayIterator it;
        result = XMLNodeGetChildIterator(testsParentNode, &it);
        kzsErrorForward(result);
        while(kzcDynamicArrayIterate(it))
        {
            struct XMLNode* node = kzcDynamicArrayIteratorGetValue(it);
            struct XMLNode* binaryKernelNode;
            struct XMLAttribute* attribute;
            result = XMLNodeGetAttributeByName(node, "isPartOfOverallScore", &attribute);
            kzsErrorForward(result);
            if(XMLAttributeGetType(attribute) == XML_ATTRIBUTE_TYPE_INTEGER && XMLAttributeGetInteger(attribute) > 0)
            {
                count++;

                result = XMLNodeGetAttributeByName(node, "category", &attribute);
                kzsErrorForward(result);
                for(i = 0; i < 4; ++i)
                {
                    if(XMLAttributeGetType(attribute) == XML_ATTRIBUTE_TYPE_STRING && kzcStringCompare(XMLAttributeGetString(attribute), categoryNames[i]) == 0)
                    {
                        categoryTestCount[i]++;
                    }
                }
            }

            result = XMLNodeGetAttributeByName(node, "validData", &attribute);
            kzsErrorForward(result);
            if(XMLAttributeGetType(attribute) == XML_ATTRIBUTE_TYPE_INTEGER && XMLAttributeGetInteger(attribute) == 0)
            {
                allTestsHaveValidConfiguration = KZ_FALSE;
            }

            result = XMLNodeGetChildNode(node, "binaryKernels", &binaryKernelNode);
            kzsErrorForward(result);
            if(XMLNodeGetType(binaryKernelNode) == XML_NODE_TYPE_INTEGER && XMLNodeGetInteger(binaryKernelNode) != 0)
            {
                allTestsHaveOnlineKernels = KZ_FALSE;
            }            
        }
        for(i = 0; i < 4; ++i)
        {
            categoryScoreExponent[i] = 1.0f / (kzFloat)categoryTestCount[i];
        }

        result = XMLNodeGetChildIterator(testsParentNode, &it);
        kzsErrorForward(result);
        while(kzcDynamicArrayIterate(it))
        {
            struct XMLNode* node = kzcDynamicArrayIteratorGetValue(it);

            struct XMLAttribute* attribute;
            result = XMLNodeGetAttributeByName(node, "isPartOfOverallScore", &attribute);
            kzsErrorForward(result);
            if(XMLAttributeGetType(attribute) == XML_ATTRIBUTE_TYPE_INTEGER && XMLAttributeGetInteger(attribute) > 0)
            {
                kzFloat testScore;
                kzFloat scoreScale;
                struct XMLNode* testScoreNode;
                struct XMLNode* scoreScaleNode;
                result = XMLNodeGetChildNode(node, "score", &testScoreNode);
                kzsErrorForward(result);
                testScore = XMLNodeGetFloat(testScoreNode);
                result = XMLNodeGetChildNode(node, "scoreScaleFactor", &scoreScaleNode);
                kzsErrorForward(result);
                scoreScale = XMLNodeGetFloat(scoreScaleNode);

                scoreValue += scoreScale * kzsLogEf(testScore);
                scoreDivider += scoreScale;

                result = XMLNodeGetAttributeByName(node, "category", &attribute);
                kzsErrorForward(result);
                for(i = 0; i < 4; ++i)
                {
                    if(XMLAttributeGetType(attribute) == XML_ATTRIBUTE_TYPE_STRING && kzcStringCompare(XMLAttributeGetString(attribute), categoryNames[i]) == 0)
                    {
                        categoryGeometricMean[i] *= (kzFloat)testScore;
                    }
                }
            }
        }

        /* If no tests for scoring were found set score to zero. */
        if(count == 0)
        {
            finalScore = 0.0f;
        }
        else
        {
            finalScore = kzsExp(scoreValue / scoreDivider);
        }
        for(i = 0; i < 4; ++i)
        {
            if(categoryTestCount[i] == 0 || categoryGeometricMean[i] <= 0.0f)
            {
                categoryGeometricMean[i] = 0.0f;
            }
            else
            {
                categoryGeometricMean[i] = kzsPowf(categoryGeometricMean[i], categoryScoreExponent[i]);
            }
        }
    }

    /* Fetch info if full or embedded profile. */
    {
        struct XMLNode* node;
        result = XMLDocumentGetNode(xmlDocument, "report/benchmark/settings/OpenCLFullProfile", &node);
        kzsErrorForward(result);
        fullProfile = kzcStringCompare(XMLNodeGetString(node), "1") == 0;
    }


    /* Validate test suite. */
    {
        kzUint n;
        struct KzcDynamicArrayIterator it;
        #define TEST_COUNT_FULL_PROFILE 16
        const kzUint testCountEmbeddedProfile = 14;
        kzUint testsRun = 0;
        kzUint testRunCount[TEST_COUNT_FULL_PROFILE] = 
        {
            0
        };
        kzString testNames[] = 
        {
            "Julia Rendering Test",
            "Mandelbulb Rendering Test",
            "Fluid Operations Test",
            "Soft Body Test",
            "Smoothed Particle Hydrodynamics Test",
            "Wave Simulation Test",
            "Image Sharpening Test",
            "Image Blur Test",
            "Image Surface Smoothing Test",
            "Image Noise Reduction Test",
            "Video Sharpening Test",
            "Video Blur Test",
            "Video Surface Smoothing Test",
            "Video Noise Reduction Test",
            "Image Color Correction Test",
            "Video Color Correction Test"
        };
        
        result = XMLNodeGetChildIterator(testsParentNode, &it);
        kzsErrorForward(result);

        validTestSuite = KZ_TRUE;

        while(kzcDynamicArrayIterate(it))
        {
            struct XMLNode* node = kzcDynamicArrayIteratorGetValue(it);
            struct XMLAttribute* attribute;
            result = XMLNodeGetAttributeByName(node, "isPartOfOverallScore", &attribute);
            kzsErrorForward(result);
            if(XMLAttributeGetType(attribute) == XML_ATTRIBUTE_TYPE_INTEGER && XMLAttributeGetInteger(attribute) > 0)
            {
                struct XMLAttribute* nameAttribute;
                result = XMLNodeGetAttributeByName(node, "name", &nameAttribute);
                kzsErrorForward(result);
                for(n = 0; n < TEST_COUNT_FULL_PROFILE; ++n)
                {
                    if(kzcStringCompare(XMLAttributeGetString(nameAttribute), testNames[n]) == 0)
                    {
                        testRunCount[n]++;
                    }
                }
                testsRun++;
            }
        }

        for(n = 0; n < (fullProfile ? TEST_COUNT_FULL_PROFILE : testCountEmbeddedProfile); ++n)
        {
            validTestSuite &= (kzBool)(testRunCount[n] == 1);
        }

        validTestSuite &= (kzBool)(testsRun == (fullProfile ? TEST_COUNT_FULL_PROFILE : testCountEmbeddedProfile));
    }

    result = XMLNodeGetChildNode(scoreNode, benchmarkScoreNodeName, &overallScoreNode);
    kzsErrorForward(result);
    if(overallScoreNode == KZ_NULL)
    {
        result = XMLNodeCreateFloat(kzcMemoryGetManager(scoreNode), benchmarkScoreNodeName, 0.0f, &overallScoreNode);
        kzsErrorForward(result);
        result = XMLNodeAddChild(scoreNode, overallScoreNode);
        kzsErrorForward(result);
    }

    XMLNodeSetFloat(overallScoreNode, finalScore);
    for(i = 0; i < 4; ++i)
    {
        XMLNodeSetFloat(categoryScoreNode[i], categoryGeometricMean[i]);
    }

    {
        struct XMLNode* validNode;
        result = XMLDocumentGetNode(xmlDocument, "xml/validData", &validNode);
        kzsErrorForward(result);
        XMLNodeSetInteger(validNode, allTestsHaveValidConfiguration ? 1 : 0);
    }
    {
        struct XMLNode* validNode;
        result = XMLDocumentGetNode(xmlDocument, "xml/validTestSuite", &validNode);
        kzsErrorForward(result);
        XMLNodeSetInteger(validNode, validTestSuite ? 1 : 0);
    }
    {
        struct XMLNode* validNode;
        result = XMLDocumentGetNode(xmlDocument, "xml/binaryKernels", &validNode);
        kzsErrorForward(result);
        XMLNodeSetInteger(validNode, allTestsHaveOnlineKernels? 0 : 1);
    }

    kzsSuccess();
}
