/**
 * \file
 * Core layer error codes.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZC_ERROR_CODES_H
#define KZC_ERROR_CODES_H


/* Core layer error codes have reserved range of 20000 - 29999 */

/* Memory manager error codes. (20100 - 20199) */
#define KZC_ERROR_MEMORY_INVALID_POOL_SIZE (kzsError)20101 /**< Memory pool size is invalid. */
#define KZC_ERROR_MEMORY_ALLOCATION_MISMATCH (kzsError)20102 /**< Incorrect usage of memory allocation/deallocation. */
#define KZC_ERROR_MEMORY_POOL_NOT_FOUND (kzsError)20103 /**< Memory pool for the given pointer could not be determined. */

/* Util / Collection error codes. (20200 - 20299) */
#define KZC_ERROR_ELEMENT_NOT_FOUND (kzsError)20201 /**< Given element not found from the collection. */
#define KZC_ERROR_DUPLICATE_ELEMENT (kzsError)20202 /**< Element already exists in the given collection. */

/* Util / io error codes. (20300 - 20399) */
#define KZC_EXCEPTION_END_OF_STREAM (kzsException)(-20301) /**< End of stream encountered while reading/writing a stream. */
#define KZC_ERROR_UNSPECIFIED_ENDIANNESS (kzsError)20302 /**< Stream endianness was unspecified while trying to read/write non-bytes. */
#define KZC_ERROR_STREAM_OPERATION_FAILED (kzsError)20303 /**< Miscellaneous error for streams. */

/* Util / image error codes. (20400 - 20499) */
#define KZC_ERROR_IMAGE_FORMAT_UNSUPPORTED (kzsError)20401 /**< Unsupported image format. */
#define KZC_ERROR_IMAGE_INVALID (kzsError)20402 /**< Corrupted image data. */
#define KZC_ERROR_IMAGE_WRITE_ERROR (kzsError)20403 /**< Error occurred while writing image data. */
#define KZC_ERROR_IMAGE_MANIPULATION_ERROR (kzsError)20404 /**< Unable to perform given image manipulation operation. */

/* Util / string error codes. (20500 - 20599) */
#define KZC_ERROR_STRING_INVALID_FORMAT (kzsError)20501 /**< Invalid string format. */

/* Util / font error codes (20600 - 20699) */
#define KZC_ERROR_FONT_INVALID_DATA (kzsError)20601 /**< Font data is corrupt. */
#define KZC_ERROR_ITYPE_FAILED (kzsError)20602 /**< Error with iType. */
#define KZC_ERROR_FREETYPE_FAILED (kzsError)20603 /**< Error with FreeType. */
#define KZC_ERROR_FONT_SHADER_NOT_INITIALIZED (kzsError)20604 /**< Uninitialized font shader. */

/* Resource manager error codes (21000 - 21099) */
#define KZC_ERROR_RESOURCE_VERTEX_BUFFER_MAPPING_INVALID (kzsError)21001 /**< Invalid mapping for vertex buffer. */
#define KZC_ERROR_SHADER_COMPILATION_FAILED (kzsError)21002 /**< Shader compilation failed. */
#define KZC_ERROR_TEXTURE_FORMAT_UNSUPPORTED (kzsError)21003 /**< Unsupported texture format. */
#define KZC_ERROR_TEXTURE_SIZE_UNSUPPORTED (kzsError)21004 /**< Unsupported texture size. */

/* Settings parser error codes (21100 - 21199) */
#define KZC_ERROR_PARSER_UNEXPECTED_TOKEN (kzsError)21101 /**< Settings parsing failed due to unexpected token. */
#define KZC_ERROR_PARSER_UNCLOSED_SECTION (kzsError)21102 /**< Settings parsing failed due to unclosed section. */
#define KZC_ERROR_PARSER_PAIRLESS_BRACE (kzsError)21103 /**< Settings parsing failed due to brace mismatch. */

/* Renderer error codes (21200 - 21299) */
#define KZC_ERROR_OPENGL_ERROR (kzsError)21201 /**< OpenGL related error. */
#define KZC_ERROR_SHADER_UNDEFINED (kzsError)20202 /**< Undefined shader error. */


#endif
