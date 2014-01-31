#pragma once

#include "util.h"
#include "core/memory/memory_manager.h"

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace Rightware::Kanzi::Engine::Core::Memory;
using namespace Rightware::Kanzi::Engine;

extern "C" {
#include <system/debug/kzs_error.h>
#include <user/scene_graph/kzu_mesh.h>
}

namespace Rightware
{
    namespace Kanzi 
    {
        namespace Engine 
        {
            namespace User 
            {
                namespace SceneGraph 
                {
                    public ref class Mesh
                    {
                    public:
                        Mesh(MemoryManager^ memoryManager)
                        {
                            KzuMesh* native2;
                            Util::WrapError(kzuMeshCreate(memoryManager->GetNative(), &native2));
                            this->native = native2;
                        }

                    private:
                        KzuMesh* native;
                    };
                }
            }
        }
    }
}