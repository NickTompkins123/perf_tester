#ifndef _BF_SCENE_QUEUE_
#define _BF_SCENE_QUEUE_


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/** Defines how scene queue is played. */
enum BfSceneQueueMode
{
    BF_SCENE_QUEUE_REPEAT, /**< Endlessly cycle the current scene queue. */
    BF_SCENE_QUEUE_ONCE /**< Go through the current scene queue once. This clears the queue */
};


/* Forward declarations. */
struct KzcMemoryManager;
struct BenchmarkFramework;
struct BfReportLogger;
struct BfScene;
struct Bf;

/**
 * \struct KzcQueue
 * Structure of a test queue.
 */
struct BfSceneQueue;


/** Creates the test queue. */
kzsError bfSceneQueueCreate(const struct KzcMemoryManager* memoryManager, struct BfSceneQueue** out_queue);
/** Deletes the test queue. */
kzsError bfSceneQueueDelete(struct BfSceneQueue* queue);

/** Pushes a test to queue. */
kzsError bfSceneQueueUpdate(const struct BfSceneQueue* queue, struct BenchmarkFramework* framework, kzUint deltaTime, 
                            const struct BfReportLogger* reportLogger, struct BfScene** out_executedScene, kzBool* out_currentSceneFinished, 
                            kzBool* out_queueFinished);

/** Gets current scene from queue. */
kzsError bfSceneQueueGetCurrentScene(const struct BfSceneQueue* queue, struct BfScene** out_scene);

/** Pushes a test to queue. */
kzsError bfSceneQueueAdd(const struct BfSceneQueue* queue, struct BfScene* test);

/** Clears the scene queue. */
kzsError bfSceneQueueClear(struct BenchmarkFramework* framework, struct BfSceneQueue* queue);

/** Sets playback mode. */
void bfSceneSetRepeatMode(struct BfSceneQueue* queue, enum BfSceneQueueMode mode);
/** Gets playback mode from scene queue. */
enum BfSceneQueueMode bfSceneGetRepeatMode(const struct BfSceneQueue* queue);


#endif
