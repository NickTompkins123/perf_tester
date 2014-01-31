#pragma once

#include "macros.hpp"
#include "util.hpp"

extern "C" {
#include <core/memory/kzc_memory_manager.h>
}

KANZI_ENGINE_NAMESPACE_BEGIN
namespace Core
{
    namespace Memory
    {
        public ref class MemoryManager : KanziWrapper<KzcMemoryManager>
        {
        public:
            virtual ~MemoryManager();

        internal:
            static KzcMemoryManager* GetNativeMemoryManager(void* pointer);

        protected:
            MemoryManager();
        };


        public ref class SystemMemoryManager : MemoryManager
        {
        public:
            SystemMemoryManager();
        };


        public ref class PooledMemoryManager : MemoryManager
        {
        public:
            PooledMemoryManager(MemoryManager^ parentMemoryManager, int numPools, int poolSize);
        };


        public ref class QuickMemoryManager : MemoryManager
        {
        public:
            QuickMemoryManager(MemoryManager^ parentMemoryManager, unsigned int size);
        };
    }
}
KANZI_ENGINE_NAMESPACE_END

