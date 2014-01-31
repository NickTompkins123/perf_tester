/**
* \file
* Threading support.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include <system/thread/kzs_thread.h>

#include <system/wrappers/kzs_memory.h>
#include <system/kzs_error_codes.h>

#include <pthread.h>
#include <sys/time.h>
#include <errno.h>

#define KZS_THREAD_DEBUG 0
#if KZS_THREAD_DEBUG
#include <stdio.h>
#endif


struct KzsThread
{
    pthread_t id;
    KzsThreadRunner threadRunner;
    void* userData;
    kzBool deleteAutomatically;
    kzsError exitResult;
};

struct KzsThreadLock
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    kzBool value;
};

static pthread_mutex_t g_kzsThreadSleepMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_kzsThreadSleepCond = PTHREAD_COND_INITIALIZER;


static void* kzsThreadRunner_internal(void* arg)
{
    kzsError result;
    struct KzsThread* thread = (struct KzsThread*)arg;

    result = thread->threadRunner(thread->userData);
    thread->exitResult = result;

    if (thread->deleteAutomatically)
    {
        kzsError result2 = kzsThreadDelete(thread);
        if (result == KZS_SUCCESS)
        {
            thread->exitResult = result2;
        }
    }

    return NULL;
}


kzsError kzsThreadCreate(KzsThreadRunner threadRunner, void* userData, kzBool deleteAutomatically, struct KzsThread** out_thread)
{
    kzInt result;
    struct KzsThread* thread;

    thread = kzsMalloc(sizeof(*thread));
    kzsErrorTest(thread != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Out of memory while creating thread");

    thread->threadRunner = threadRunner;
    thread->userData = userData;
    thread->deleteAutomatically = deleteAutomatically;
    thread->exitResult = KZS_ERROR_THREAD_NOT_FINISHED;

    result = pthread_create(&thread->id, NULL, kzsThreadRunner_internal, thread);
    kzsErrorTest(result == 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to create thread");

    *out_thread = thread;
    kzsSuccess();
}

kzsError kzsThreadDelete(struct KzsThread* thread)
{
    kzsFree(thread);

    kzsSuccess();
}

kzsError kzsThreadJoin(const struct KzsThread* thread)
{
    kzInt result;

    kzsErrorTest(!thread->deleteAutomatically, KZS_ERROR_THREAD_OPERATION_FAILED, "Cannot join a thread with automatic deletion");

    result = pthread_join(thread->id, NULL);
    kzsErrorTest(result == 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to join thread");

    kzsSuccess();
}

kzsError kzsThreadGetExitResult(const struct KzsThread* thread)
{
    return thread->exitResult;
}

kzBool kzsThreadIsCurrent(const struct KzsThread* thread)
{
    return thread->id == pthread_self();
}

kzsError kzsThreadSleep(kzUint milliseconds)
{
    kzInt result;
    struct timeval currentTime;
    struct timespec timeout;

    gettimeofday(&currentTime, NULL);

    timeout.tv_sec = currentTime.tv_sec + milliseconds / 1000;
    timeout.tv_nsec = currentTime.tv_usec * 1000 + (milliseconds % 1000) * 1000000;
    if (timeout.tv_nsec > 1000000000)
    {
        ++timeout.tv_sec;
        timeout.tv_nsec -= 1000000000;
    }

    result = pthread_mutex_lock(&g_kzsThreadSleepMutex);
    kzsErrorTest(result == 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to lock mutex for sleep");

    result = pthread_cond_timedwait(&g_kzsThreadSleepCond, &g_kzsThreadSleepMutex, &timeout);
    kzsErrorTest(result == ETIMEDOUT, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to sleep thread");

    result = pthread_mutex_unlock(&g_kzsThreadSleepMutex);
    kzsErrorTest(result == 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to unlock mutex after sleep");

    kzsSuccess();
}


kzsError kzsThreadLockCreate(struct KzsThreadLock** out_threadLock)
{
    kzInt result;
    struct KzsThreadLock* threadLock;

    threadLock = kzsMalloc(sizeof(*threadLock));
    kzsErrorTest(threadLock != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Out of memory while creating thread lock");

    result = pthread_mutex_init(&threadLock->mutex, NULL);
    kzsErrorTest(result == 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to create mutex");

    result = pthread_cond_init(&threadLock->cond, NULL);
    kzsErrorTest(result == 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to create condition variable");

    threadLock->value = KZ_FALSE;

    *out_threadLock = threadLock;
    kzsSuccess();
}

kzsError kzsThreadLockDelete(struct KzsThreadLock* threadLock)
{
    kzInt result;
    
    result = pthread_mutex_destroy(&threadLock->mutex);
    kzsErrorTest(result == 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to delete mutex");

    result = pthread_cond_destroy(&threadLock->cond);
    kzsErrorTest(result == 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to delete conditional variable");

    kzsFree(threadLock);

    kzsSuccess();
}

kzsError kzsThreadLockAcquire(struct KzsThreadLock* threadLock)
{
    kzInt result;

    result = pthread_mutex_lock(&threadLock->mutex);
    kzsErrorTest(result == 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to lock mutex");
#if KZS_THREAD_DEBUG
    printf("Locked mutex %p\n", (void*)&threadLock->mutex);
#endif

    kzsSuccess();
}

kzsError kzsThreadLockRelease(struct KzsThreadLock* threadLock)
{
    kzInt result;

    result = pthread_mutex_unlock(&threadLock->mutex);
    kzsErrorTest(result == 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to unlock mutex");
#if KZS_THREAD_DEBUG
    printf("Released mutex %p\n", (void*)&threadLock->mutex);
#endif

    kzsSuccess();
}

kzsError kzsThreadLockWait(struct KzsThreadLock* threadLock, kzBool autoLock)
{
    kzInt result;

    if (autoLock)
    {
        result = pthread_mutex_lock(&threadLock->mutex);
        kzsErrorTest(result == 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to lock mutex");
#if KZS_THREAD_DEBUG
        printf("Locked mutex %p\n", (void*)&threadLock->mutex);
#endif
    }

    /* This function assumes that the mutex is already locked by this thread. */
    while (!threadLock->value)
    {
        /* Wait for a condition variable. This will unlock and lock again the mutex. */
#if KZS_THREAD_DEBUG
        printf("Waiting for %p (%p)\n", (void*)&threadLock->cond, (void*)&threadLock->mutex);
#endif
        result = pthread_cond_wait(&threadLock->cond, &threadLock->mutex);
        kzsErrorTest(result == 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to wait for condition variable");
    }

    if (autoLock)
    {
        result = pthread_mutex_unlock(&threadLock->mutex);
        kzsErrorTest(result == 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to unlock mutex");
#if KZS_THREAD_DEBUG
        printf("Released mutex %p\n", (void*)&threadLock->mutex);
#endif
    }

    kzsSuccess();
}
kzsError kzsThreadLockWaitAndReset(struct KzsThreadLock* threadLock, kzBool autoLock)
{
    kzsError result;
    kzInt result1;

    if (autoLock)
    {
        result1 = pthread_mutex_lock(&threadLock->mutex);
        kzsErrorTest(result1 == 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to lock mutex");
#if KZS_THREAD_DEBUG
        printf("Locked mutex %p\n", (void*)&threadLock->mutex);
#endif
    }

    result = kzsThreadLockWait(threadLock, KZ_FALSE);
    kzsErrorForward(result);

    result = kzsThreadLockSet(threadLock, KZ_FALSE, KZ_FALSE);
    kzsErrorForward(result);

    if (autoLock)
    {
        result1 = pthread_mutex_unlock(&threadLock->mutex);
        kzsErrorTest(result1 == 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to unlock mutex");
#if KZS_THREAD_DEBUG
        printf("Released mutex %p\n", (void*)&threadLock->mutex);
#endif
    }

    kzsSuccess();
}

kzsError kzsThreadLockSet(struct KzsThreadLock* threadLock, kzBool value, kzBool autoLock)
{
    kzInt result;

    if (autoLock)
    {
        result = pthread_mutex_lock(&threadLock->mutex);
        kzsErrorTest(result == 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to lock mutex");
#if KZS_THREAD_DEBUG
        printf("Locked mutex %p\n", (void*)&threadLock->mutex);
#endif
    }
    
    /* Set the lock value. */
    threadLock->value = value;
    if (value)
    {
#if KZS_THREAD_DEBUG
        printf("Signaling %p (%p)\n", (void*)&threadLock->cond, (void*)&threadLock->mutex);
#endif
        result = pthread_cond_broadcast(&threadLock->cond);
        kzsErrorTest(result == 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to signal condition variable");
    }

    if (autoLock)
    {
        result = pthread_mutex_unlock(&threadLock->mutex);
        kzsErrorTest(result == 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to unlock mutex");
#if KZS_THREAD_DEBUG
        printf("Released mutex %p\n", (void*)&threadLock->mutex);
#endif
    }
    
    kzsSuccess();
}

kzsError kzsThreadLockIsSet(struct KzsThreadLock* threadLock, kzBool autoLock, kzBool* out_isSet)
{
    kzInt result;
    kzBool value;

    if (autoLock)
    {
        result = pthread_mutex_lock(&threadLock->mutex);
        kzsErrorTest(result == 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to lock mutex");
#if KZS_THREAD_DEBUG
        printf("Locked mutex %p\n", (void*)&threadLock->mutex);
#endif
    }

    value = threadLock->value;

    if (autoLock)
    {
        result = pthread_mutex_unlock(&threadLock->mutex);
        kzsErrorTest(result == 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to unlock mutex");
#if KZS_THREAD_DEBUG
        printf("Released mutex %p\n", (void*)&threadLock->mutex);
#endif
    }

    *out_isSet = value;
    kzsSuccess();
}
