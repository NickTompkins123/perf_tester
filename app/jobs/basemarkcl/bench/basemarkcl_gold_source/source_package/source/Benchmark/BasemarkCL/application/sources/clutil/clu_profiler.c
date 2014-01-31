/*
* Utility library for profiling OpenCL calls
* Copyright 2011 by Rightware. All rights reserved.
*/

#include "clu_profiler.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/debug/kzc_log.h>

#include <stdio.h>


kzsError cluProfilerCreate(const struct KzcMemoryManager* manager, size_t maxEvents, kzBool profilingEnabled, struct CluProfiler **out_profiler)
{
    struct CluProfiler *profiler;
    kzsError result;

    result = kzcMemoryAllocVariable(manager, profiler, "cluProfiler");
    kzsErrorForward(result);
    profiler->maxEvents = maxEvents;
    profiler->eventCount = 0;
    profiler->profilingEnabled = profilingEnabled;

    result = kzcMemoryAllocArray(manager, profiler->eventList, maxEvents, "List of cl_events for profiler");
    kzsErrorForward(result);
    result = kzcMemoryAllocArray(manager, profiler->stringList, maxEvents, "List description strings for profiler");
    kzsErrorForward(result);

    *out_profiler = profiler;
    kzsSuccess();
}

kzsError cluProfilerDelete(struct CluProfiler *profiler)
{
    kzsError result;
    result = kzcMemoryFreeArray(profiler->eventList);
    kzsErrorForward(result);
    result = kzcMemoryFreeArray(profiler->stringList);
    kzsErrorForward(result);
    result = kzcMemoryFreeVariable(profiler);
    kzsErrorForward(result);
    kzsSuccess();
}

kzsError cluProfilerLogEvents(struct CluProfiler *profiler)
{
    cl_ulong cQueued = 0;
    cl_ulong cSubmit = 0;
    cl_ulong cStart = 0;
    cl_ulong cEnd = 0;
    kzChar logstring[512];
    size_t i;
    size_t Events = profiler->eventCount;
    cl_int clResult;
    for(i = 0; i < Events; i++)
    {   
        clResult = clGetEventProfilingInfo(profiler->eventList[i], CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &cQueued, NULL);
        cluClErrorTest(clResult);
        clResult = clGetEventProfilingInfo(profiler->eventList[i], CL_PROFILING_COMMAND_SUBMIT, sizeof(cl_ulong), &cSubmit, NULL);
        cluClErrorTest(clResult);
        clResult = clGetEventProfilingInfo(profiler->eventList[i], CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &cStart, NULL);
        cluClErrorTest(clResult);
        clResult = clGetEventProfilingInfo(profiler->eventList[i], CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &cEnd, NULL);
        cluClErrorTest(clResult);
        clResult = clReleaseEvent(profiler->eventList[i]); /* Consider if it's wise to release the events here. As of now however this is good enough. */
        cluClErrorTest(clResult);
        sprintf(logstring, "PROFILING %s %llu %llu %llu %llu", profiler->stringList[i], cQueued, cSubmit, cStart, cEnd);
        kzsLog(KZS_LOG_LEVEL_INFO, (kzString)logstring);
    }
    profiler->eventCount = 0;
    kzsSuccess();
}


kzsError cluProfilerAddEvent_private(struct CluProfiler *profiler, kzString description, cl_event profilingEvent)
{
    if(profiler->eventCount < profiler->maxEvents)
    {  
        profiler->eventList[profiler->eventCount] = profilingEvent;
        profiler->stringList[profiler->eventCount] = description;
        profiler->eventCount += 1;
    }

    kzsSuccess();

}
