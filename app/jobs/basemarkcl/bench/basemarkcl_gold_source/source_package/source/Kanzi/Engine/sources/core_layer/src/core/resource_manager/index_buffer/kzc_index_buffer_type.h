/**
* \file
* Index buffer enumerations.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_INDEX_BUFFER_TYPE_H
#define KZC_INDEX_BUFFER_TYPE_H


/** Enumeration for index buffer primitive type. */
enum KzcPrimitiveType
{
    KZC_PRIMITIVE_TYPE_POINTS, /**< Point primitive type. */
    KZC_PRIMITIVE_TYPE_LINES, /**< Line primitive type. */
    KZC_PRIMITIVE_TYPE_LINE_LOOP, /**< Line loop primitive type. */
    KZC_PRIMITIVE_TYPE_LINE_STRIP, /**< Line strip primitive type. */
    KZC_PRIMITIVE_TYPE_TRIANGLES, /**< Triangle primitive type. */
    KZC_PRIMITIVE_TYPE_TRIANGLE_STRIP, /**< Triangle strip primitive type. */
    KZC_PRIMITIVE_TYPE_TRIANGLE_FAN /**< Triangle fan primitive type. */
};


#endif

