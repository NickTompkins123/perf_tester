#pragma once

#include "macros.hpp"
#include "util.hpp"
#include "user/scene_graph/object_node.hpp"

extern "C" {
#include <user/scene_graph/kzu_light.h>
#include <system/debug/kzs_error.h>
}

KANZI_ENGINE_NAMESPACE_BEGIN
// Forward declarations.
namespace User
{
    namespace Engine
    {
        ref class Engine;
    }
}
// Class definition.
namespace User 
{
    namespace SceneGraph 
    {
        public ref class Light : ObjectNode
        {
        public:
            Light(Core::Memory::MemoryManager^ memoryManager);
            virtual ~Light();
        };
    }
}
KANZI_ENGINE_NAMESPACE_END
