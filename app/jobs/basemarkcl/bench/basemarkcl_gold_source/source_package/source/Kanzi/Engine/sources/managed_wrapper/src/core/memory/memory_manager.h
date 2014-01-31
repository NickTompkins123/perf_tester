#pragma once

using namespace System;
using namespace System::Runtime::InteropServices;

extern "C" {
#include <system/debug/kzs_error.h>
#include <core/memory/kzc_memory_manager.h>
}

namespace Rightware
{
    namespace Kanzi 
    {
        namespace Engine 
        {
            namespace Core
            {
                namespace Memory
                {
                    public ref class MemoryManager
                    {
                    public:
                        virtual ~MemoryManager()
                        {
                            if (native != KZ_NULL)
                            {
                                kzcMemoryManagerDelete(native);
                            }
                        }

                        KzcMemoryManager* GetNative()
                        {
                            return native;
                        }

                    protected:
                        MemoryManager()
                        {
                        }

                        void SetNative(KzcMemoryManager* native)
                        {
                            this->native = native;
                        }

                    private:
                        KzcMemoryManager* native;
                    };

                    public ref class SystemMemoryManager : MemoryManager
                    {
                    public:
                        SystemMemoryManager()
                        {
                            struct KzcMemoryManager* native;
                            kzcMemoryManagerCreateSystemManager(&native);
                            SetNative(native);
                        }
                    };

                    public ref class PooledMemoryManager : MemoryManager
                    {
                    public:
                        PooledMemoryManager(MemoryManager^ parentMemoryManager, int numPools, int poolSize)
                        {
                            struct KzcMemoryManager* native;
                            kzcMemoryManagerCreatePooled(parentMemoryManager->GetNative(), numPools, poolSize, &native);
                            SetNative(native);
                        }
                    };

                    public ref class QuickMemoryManager : MemoryManager
                    {
                    public:
                        QuickMemoryManager(MemoryManager^ parentMemoryManager, unsigned int size)
                        {
                            struct KzcMemoryManager* native;
                            kzcMemoryManagerCreateQuickArea(parentMemoryManager->GetNative(), size, &native);
                            SetNative(native);
                        }
                    };

                }
            }
        }
    }
}
