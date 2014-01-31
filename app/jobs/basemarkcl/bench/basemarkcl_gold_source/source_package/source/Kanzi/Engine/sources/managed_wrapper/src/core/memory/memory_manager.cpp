#include "memory_manager.hpp"

#include "util.hpp"

extern "C" {
#include <core/memory/kzc_memory_manager.h>
#include <core/memory/kzc_memory_system.h>
#include <core/memory/kzc_memory_pooled.h>
#include <core/memory/kzc_memory_quick.h>
#include <system/debug/kzs_error.h>
}

KANZI_ENGINE_NAMESPACE_BEGIN
namespace Core
{
    namespace Memory
    {
        MemoryManager::MemoryManager()
        {
        }

        MemoryManager::~MemoryManager()
        {
            if (native != KZ_NULL)
            {
                kzcMemoryManagerDelete(native);
            }
        }

        KzcMemoryManager* MemoryManager::GetNativeMemoryManager(void* pointer)
        {
            return kzcMemoryGetManager(pointer);
        }

        SystemMemoryManager::SystemMemoryManager()
        {
            struct KzcMemoryManager* native;
            kzcMemoryManagerCreateSystemManager(&native);
            this->native = native;
        }

        PooledMemoryManager::PooledMemoryManager(MemoryManager^ parentMemoryManager, int numPools, int poolSize)
        {
            struct KzcMemoryManager* native;
            kzcMemoryManagerCreatePooledManager(parentMemoryManager->GetNative(), numPools, poolSize, &native);
            this->native = native;
        }

        QuickMemoryManager::QuickMemoryManager(MemoryManager^ parentMemoryManager, unsigned int size)
        {
            struct KzcMemoryManager* native;
            kzcMemoryManagerCreateQuickManager(parentMemoryManager->GetNative(), size, &native);
            this->native = native;
        }
    }
}
KANZI_ENGINE_NAMESPACE_END

