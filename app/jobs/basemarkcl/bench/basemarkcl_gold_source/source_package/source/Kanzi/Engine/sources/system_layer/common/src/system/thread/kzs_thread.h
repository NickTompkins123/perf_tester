/**
* \file
* Threading support.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_THREAD_H
#define KZS_THREAD_H


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/**
 * \struct KzsThread
 * Thread.
 */
struct KzsThread;

/**
 * \struct KzsThreadLock
 * Lock object for synchronizing threads.
 */
struct KzsThreadLock;


/** Thread execution function type. */
typedef kzsError (*KzsThreadRunner)(void* userData);


/**
 * Creates a thread using the given runner function and arbitrary user data. This function also starts the thread.
 * If deleteAutomatically is KZ_TRUE, the thread object is deleted when the thread finishes execution.
 */
kzsError kzsThreadCreate(KzsThreadRunner threadRunner, void* userData, kzBool deleteAutomatically, struct KzsThread** out_thread);

/** Deletes a thread. It is an error to delete a running thread. */
kzsError kzsThreadDelete(struct KzsThread* thread);

/** Waits until the given thread is terminated. */
kzsError kzsThreadJoin(const struct KzsThread* thread);

/**
 * Returns the exit code of the thread. Returns error if the thread has not yet finished execution.
 * Use kzsThreadJoin to make sure the thread is finished.
 */
kzsError kzsThreadGetExitResult(const struct KzsThread* thread);

/** Checks if the given thread is the currently executing thread. */
kzBool kzsThreadIsCurrent(const struct KzsThread* thread);

/** Sleeps the currently executing thread by given amount of milliseconds. */
kzsError kzsThreadSleep(kzUint milliseconds);


/** Creates a thread synchronization lock object. */
kzsError kzsThreadLockCreate(struct KzsThreadLock** out_threadLock);

/** Deletes a thread synchronization lock object. */
kzsError kzsThreadLockDelete(struct KzsThreadLock* threadLock);

/** Starts a synchronized code section by acquiring a thread lock. This call must be paired with kzsThreadLockRelease in all code paths. */
kzsError kzsThreadLockAcquire(struct KzsThreadLock* threadLock);

/** Ends a synchronized code section by releasing a thread lock. This call must be paired with kzsThreadLockAcquire in all code paths. */
kzsError kzsThreadLockRelease(struct KzsThreadLock* threadLock);

/**
 * Waits for a given thread lock to become true. If the lock is already true, this function will not wait.
 * If autoLock is KZ_TRUE, the lock must not be acquired by the thread.
 * If autoLock is KZ_FALSE, the lock must be acquired by the thread exactly once.
 */
kzsError kzsThreadLockWait(struct KzsThreadLock* threadLock, kzBool autoLock);

/**
 * Waits for a given thread lock to become true. If the lock is already true, this function will not wait.
 * After the wait is finished, the lock will be set to KZ_FALSE before any other thread wokes up.
 * If autoLock is KZ_TRUE, the lock must not be acquired by the thread.
 * If autoLock is KZ_FALSE, the lock must be acquired by the thread exactly once.
 */
kzsError kzsThreadLockWaitAndReset(struct KzsThreadLock* threadLock, kzBool autoLock);

/** 
 * Sets the value of the given thread lock to true or false. If the value is true, any thread waiting for it can wake up.
 * If autoLock is KZ_TRUE, the lock must not be acquired by the thread.
 * If autoLock is KZ_FALSE, the lock must be acquired by the thread exactly once.
 */
kzsError kzsThreadLockSet(struct KzsThreadLock* threadLock, kzBool value, kzBool autoLock);

/** 
 * Returns the value of the given thread lock (true or false).
 * If autoLock is KZ_TRUE, the lock must not be acquired by the thread.
 * If autoLock is KZ_FALSE, the lock must be acquired by the thread exactly once.
 */
kzsError kzsThreadLockIsSet(struct KzsThreadLock* threadLock, kzBool autoLock, kzBool* out_isSet);


#endif
