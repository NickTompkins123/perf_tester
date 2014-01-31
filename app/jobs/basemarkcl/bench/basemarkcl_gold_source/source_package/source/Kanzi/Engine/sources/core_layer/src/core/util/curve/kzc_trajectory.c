/**
* \file
* Trajectory. A path in 3d space.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_trajectory.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/curve/kzc_spline.h>


/** Internal helper for creating the trajectory. */
static kzsError kzcTrajectoryCreate_internal(const struct KzcMemoryManager* memoryManager, enum KzcTrajectoryType type, 
                                             struct KzcTrajectory** out_trajectory);

struct KzcTrajectoryData
{
    kzBool selfOwned;
    enum KzcTrajectoryType type; /**< Type of the trajectory. */
    /* Type specific data. */
    union 
    {
        struct 
        {
            struct KzcSpline* spline; /**< The spline of the trajectory. */
        } spline;

        struct 
        {
            kzFloat radius; /**< Radius of circle. */
            struct KzcVector3 origin; /**< Origin of circle. */
            struct KzcVector3 up; /**< Normalized up vector for plane where circle is on. */
        } circle;

        struct
        {
            kzFloat length; /**< Length of the line. */
            struct KzcVector3 origin; /**< Origin of the line. */
            struct KzcVector3 direction; /**< Normalized direction of the line. */
        } line;
    } typeData;
};


struct KzcTrajectory
{
    struct KzcTrajectoryData* data;  
};


static kzsError kzcTrajectoryCreate_internal(const struct KzcMemoryManager* memoryManager, enum KzcTrajectoryType type,
                                     struct KzcTrajectory** out_trajectory)
{
    kzsError result;
    struct KzcTrajectory* trajectory;
    struct KzcTrajectoryData* trajectoryData;

    result = kzcMemoryAllocVariable(memoryManager, trajectory, "Trajectory");
    kzsErrorForward(result);

    result = kzcMemoryAllocVariable(memoryManager, trajectoryData, "Trajectory Data");
    kzsErrorForward(result);

    trajectory->data = trajectoryData;
    trajectoryData->selfOwned = KZ_TRUE;
    trajectoryData->type = type;

    *out_trajectory = trajectory;
    kzsSuccess();
}

kzsError kzcTrajectoryCreateCircle(const struct KzcMemoryManager* memoryManager, struct KzcVector3 origin,
                             struct KzcVector3 up, kzFloat radius, struct KzcTrajectory** out_trajectory)
{
    kzsError result;
    struct KzcTrajectory* trajectory;

    result = kzcTrajectoryCreate_internal(memoryManager, KZC_TRAJECTORY_TYPE_CIRCLE, &trajectory);
    kzsErrorForward(result);

    trajectory->data->typeData.circle.radius = radius;
    trajectory->data->typeData.circle.origin = origin;
    trajectory->data->typeData.circle.up = up;

    kzcVector3Normalize(&trajectory->data->typeData.circle.up);

    *out_trajectory = trajectory;
    kzsSuccess();
}

kzsError kzcTrajectoryCreateLine(const struct KzcMemoryManager* memoryManager, struct KzcVector3 origin,
                                struct KzcVector3 direction, kzFloat length, struct KzcTrajectory** out_trajectory)
{
    kzsError result;
    struct KzcTrajectory* trajectory;

    result = kzcTrajectoryCreate_internal(memoryManager, KZC_TRAJECTORY_TYPE_LINE, &trajectory);
    kzsErrorForward(result);

    trajectory->data->typeData.line.origin = origin;
    trajectory->data->typeData.line.direction = direction;
    trajectory->data->typeData.line.length = length;

    kzcVector3Normalize(&trajectory->data->typeData.line.direction);

    *out_trajectory = trajectory;
    kzsSuccess();
}
 
kzsError kzcTrajectoryCreateSpline(const struct KzcMemoryManager* memoryManager, struct KzcSpline* spline, struct KzcTrajectory** out_trajectory)
{
    kzsError result;
    struct KzcTrajectory* trajectory;

    result = kzcTrajectoryCreate_internal(memoryManager, KZC_TRAJECTORY_TYPE_SPLINE, &trajectory);
    kzsErrorForward(result);

    trajectory->data->typeData.spline.spline = spline;

    *out_trajectory = trajectory;
    kzsSuccess();
}

kzsError kzcTrajectoryDelete(struct KzcTrajectory* trajectory)
{
    kzsError result;
    kzBool selfOwned;
    
    kzsAssert(kzcTrajectoryIsValid(trajectory));

    selfOwned = trajectory->data->selfOwned;

    if(trajectory->data->type == KZC_TRAJECTORY_TYPE_SPLINE && trajectory->data->typeData.spline.spline != KZ_NULL)
    {
        result = kzcSplineDelete(trajectory->data->typeData.spline.spline);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreeVariable(trajectory->data);
    kzsErrorForward(result);

    if(selfOwned)
    {
        result = kzcMemoryFreeVariable(trajectory);
        kzsErrorForward(result);
    }
    else
    {
        trajectory->data = KZ_NULL;
    }

    kzsSuccess();
}

kzsError kzcTrajectoryGetPoint(const struct KzcTrajectory* trajectory, kzFloat t, struct KzcVector3* out_position)
{
    struct KzcVector3 position;
    kzsAssert(kzcTrajectoryIsValid(trajectory));

    switch(trajectory->data->type)
    {
        case KZC_TRAJECTORY_TYPE_CIRCLE:
        {
            kzFloat angle = t * 2.0f * KZS_PI;
            struct KzcMatrix4x4 matrix = KZC_MATRIX4X4_IDENTITY;

            kzcMatrix4x4Rotate(&matrix, angle, 
                trajectory->data->typeData.circle.up.data[0], 
                trajectory->data->typeData.circle.up.data[1], 
                trajectory->data->typeData.circle.up.data[2]);

            /* Find arbitrary vector perpendicular to the up vector. */
            {
                /* normal = anyVector - up * dot(anyVector, up) where dot(anyVector, up) != 1 */                
                struct KzcVector3 n = kzcVector3(trajectory->data->typeData.circle.up.data[2], 
                                trajectory->data->typeData.circle.up.data[1], 
                                trajectory->data->typeData.circle.up.data[0]);
                struct KzcVector3 f = trajectory->data->typeData.circle.up;

                /* If vectors are perpendicular select new one. */
                if(kzcVector3Dot(&n, &f) > 0.99f)
                {
                    n = kzcVector3(1.0f, 0.0f, 0.0f);    
                }

                kzcVector3Scale(&f, kzcVector3Dot(&n, &f), &f);
                kzcVector3Subtract(&n, &f, &n);
                                
                kzcMatrix4x4Translate(&matrix, n.data[0] * trajectory->data->typeData.circle.radius, 
                    n.data[1] * trajectory->data->typeData.circle.radius, 
                    n.data[2] * trajectory->data->typeData.circle.radius);
            }

            position.data[0] = matrix.data[KZC_MATRIX4X4_INDEX_TRANSLATION_X];
            position.data[1] = matrix.data[KZC_MATRIX4X4_INDEX_TRANSLATION_Y];
            position.data[2] = matrix.data[KZC_MATRIX4X4_INDEX_TRANSLATION_Z];
            break;
        }
        case KZC_TRAJECTORY_TYPE_LINE:
        {
            struct KzcVector3 origin = trajectory->data->typeData.line.origin;
            struct KzcVector3 direction = trajectory->data->typeData.line.direction;
            kzcVector3Scale(&direction, trajectory->data->typeData.line.length * t, &position);
            kzcVector3Add(&origin, &position, &position);
            break;
        }
        case KZC_TRAJECTORY_TYPE_SPLINE:
        {
            kzcSplineGetPoint(trajectory->data->typeData.spline.spline, t * (kzFloat)kzcSplineGetLength(trajectory->data->typeData.spline.spline), position.data);
            break;
        }
        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Trajectory type unknown");
        }
    }
    
    *out_position = position;
    kzsSuccess();
}

kzBool kzcTrajectoryIsComplete(const struct KzcTrajectory* trajectory)
{
    kzBool complete = KZ_TRUE;

    if(trajectory->data->type == KZC_TRAJECTORY_TYPE_SPLINE && trajectory->data->typeData.spline.spline == KZ_NULL)
    {
        complete = KZ_FALSE;
    }

    return complete;
}

void kzcTrajectoryTakeOwnership(const struct KzcTrajectory* trajectory)
{
    kzsAssert(kzcTrajectoryIsValid(trajectory));
    trajectory->data->selfOwned = KZ_FALSE;
}

kzBool kzcTrajectoryIsValid(const struct KzcTrajectory* trajectory)
{
    return trajectory != KZ_NULL && trajectory->data != KZ_NULL;
}

void kzcTrajectoryTransferData(struct KzcTrajectory* targetTrajectory, struct KzcTrajectory* sourceTrajectory)
{
    kzsAssert(kzcIsValidPointer(targetTrajectory));
    kzsAssert(targetTrajectory->data == KZ_NULL);
    kzsAssert(kzcTrajectoryIsValid(sourceTrajectory));

    targetTrajectory->data = sourceTrajectory->data;
    sourceTrajectory->data = KZ_NULL;
}
