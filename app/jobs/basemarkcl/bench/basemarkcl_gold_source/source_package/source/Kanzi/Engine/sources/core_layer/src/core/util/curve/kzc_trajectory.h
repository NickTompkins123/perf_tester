/**
* \file
* Trajectory. A path in 3d space.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_TRAJECTORY_H
#define KZC_TRAJECTORY_H


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/** Type of trajectory. */
enum KzcTrajectoryType
{
    KZC_TRAJECTORY_TYPE_SPLINE, /**< Series of points. Spline. */ 
    KZC_TRAJECTORY_TYPE_CIRCLE, /**< Circle. */ 
    KZC_TRAJECTORY_TYPE_LINE    /**< Line. */ 
};


/** Type of interpolation for spline trajectory. */
enum KzcTrajectoryInterpolationType
{
    KZC_TRAJECTORY_INTERPOLATION_TYPE_LINEAR,       /**< Linear interpolation. */ 
    KZC_TRAJECTORY_INTERPOLATION_TYPE_CATMULL_ROM,  /**< Catmull-Rom interpolation. */ 
    KZC_TRAJECTORY_INTERPOLATION_TYPE_HERMITE       /**< Hermite interpolation. */ 
};


/* Forward declaration. */
struct KzcMemoryManager;
struct KzcVector3;
struct KzcSpline;

/**
* \struct KzcTrajectory
* Structure to hold a trajectory.
*/
struct KzcTrajectory;


/** 
* Create circle trajectory.
* \param memoryManager MemoryManager to allocate memory from.
* \param origin Origin of the circle trajectory.
* \param up Normal vector for the plane where the circle exists. The vector will be normalized internally.
* \param radius Radius of the circle.
*/
kzsError kzcTrajectoryCreateCircle(const struct KzcMemoryManager* memoryManager, struct KzcVector3 origin,
                                   struct KzcVector3 up, kzFloat radius, struct KzcTrajectory** out_trajectory);

/** 
* Create line trajectory.
* \param memoryManager MemoryManager to allocate memory from
* \param origin Origin of the line.
* \param direction Direction vector for the line. Direction will be normalized internally. 
* \param length Length of the line.
*/
kzsError kzcTrajectoryCreateLine(const struct KzcMemoryManager* memoryManager, struct KzcVector3 origin,
                                 struct KzcVector3 direction, kzFloat length, struct KzcTrajectory** out_trajectory);

/** 
* Create spline trajectory.
* \param memoryManager MemoryManager to allocate memory from.
* \param spline Spline structure to use as trajectory. Takes ownership of the spline and releases upon delete.
*/
kzsError kzcTrajectoryCreateSpline(const struct KzcMemoryManager* memoryManager, struct KzcSpline* spline, struct KzcTrajectory** out_trajectory);


/** Delete trajectory structure. */
kzsError kzcTrajectoryDelete(struct KzcTrajectory* trajectory);

/** Gets a point on trajectory.
* \param t Point on the trajectory. 0.0f start, 1.0f end
*/
kzsError kzcTrajectoryGetPoint(const struct KzcTrajectory* trajectory, kzFloat t, struct KzcVector3* out_position);

/** Returns true if trajectory is complete and usable. */
kzBool kzcTrajectoryIsComplete(const struct KzcTrajectory* trajectory);

/**
 * Takes ownership of the trajectory container deletion. After this, when a trajectory is deleted,
 * only the data is actually deleted and the container remains alive.
 */
void kzcTrajectoryTakeOwnership(const struct KzcTrajectory* trajectory);
/** Checks if the trajectory is valid (pointer and the data are non-null). */
kzBool kzcTrajectoryIsValid(const struct KzcTrajectory* trajectory);
/** Moves the data from source texture to the target trajectory. */
void kzcTrajectoryTransferData(struct KzcTrajectory* targetTrajectory, struct KzcTrajectory* sourceTrajectory);


#endif
