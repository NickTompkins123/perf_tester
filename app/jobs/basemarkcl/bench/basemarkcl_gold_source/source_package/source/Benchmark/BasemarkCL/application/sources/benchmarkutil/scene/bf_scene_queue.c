#include "bf_scene_queue.h"

#include <benchmarkutil/bf_benchmark_framework.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_queue.h>


struct BfSceneQueue
{
    struct KzcQueue* tests; /**< Active tests in queue. */
    enum BfSceneQueueMode playMode; /**< Playback mode of the queue. */
};


kzsError bfSceneQueueCreate(const struct KzcMemoryManager* memoryManager, struct BfSceneQueue** out_queue)
{
    kzsError result;
    struct BfSceneQueue* queue;

    result = kzcMemoryAllocVariable(memoryManager, queue, "Scene queue");
    kzsErrorForward(result);

    result = kzcQueueCreate(memoryManager, &queue->tests);
    kzsErrorForward(result);

    queue->playMode = BF_SCENE_QUEUE_ONCE;

    *out_queue = queue;
    kzsSuccess();
}

kzsError bfSceneQueueDelete(struct BfSceneQueue* queue)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(queue));

    /* TODO: Uninitialize all scenes in queue. */

    result = kzcQueueDelete(queue->tests);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(queue);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError bfSceneQueueUpdate(const struct BfSceneQueue* queue, struct BenchmarkFramework* framework, kzUint deltaTime,
                            const struct BfReportLogger* reportLogger, struct BfScene** out_executedScene, kzBool* out_currentSceneFinished, kzBool* out_finished)
{
    kzsError result;
    struct BfScene* currentTest;
    kzBool complete = KZ_FALSE;
    kzBool currentSceneFinished;

    kzsAssert(kzcIsValidPointer(queue));
    KZ_UNUSED_PARAMETER(reportLogger);

    result = kzcQueuePeek(queue->tests, &currentTest);
    kzsErrorForward(result);

    result = bfSceneUpdate(framework, deltaTime, currentTest, &currentSceneFinished);
    kzsErrorForward(result);

    if(currentSceneFinished)
    {
        kzUint queueSize;

        result = kzcQueueReceive(queue->tests, &currentTest);
        kzsErrorForward(result);

        if(queue->playMode == BF_SCENE_QUEUE_REPEAT)
        {
            result = kzcQueueAdd(queue->tests, currentTest);
            kzsErrorForward(result);
        }

        queueSize = kzcQueueGetSize(queue->tests);
        if(queueSize == 0)
        {
            complete = KZ_TRUE;
        }
    }

    *out_currentSceneFinished = currentSceneFinished;
    *out_finished = complete;
    *out_executedScene = currentTest;
    kzsSuccess();
}

kzsError bfSceneQueueGetCurrentScene(const struct BfSceneQueue* queue, struct BfScene** out_scene)
{
    kzsError result;
    struct BfScene* scene = KZ_NULL;
    kzsAssert(kzcIsValidPointer(queue));
    
    if(kzcQueueGetSize(queue->tests) > 0)
    {
        result = kzcQueuePeek(queue->tests, &scene);
        kzsErrorForward(result);
    }

    *out_scene = scene;
    kzsSuccess();
}

kzsError bfSceneQueueAdd(const struct BfSceneQueue* queue, struct BfScene* test)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(queue));

    result = kzcQueueAdd(queue->tests, test);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError bfSceneQueueClear(struct BenchmarkFramework* framework, struct BfSceneQueue* queue)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(queue));

    while(!kzcQueueIsEmpty(queue->tests))
    {
        struct BfScene* test;
        result = kzcQueueReceive(queue->tests, &test);
        kzsErrorForward(result);
        result = bfSceneDelete(test);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

void bfSceneSetRepeatMode(struct BfSceneQueue* queue, enum BfSceneQueueMode mode)
{
    kzsAssert(kzcIsValidPointer(queue));
    queue->playMode = mode;
}

enum BfSceneQueueMode bfSceneGetRepeatMode(const struct BfSceneQueue* queue)
{
    kzsAssert(kzcIsValidPointer(queue));
    return queue->playMode;
}
