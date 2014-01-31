/**
 * \file
 * User layer error codes.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZU_ERROR_CODES_H
#define KZU_ERROR_CODES_H


/* User layer error codes have reserved range of 30000 - 39999 */

/* Binary loader error codes. (30100 - 30199) */
#define KZU_EXCEPTION_FILE_NOT_FOUND (kzsException)(-30101) /**< Given file was not found from a binary. */
#define KZU_ERROR_INVALID_BINARY_DATA (kzsError)30102 /**< Encountered invalid binary data while reading .kzb. */
#define KZU_ERROR_WRONG_BINARY_FILE_TYPE (kzsError)30103 /**< Wrong binary file type for given file. */
#define KZU_EXCEPTION_WRONG_BINARY_FILE_TYPE (kzsException)(-30103) /**< Wrong binary file type for given file. */
#define KZU_ERROR_INVALID_FILE_PATH (kzsError)30104 /**< Invalid file path. */

/* Scene specific error codes. (30200 - 30299) */
#define KZU_ERROR_INVALID_SCENE_DATA (kzsError)30201 /**< Scene contains invalid data. */
#define KZU_ERROR_INVALID_SCENE_GRAPH (kzsError)30202 /**< Scene graph contains invalid data. */

/* Property specific error codes (30300 - 30399) */
#define KZU_ERROR_PROPERTY_NOT_FOUND (kzsError)30301 /**< Requested property was not found. */
#define KZU_ERROR_INVALID_PROPERTY (kzsError)30302 /**< Property was invalid. */

/* Material specific error codes (30400 - 30499) */
#define KZU_ERROR_INVALID_SHADER_TYPE (kzsError)30401 /**< Invalid shader type. */

/* Project error codes (30600 - 30699) */
#define KZU_ERROR_PROJECT_LOADING_FAILED (kzsError)30601 /**< Failed to load project. */
#define KZU_ERROR_PROJECT_OBJECT_NOT_FOUND (kzsError)30602 /**< Requested object not found from project. */
#define KZU_ERROR_PROJECT_OBJECT_ALREADY_EXISTS (kzsError)30603 /**< An object with same key already exists in the project. */
#define KZU_ERROR_PROJECT_CORE_ASSET_NOT_FOUND (kzsError)30604 /**< Core asset was not found. */

/* Object node error codes (30700 - 30799) */
#define KZU_ERROR_OBJECT_NODE_CHILD_INFO_NOT_FOUND (kzsError)30701 /**< Failed to find child link type. */

/* LOD specific error codes (30800 - 30899) */
#define KZU_ERROR_INVALID_LOD_DATA (kzsError)30801


#endif
