/**
* \file
* Threading support.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include <system/thread/kzs_thread.h>

#include <system/wrappers/kzs_memory.h>
#include <system/kzs_error_codes.h>

#include <windows.h>
#include <WinDef.h>

#define KZ_THREAD_DEBUG 0
#if KZ_THREAD_DEBUG
#include <stdio.h>
#endif


struct KzsThread
{
    HANDLE handle;
    KzsThreadRunner threadRunner;
    void* userData;
    kzBool deleteAutomatically;
    kzsError exitResult;
    HANDLE finishedEvent;
};

struct KzsThreadLock
{
    HANDLE mutex;
    HANDLE event;
    kzBool value;
};


DWORD WINAPI kzsThreadRunner_internal(LPVOID lpParameter)
{
    kzsError result;
    BOOL result2;
    DWORD exitCode;
    struct KzsThread* thread = (struct KzsThread*)lpParameter;

    result = thread->threadRunner(thread->userData);
    kzsErrorIf(result)
    {
        exitCode = EXIT_FAILURE;
    }
    else
    {
        exitCode = EXIT_SUCCESS;
    }
    thread->exitResult = result;

    if (thread->deleteAutomatically)
    {
        result = kzsThreadDelete(thread);
        kzsErrorIf(result)
        {
            exitCode = EXIT_FAILURE;
        }
    }
    else
    {
        result2 = SetEvent(thread->finishedEvent);
        kzsErrorTest(result2 != 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to set event for thread finishing");
    }

    return exitCode;
}


kzsError kzsThreadCreate(KzsThreadRunner threadRunner, void* userData, kzBool deleteAutomatically, struct KzsThread** out_thread)
{
    DWORD result;
    struct KzsThread* thread;

    thread = kzsMalloc(sizeof(*thread));
    kzsErrorTest(thread != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Out of memory while creating thread");

    thread->handle = CreateThread(NULL, 0, kzsThreadRunner_internal, thread, CREATE_SUSPENDED, NULL);
    kzsErrorTest(thread->handle != NULL, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to create thread");

    if (deleteAutomatically)
    {
        thread->finishedEvent = KZ_NULL;
    }
    else
    {
        thread->finishedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        kzsErrorTest(thread->finishedEvent != NULL, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to create event for thread finish notification");
    }

    thread->threadRunner = threadRunner;
    thread->userData = userData;
    thread->deleteAutomatically = deleteAutomatically;
    thread->exitResult = KZS_ERROR_THREAD_NOT_FINISHED;

    result = ResumeThread(thread->handle);
    kzsErrorTest(result != -1, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to start thread");

    *out_thread = thread;
    kzsSuccess();
}

kzsError kzsThreadDelete(struct KzsThread* thread)
{
    if (thread->finishedEvent != KZ_NULL)
    {
        BOOL result = CloseHandle(thread->finishedEvent);
        kzsErrorTest(result != 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to delete event of thread finish notification");
    }

    kzsFree(thread);

    kzsSuccess();
}

kzsError kzsThreadJoin(const struct KzsThread* thread)
{
    DWORD result;

    kzsErrorTest(thread->finishedEvent != KZ_NULL, KZS_ERROR_THREAD_OPERATION_FAILED, "Cannot join a thread with automatic deletion");

    result = WaitForSingleObject(thread->finishedEvent, INFINITE);
    kzsErrorTest(result != WAIT_FAILED, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to join thread");

    kzsSuccess();
}

kzsError kzsThreadGetExitResult(const struct KzsThread* thread)
{
    return thread->exitResult;
}

kzBool kzsThreadIsCurrent(const struct KzsThread* thread)
{
    return thread->handle == GetCurrentThread();
}

kzsError kzsThreadSleep(kzUint milliseconds)
{
    Sleep(milliseconds);

    kzsSuccess();
}


kzsError kzsThreadLockCreate(struct KzsThreadLock** out_threadLock)
{
    struct KzsThreadLock* threadLock;

    threadLock = kzsMalloc(sizeof(*threadLock));
    kzsErrorTest(threadLock != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Out of memory while creating thread lock");

    threadLock->mutex = CreateMutex(NULL, FALSE, NULL);
    kzsErrorTest(threadLock->mutex != NULL, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to create mutex");

    threadLock->event = CreateEvent(NULL, TRUE, FALSE, NULL);
    kzsErrorTest(threadLock->event != NULL, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to create event");

    threadLock->value = KZ_FALSE;

    *out_threadLock = threadLock;
    kzsSuccess();
}

kzsError kzsThreadLockDelete(struct KzsThreadLock* threadLock)
{
    BOOL result;
    
    result = CloseHandle(threadLock->mutex);
    kzsErrorTest(result != 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to delete mutex");

    result = CloseHandle(threadLock->event);
    kzsErrorTest(result != 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to delete event");

    kzsFree(threadLock);

    kzsSuccess();
}

kzsError kzsThreadLockAcquire(struct KzsThreadLock* threadLock)
{
    DWORD result;
    
    result = WaitForSingleObject(threadLock->mutex, INFINITE);
    kzsErrorTest(result != WAIT_FAILED, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to lock mutex");
#if KZ_THREAD_DEBUG
    printf("Locked mutex %p\n", threadLock->mutex);
#endif

    kzsSuccess();
}

kzsError kzsThreadLockRelease(struct KzsThreadLock* threadLock)
{
    BOOL result;
    
    result = ReleaseMutex(threadLock->mutex);
    kzsErrorTest(result != 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to unlock mutex");
#if KZ_THREAD_DEBUG
    printf("Released mutex %p\n", threadLock->mutex);
#endif

    kzsSuccess();
}

kzsError kzsThreadLockWait(struct KzsThreadLock* threadLock, kzBool autoLock)
{
    DWORD result;
    BOOL result2;

    if (autoLock)
    {
        result = WaitForSingleObject(threadLock->mutex, INFINITE);
        kzsErrorTest(result != WAIT_FAILED, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to lock mutex");
#if KZ_THREAD_DEBUG
        printf("Locked mutex %p\n", threadLock->mutex);
#endif
    }

    /* This function assumes that the mutex is already locked by this thread. */
    while (!threadLock->value)
    {
        /* Release the lock. */
        result2 = ReleaseMutex(threadLock->mutex);
        kzsErrorTest(result2 != 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to release mutex");
#if KZ_THREAD_DEBUG
        printf("Released mutex %p\n", threadLock->mutex);
#endif

        /* Wait for an event. */
        result = WaitForSingleObject(threadLock->event, INFINITE);
        kzsErrorTest(result != WAIT_FAILED, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to wait for event");

        /* Lock again. */
        result = WaitForSingleObject(threadLock->mutex, INFINITE);
        kzsErrorTest(result != WAIT_FAILED, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to lock mutex");
#if KZ_THREAD_DEBUG
        printf("Locked mutex %p\n", threadLock->mutex);
#endif
    }

    if (autoLock)
    {
        result2 = ReleaseMutex(threadLock->mutex);
        kzsErrorTest(result2 != 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to release mutex");
#if KZ_THREAD_DEBUG
        printf("Released mutex %p\n", threadLock->mutex);
#endif
    }

    kzsSuccess();
}
kzsError kzsThreadLockWaitAndReset(struct KzsThreadLock* threadLock, kzBool autoLock)
{
    kzsError result;
    DWORD result1;
    BOOL result2;

    if (autoLock)
    {
        result1 = WaitForSingleObject(threadLock->mutex, INFINITE);
        kzsErrorTest(result1 != WAIT_FAILED, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to lock mutex");
#if KZ_THREAD_DEBUG
        printf("Locked mutex %p\n", threadLock->mutex);
#endif
    }

    result = kzsThreadLockWait(threadLock, KZ_FALSE);
    kzsErrorForward(result);

    result = kzsThreadLockSet(threadLock, KZ_FALSE, KZ_FALSE);
    kzsErrorForward(result);

    if (autoLock)
    {
        result2 = ReleaseMutex(threadLock->mutex);
        kzsErrorTest(result2 != 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to release mutex");
#if KZ_THREAD_DEBUG
        printf("Released mutex %p\n", threadLock->mutex);
#endif
    }

    kzsSuccess();
}

kzsError kzsThreadLockSet(struct KzsThreadLock* threadLock, kzBool value, kzBool autoLock)
{
    DWORD result;
    BOOL result2;

    if (autoLock)
    {
        result = WaitForSingleObject(threadLock->mutex, INFINITE);
        kzsErrorTest(result != WAIT_FAILED, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to lock mutex");
#if KZ_THREAD_DEBUG
        printf("Locked mutex %p\n", threadLock->mutex);
#endif
    }
    
    /* Set the lock value. */
    threadLock->value = value;
    if (value)
    {
        result2 = SetEvent(threadLock->event);
        kzsErrorTest(result2 != 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to set event");
    }
    else
    {
        result2 = ResetEvent(threadLock->event);
        kzsErrorTest(result2 != 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to reset event");
    }

    if (autoLock)
    {
        result2 = ReleaseMutex(threadLock->mutex);
        kzsErrorTest(result2 != 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to release mutex");
#if KZ_THREAD_DEBUG
        printf("Released mutex %p\n", threadLock->mutex);
#endif
    }
    
    kzsSuccess();
}

kzsError kzsThreadLockIsSet(struct KzsThreadLock* threadLock, kzBool autoLock, kzBool* out_isSet)
{
    DWORD result;
    BOOL result2;
    kzBool value;

    if (autoLock)
    {
        result = WaitForSingleObject(threadLock->mutex, INFINITE);
        kzsErrorTest(result != WAIT_FAILED, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to lock mutex");
#if KZ_THREAD_DEBUG
        printf("Locked mutex %p\n", threadLock->mutex);
#endif
    }

    value = threadLock->value;

    if (autoLock)
    {
        result2 = ReleaseMutex(threadLock->mutex);
        kzsErrorTest(result2 != 0, KZS_ERROR_THREAD_OPERATION_FAILED, "Unable to release mutex");
#if KZ_THREAD_DEBUG
        printf("Released mutex %p\n", threadLock->mutex);
#endif
    }

    *out_isSet = value;
    kzsSuccess();
}
