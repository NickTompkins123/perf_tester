/**
* \file
* OpenCL Profiling macros
*
* Copyright 2011 by Rightware. All rights reserved.
*/
#ifndef CLU_PROFILER_H
#define CLU_PROFILER_H

#include "clu_opencl_base.h"

#include <clutil/clu_util.h>
#include <core/util/string/kzc_string.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>
#include <system/wrappers/kzs_memory.h>


struct CluProfiler 
{
    size_t maxEvents;
    size_t eventCount;
    kzBool profilingEnabled;
    kzString *stringList;
    cl_event *eventList;
};


kzsError cluProfilerCreate(const struct KzcMemoryManager* manager, size_t maxEvents, kzBool profilingEnabled, struct CluProfiler **out_profiler);

kzsError cluProfilerDelete(struct CluProfiler *profiler);

#define cluProfilerAddEvent(profiler_param, description_param, event_param) \
{ \
    if(profiler_param->profilingEnabled) {kzsError cResult = cluProfilerAddEvent_private(profiler_param, description_param, event_param); kzsErrorForward(cResult);} else {cl_int errResult = clReleaseEvent(event_param); cluClErrorTest(errResult);} \
}

kzsError cluProfilerAddEvent_private(struct CluProfiler *profiler, kzString description, cl_event profilingEvent);

kzsError cluProfilerLogEvents(struct CluProfiler *profiler);


#endif
