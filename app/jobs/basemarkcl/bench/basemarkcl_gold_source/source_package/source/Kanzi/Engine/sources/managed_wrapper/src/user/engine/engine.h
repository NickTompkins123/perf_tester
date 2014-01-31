#pragma once

#include "core/memory/memory_manager.h"
#include "util.h"

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace Rightware::Kanzi::Engine::Core::Memory;
using namespace Rightware::Kanzi::Engine;

extern "C" {
#include <system/debug/kzs_error.h>
#include <user/engine/kzu_engine.h>
}


namespace Rightware
{
    namespace Kanzi 
    {
        namespace Engine 
        {
            namespace User
            {
                namespace Engine
                {
                    public ref class Engine
                    {
                    public:
                        Engine(MemoryManager^ memoryManager, MemoryManager^ quickMemoryManager)
                        {
                            KzuEngine* native2;
                            Util::WrapError(kzuEngineCreate(memoryManager->GetNative(), quickMemoryManager->GetNative(), &native2));
                            native = native2;
                        }

                        virtual ~Engine()
                        {
                            if (native != KZ_NULL)
                            {
                                Util::WrapError(kzuEngineDelete(native));
                            }
                        }

                    private:
                        KzuEngine* native;
                    };
                }
            }
        }
    }
}
