/**
* \file
* 2-dimensional rectangle.
* The rectangle sizes remain non-negative for all operations as long as the input sizes are non-negative.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_RECTANGLE_H
#define KZC_RECTANGLE_H


#include "kzc_vector2.h"

#include <system/wrappers/kzs_math.h>
#include <system/kzs_types.h>


/* Forward declarations */
struct KzcVector2;


/** Structure for rectangle. */
struct KzcRectangle
{
    kzFloat x; /**< X-coordinate for rectangle left side. */
    kzFloat y; /**< Y-coordinate for rectangle top side. */
    kzFloat width; /**< Width of the rectangle. */
    kzFloat height; /**< Height of the rectangle. */
};


/** Constant zero area rectangle in origin. */
extern const struct KzcRectangle KZC_RECTANGLE_EMPTY;


/** Creates a rectangle from a point (x,y) and size (width,height). */
KZ_INLINE struct KzcRectangle kzcRectangle(kzFloat x, kzFloat y, kzFloat width, kzFloat height)
{
    struct KzcRectangle rectangle;

    rectangle.x = x;
    rectangle.y = y;
    rectangle.width = width;
    rectangle.height = height;

    return rectangle;
}

/** Creates a rectangle from two points defining arbitrary opposite corners. */
KZ_INLINE struct KzcRectangle kzcRectangleFromPoints(kzFloat x1, kzFloat y1, kzFloat x2, kzFloat y2)
{
    struct KzcRectangle rectangle;

    rectangle.x = kzsMinf(x1, x2);
    rectangle.y = kzsMinf(y1, y2);
    rectangle.width = kzsMaxf(x1, x2) - rectangle.x;
    rectangle.height = kzsMaxf(y1, y2) - rectangle.y;

    return rectangle;
}

/** Creates a rectangle from two points defining arbitrary opposite corners. */
KZ_INLINE struct KzcRectangle kzcRectangleFromVectors(const struct KzcVector2* point1, const struct KzcVector2* point2)
{
    struct KzcRectangle rectangle;

    rectangle.x = kzsMinf(kzcVector2GetX(point1), kzcVector2GetX(point2));
    rectangle.y = kzsMinf(kzcVector2GetY(point1), kzcVector2GetY(point2));
    rectangle.width = kzsMaxf(kzcVector2GetX(point1), kzcVector2GetX(point2)) - rectangle.x;
    rectangle.height = kzsMaxf(kzcVector2GetY(point1), kzcVector2GetY(point2)) - rectangle.y;

    return rectangle;
}

/** Creates a rectangle from a point vector and size vector. */
KZ_INLINE struct KzcRectangle kzcRectangleFromVectorAndSize(const struct KzcVector2* point, const struct KzcVector2* size)
{
    struct KzcRectangle rectangle;

    rectangle.x = kzcVector2GetX(point);
    rectangle.y = kzcVector2GetY(point);
    rectangle.width = kzcVector2GetX(size);
    rectangle.height = kzcVector2GetY(size);

    return rectangle;
}

/** Checks if the rectangle is empty. */
KZ_INLINE kzBool kzcRectangleIsEmpty(const struct KzcRectangle* rectangle)
{
    return (rectangle->width <= 0 || rectangle->height <= 0);
}

/** Checks if the given point is inside the given rectangle. The rectangle is closed from left/right but open from right/bottom. */
KZ_INLINE kzBool kzcRectangleContainsPoint(const struct KzcRectangle* rectangle, const struct KzcVector2* point)
{
    kzFloat x = kzcVector2GetX(point) - rectangle->x;
    kzFloat y = kzcVector2GetY(point) - rectangle->y;
    return (x >= 0 && x < rectangle->width && 
            y >= 0 && y < rectangle->height);
}

/** Checks if the given rectangle contains the given other rectangle completely. */
KZ_INLINE kzBool kzcRectangleContainsRectangle(const struct KzcRectangle* rectangle, const struct KzcRectangle* otherRectangle)
{
    return ((rectangle->x <= otherRectangle->x) && (rectangle->x + rectangle->width >= otherRectangle->x + otherRectangle->width) &&
            (rectangle->y <= otherRectangle->y) && (rectangle->y + rectangle->height >= otherRectangle->y + otherRectangle->height));
}

/** Checks if the two given rectangles overlap at least partially. */
KZ_INLINE kzBool kzcRectangleOverlaps(const struct KzcRectangle* rectangle1, const struct KzcRectangle* rectangle2)
{
    return ((rectangle1->x < rectangle2->x + rectangle2->width) && (rectangle2->x < rectangle1->x + rectangle1->width) &&
            (rectangle1->y < rectangle2->y + rectangle2->height) && (rectangle2->y < rectangle1->y + rectangle1->height));
}

/** Checks if the two given rectangles are same. */
KZ_INLINE kzBool kzcRectangleIsEqual(const struct KzcRectangle* rectangle1, const struct KzcRectangle* rectangle2)
{
    return (kzsFloatIsEqual(rectangle1->x, rectangle2->x) && kzsFloatIsEqual(rectangle1->width, rectangle2->width) &&
            kzsFloatIsEqual(rectangle1->y, rectangle2->y) && kzsFloatIsEqual(rectangle1->height, rectangle2->height));
}

/** Calculates bound rectangle for union of two rectangles. */
KZ_INLINE struct KzcRectangle kzcRectangleUnion(const struct KzcRectangle* rectangle1, const struct KzcRectangle* rectangle2)
{
    struct KzcRectangle targetRectangle;

    targetRectangle.x = kzsMinf(rectangle1->x, rectangle2->x);
    targetRectangle.y = kzsMinf(rectangle1->y, rectangle2->y);
    targetRectangle.width = kzsMaxf(rectangle1->x + rectangle1->width, rectangle2->x + rectangle2->width) - targetRectangle.x;
    targetRectangle.height = kzsMaxf(rectangle1->y + rectangle1->height, rectangle2->y + rectangle2->height) - targetRectangle.y;

    return targetRectangle;
}

/** Calculates intersection of two rectangles. */
KZ_INLINE struct KzcRectangle kzcRectangleIntersection(const struct KzcRectangle* rectangle1, const struct KzcRectangle* rectangle2)
{
    struct KzcRectangle targetRectangle;

    targetRectangle.x = kzsMaxf(rectangle1->x, rectangle2->x);
    targetRectangle.y = kzsMaxf(rectangle1->y, rectangle2->y);
    targetRectangle.width = kzsMaxf(0.0f, kzsMinf(rectangle1->x + rectangle1->width, rectangle2->x + rectangle2->width) - targetRectangle.x);
    targetRectangle.height = kzsMaxf(0.0f, kzsMinf(rectangle1->y + rectangle1->height, rectangle2->y + rectangle2->height) - targetRectangle.y);

    return targetRectangle;
}

/** Calculates the area of a rectangle. */
KZ_INLINE kzFloat kzcRectangleGetArea(const struct KzcRectangle* rectangle)
{
    return rectangle->width * rectangle->height;
}


#endif
