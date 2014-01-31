/**
* \file
* Project structure containing all the scenes and their resources.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_H
#define KZU_PROJECT_H


#include <core/util/collection/kzc_hash_map.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/** Type for a project object. */
enum KzuProjectObjectType
{
    KZU_PROJECT_OBJECT_TYPE_INVALID, /**< Invalid object type. */
    KZU_PROJECT_OBJECT_TYPE_IMAGE, /**< Image object type. */
    KZU_PROJECT_OBJECT_TYPE_SCENE, /**< Scene object type. */
    KZU_PROJECT_OBJECT_TYPE_TEXTURE, /**< Texture object type. */
    KZU_PROJECT_OBJECT_TYPE_PROPERTY_TYPE, /**< Property type object type. */
    KZU_PROJECT_OBJECT_TYPE_MATERIAL_TYPE, /**< Material type object type. */
    KZU_PROJECT_OBJECT_TYPE_MATERIAL, /**< Material object type. */
    KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, /**< Object node object type. */
    KZU_PROJECT_OBJECT_TYPE_MESH, /**< Mesh object type. */
    KZU_PROJECT_OBJECT_TYPE_SHADER_SOURCE, /**< Shader source object type. */
    KZU_PROJECT_OBJECT_TYPE_COMPOSER, /**< Composer object type. */
    KZU_PROJECT_OBJECT_TYPE_OBJECT_SOURCE, /**< Object source object type. */
    KZU_PROJECT_OBJECT_TYPE_ANIMATION, /**< Animation data. */
    KZU_PROJECT_OBJECT_TYPE_ANIMATION_CLIP, /**< Animation clip data. */
    KZU_PROJECT_OBJECT_TYPE_TIMELINE_SEQUENCE, /**< Time line sequence data. */
    KZU_PROJECT_OBJECT_TYPE_UI_COMPONENT_TYPE, /**< User interface component type. */
    KZU_PROJECT_OBJECT_TYPE_FONT, /**< Font. */
    KZU_PROJECT_OBJECT_TYPE_TRANSITION, /**< Scene object type. */
    KZU_PROJECT_OBJECT_TYPE_SCRIPT, /**< Script content. */
    KZU_PROJECT_OBJECT_TYPE_TRAJECTORY, /**< Trajectory. */
    KZU_PROJECT_OBJECT_TYPE_PROPERTY_GROUP, /**< Property group. */
    KZU_PROJECT_OBJECT_TYPE_SPLINE /**< Spline. */
};

/** Project measurement types, used for measuring loading times of specific resources. */
enum KzuProjectMeasurementType
{
    KZU_PROJECT_MEASUREMENT_IMAGE, /**< Measurement for image. */
    KZU_PROJECT_MEASUREMENT_COMPRESSED_IMAGE, /**< Measurement for compressed image. */
    KZU_PROJECT_MEASUREMENT_MESH, /**< Measurement for mesh. */
    KZU_PROJECT_MEASUREMENT_ANIMATION, /**< Measurement for animation. */
    KZU_PROJECT_MEASUREMENT_OBJECT_NODE, /**< Measurement for object node. */
    KZU_PROJECT_MEASUREMENT_MATERIAL_TYPE /**< Measurement for material types. */
};


/* Forward declarations */
struct KzuScene;
struct KzuPropertyType;
struct KzuProperty;
struct KzuPropertyManager;
struct KzuMessageDispatcher;
struct KzuObjectNode;
struct KzuUiManager;
struct KzuBinaryDirectory;
struct KzcMemoryManager;
struct KzcResourceManager;
struct KzcBitmapFontSystem;
struct KzcTruetypeSystem;


/**
 * \struct KzuProject
 * Project structure.
 */
struct KzuProject;


/** Iterator for project objects of specific type. */
struct KzuProjectObjectIterator
{
    enum KzuProjectObjectType type_private;         /**< Type of objects to look for when iterating. */
    struct KzcHashMapIterator mapIterator_private;  /**< Hash map iterator corresponding the project iterator. */
};


/** Creates a new project object. */
kzsError kzuProjectCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                          struct KzuBinaryDirectory* directory, struct KzuProject** out_project);
/** Deletes a project object. */
kzsError kzuProjectDelete(struct KzuProject* project);

/** Deletes the content of a project. */
kzsError kzuProjectClear(const struct KzuProject* project);
/** Deletes the project resource cache. Contains only the resources that are cached. */
kzsError kzuProjectClearCache(const struct KzuProject* project);


/** Returns the resource manager of the project. */
struct KzcResourceManager* kzuProjectGetResourceManager(const struct KzuProject* project);

/** Returns the property manager of the project. */
struct KzuPropertyManager* kzuProjectGetPropertyManager(const struct KzuProject* project);

/** Returns the message dispatcher of the project. */
struct KzuMessageDispatcher* kzuProjectGetMessageDispatcher(const struct KzuProject* project);

/** Returns the binary directory of the project. */
struct KzuBinaryDirectory* kzuProjectGetBinaryDirectory(const struct KzuProject* project);


/**
 * Adds an object of specified type to the project. The object is registered with the given name.
 * Only one object with the same name can be added to a project. The object can later be queried with the name.
 * An error is thrown if the object already exists in the project.
 */
kzsError kzuProjectAddObject(const struct KzuProject* project, enum KzuProjectObjectType type, void* object, kzString path);

/** Removes object with given type and path from project. Ownership has to be set back to object after this operation. */
kzsError kzuProjectRemoveObject(const struct KzuProject* project, enum KzuProjectObjectType type, kzString path);

/**
 * Adds an object of specified type to the project. The object is registered with the given name.
 * If an object with the same name and type was already stored in the project before but the data was deleted,
 * this function will move the data from the given object to the pre-existing one, and return the already stored object.
 */
kzsError kzuProjectObjectLoaded(const struct KzuProject* project, enum KzuProjectObjectType type, void* object,
                                kzString path, kzBool storeToResourceCache, void** out_storedObject);

/**
 * Adds an object of specified type to the project. The object is not registered with any name which could be used
 * to identify it or queried from the project.
 */
kzsError kzuProjectAddAnonymousObject(struct KzuProject* project, enum KzuProjectObjectType type, void* object);

/** Deletes an object of given type. The object must have been registered previously with the given path. */
kzsError kzuProjectDeleteObject(const struct KzuProject* project, enum KzuProjectObjectType type, kzString path);

/** Gets an object of given type. The object must have been registered previously with the given path. */
void* kzuProjectGetObject(const struct KzuProject* project, enum KzuProjectObjectType type, kzString path);

/** Geths the path of given object. */
kzString kzuProjectGetObjectPath(const struct KzuProject* project, const void* object);

/** Gets root source from project.  */
struct KzuObjectSource* kzuProjectGetRootSource(const struct KzuProject* project);


/** Adds single file path as permanent, which means that object with this path is not removed from memory when projectClear is called. */
kzsError kzuProjectAddPermanentFilePath(const struct KzuProject* project, kzString filePath);
/** Gets permanent file paths from project. */
struct KzcHashSet* kzuProjectGetPermanentFilePaths(const struct KzuProject* project);
/** Returns if specific file path is permanent inside project. */
kzBool kzuProjectHasPermanentFilePath(const struct KzuProject* project, kzString filePath);
/** Removes single file path from being permanent. */
kzsError kzuProjectRemovePermanentFilePath(const struct KzuProject* project, kzString filePath);


/**
 * Gets a property type from the project. This function returns either a previously registered object or
 * a fixed property type if the name matches any.
 */
struct KzuPropertyType* kzuProjectGetPropertyType(const struct KzuProject* project, kzString name);


/** Returns an iterator to specific type of objects in a project. */
struct KzuProjectObjectIterator kzuProjectGetObjectsOfType(const struct KzuProject* project, enum KzuProjectObjectType type);

/** Finds the next entry in the iterator. Returns KZ_TRUE if next entry is found, otherwise KZ_FALSE. */
#define kzuProjectObjectIterate(iterator_param) kzuProjectObjectIterate_private(&iterator_param)
/** Returns the next value in iterator. */
#define kzuProjectObjectIteratorGetValue(iterator_param) kzuProjectObjectIteratorGetValue_private(&iterator_param)

/** Iterates objects. */
kzBool kzuProjectObjectIterate_private(struct KzuProjectObjectIterator* iterator);
/** Gets value from object pointed by iterator. */
void* kzuProjectObjectIteratorGetValue_private(const struct KzuProjectObjectIterator* iterator);

/** Get ui project from project. */
struct KzuUiManager* kzuProjectGetUiProject(const struct KzuProject* project);

/** Gets a bitmap font system of the project. It is created and deleted automatically if necessary. */
kzsError kzuProjectGetBitmapFontSystem(struct KzuProject* project, struct KzcBitmapFontSystem** out_bitmapFontSystem);

/** Sets a TrueType font system for the project, which overrides the default TrueType system. */
kzsError kzuProjectSetTruetypeSystem(struct KzuProject* project, struct KzcTruetypeSystem* truetypeSystem);

/** Gets a TrueType font system of the project. It is created and deleted automatically if necessary. */
kzsError kzuProjectGetTruetypeSystem(struct KzuProject* project, struct KzcTruetypeSystem** out_truetypeSystem);

/** Sets project viewport. */
kzsError kzuProjectSetViewport(const struct KzuProject* project, kzFloat x, kzFloat y, kzFloat width, kzFloat height, kzFloat aspectRatio,
                               kzBool relative);

/** Sets time measurement enabled / disabled. */
void kzuProjectSetTimeMeasurementEnabled(struct KzuProject* project, kzBool enabled);
/** Returns if time measurement is enabled. */
kzBool kzuProjectIsTimeMeasurementEnabled(const struct KzuProject* project);
/** Private function for adding cumulative loading time. */
void kzuProjectAddMeasurementCumulativeTime_private(struct KzuProject* project, kzUint startTime, enum KzuProjectMeasurementType measurementType);
/** Prints project loader time measurement info. */
kzsError kzuProjectPrintTimeMeasurementInfo(const struct KzuProject* project);
/** Gets time measurement info in milliseconds. */
kzUint kzuProjectGetTimeMeasurementInfo(const struct KzuProject* project, enum KzuProjectMeasurementType measurementType);


#endif
