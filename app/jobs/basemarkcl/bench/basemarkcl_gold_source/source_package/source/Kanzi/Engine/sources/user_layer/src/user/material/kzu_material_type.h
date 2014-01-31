/**
* \file
* Specifies material type. Material type is set of property types that
* define material (e.g. textured, 2 lights).
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_MATERIAL_TYPE_H
#define KZU_MATERIAL_TYPE_H


#include <core/resource_manager/kzc_resource_memory_type.h>

#include <system/wrappers/kzs_opengl_base.h>
#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzuLightPropertyType;
struct KzuProperty;
struct KzuPropertyType;
struct KzuPropertyTypeCollection;
struct KzcMemoryManager;
struct KzcResourceManager;
struct KzcHashMap;
struct KzcDynamicArray;
struct KzcShader;


/** Shader info for material type (no shader / source shader / binary shader). */
struct KzuMaterialTypeShaderInfo
{
    /** Type of the shader. */
    enum KzuShaderType
    {
        KZU_SHADER_TYPE_NONE,                   /**< No shader. */
        KZU_SHADER_TYPE_SOURCE,                 /**< Source shader. */
        KZU_SHADER_TYPE_BINARY_SEPARATED,       /**< Separated binary format (separate vertex+fragment). */
        KZU_SHADER_TYPE_BINARY_COMBINED         /**< Combined binary format (1 binary for shader program). */
    } type;                                     /**< Type of the shader. */

    union
    {
        struct
        {
            kzUint vertexDataSize;              /**< Vertex binary size in bytes. */
            kzUint fragmentDataSize;            /**< Fragment binary size in bytes. */
            void* vertexData;                   /**< Binary code for vertex shader. */
            void* fragmentData;                 /**< Binary code for fragment shader. */
            struct KzcDynamicArray* uniforms;   /**< Uniforms. */
        } binarySeparated;                      /**< Binary separated data. */
        struct
        {
            kzUint size;                        /**< Binary size in bytes. */
            void* data;                         /**< Binary code. */
            struct KzcDynamicArray* uniforms;   /**< Uniforms. */
        } binaryCombined;                       /**< Binary combined data. */
        struct
        {
            kzString vertex;                    /**< Vertex shader. */
            kzString fragment;                  /**< Fragment shader. */
            struct KzcDynamicArray* uniforms;   /**< List of uniforms. */
        } code;                                 /**< Code data. */
    } shaderData;                               /**< Shader data definition. */

    struct KzcDynamicArray* vertexAttributes;   /**< Shader attributes, list of strings. */
};

/**
 * \struct KzuMaterialType
 * Structure for material type. Material type is set of properties that are used for final output.
 */
struct KzuMaterialType;


/** Creates new material type. */
kzsError kzuMaterialTypeCreate(const struct KzcMemoryManager* memoryManager, struct KzuMaterialType** out_materialType);
/** Deletes material type. */
kzsError kzuMaterialTypeDelete(struct KzuMaterialType* materialType);

/**
 * Takes ownership of the material type container deletion. After this, when a material type is deleted,
 * only the data is actually deleted and the container remains alive.
 */
void kzuMaterialTypeTakeOwnership(const struct KzuMaterialType* materialType);
/** Checks if the material type is valid (pointer and the data are non-null). */
kzBool kzuMaterialTypeIsValid(const struct KzuMaterialType* materialType);
/** Moves the data from source material type to the target material type. */
void kzuMaterialTypeTransferData(struct KzuMaterialType* targetMaterialType, struct KzuMaterialType* sourceMaterialType);

/** Assigns properties for material type. */
kzsError kzuMaterialTypeAddPropertyType(const struct KzuMaterialType* materialType, struct KzuPropertyType* propertyType);
/** Assigns lights for material type. */
kzsError kzuMaterialTypeAddLightPropertyType(const struct KzuMaterialType* materialType, const struct KzuLightPropertyType* propertyType);

/** Returns property type collection from material type. */
struct KzuPropertyTypeCollection* kzuMaterialTypeGetPropertyTypeCollection(const struct KzuMaterialType* materialType);

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
/** Gets shader program from material type. */
struct KzcShader* kzuMaterialTypeGetShaderProgram(const struct KzuMaterialType* materialType);
#endif

/** Sets shader for material type. */
kzsError kzuMaterialTypeSetShader(const struct KzuMaterialType* materialType, struct KzcResourceManager* resourceManager,
                                  enum KzcResourceMemoryType memoryType, const struct KzuMaterialTypeShaderInfo* shaderInfo, 
                                  kzBool* out_shaderValid);
/** Gets vertex attributes from material type. Each attribute of type string. */
struct KzcDynamicArray* kzuMaterialTypeGetVertexAttributes(const struct KzuMaterialType* materialType);

/** Returns property types of material type. */
struct KzcDynamicArray* kzuMaterialTypeGetPropertyTypes(const struct KzuMaterialType* materialType);
/** Returns light property types of material type. */
struct KzcHashMap* kzuMaterialTypeGetLightPropertyTypes(const struct KzuMaterialType* materialType);
/** Gets property type from material type by name, KZ_NULL if not found. */
struct KzuPropertyType* kzuMaterialTypeGetPropertyTypeByName(const struct KzuMaterialType* materialType, kzString propertyTypeName);
/** Checks if material type contains specific property type. */
kzBool kzuMaterialTypeContainsPropertyType(const struct KzuMaterialType* materialType, const struct KzuPropertyType* propertyType);
/** Checks if material type has valid shader. */
kzBool kzuMaterialTypeIsValidShader(const struct KzuMaterialType* materialType);
/** Checks if the shader has been set for material type. */
kzBool kzuMaterialTypeIsShaderSetCalled(const struct KzuMaterialType* materialType);


/** Fills shader info with source shader data. */
void kzuMaterialTypeShaderInfoSetSourceShader(struct KzuMaterialTypeShaderInfo* shaderInfo, kzString vertexShaderSourceCode,
                                              kzString fragmentShaderSourceCode, struct KzcDynamicArray* uniforms,
                                              struct KzcDynamicArray* vertexAttributes);
/** Fills shader info with separated binary shader data (different binary for vertex and fragment shader). */
void kzuMaterialTypeShaderInfoSetBinaryShaderSeparated(struct KzuMaterialTypeShaderInfo* shaderInfo, void* vertexShaderData, kzUint vertexShaderLength,
                                                       void* fragmentShaderData, kzUint fragmentShaderLength,struct KzcDynamicArray* uniforms,
                                                       struct KzcDynamicArray* vertexAttributes);
/** Fills shader info with combined binary shader data (same binary for vertex and fragment shader, i.e. complete shader program). */
void kzuMaterialTypeShaderInfoSetBinaryShaderCombined(struct KzuMaterialTypeShaderInfo* shaderInfo, void* shaderData, kzUint shaderLength,
                                                      struct KzcDynamicArray* uniforms, struct KzcDynamicArray* vertexAttributes);
/** Fills shader info with information that no gpu shader is in use (fixed function pipeline). */
void kzuMaterialTypeShaderInfoSetNoShader(struct KzuMaterialTypeShaderInfo* shaderInfo, struct KzcDynamicArray* vertexAttributes);


#endif
