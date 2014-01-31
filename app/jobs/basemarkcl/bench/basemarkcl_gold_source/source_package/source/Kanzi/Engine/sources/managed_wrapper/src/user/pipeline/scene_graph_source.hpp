#pragma once

#include "macros.hpp"
#include "util.hpp"

extern "C" {
#include <user/filter/kzu_scene_graph_source.h>
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
    namespace Pipeline 
    {
        public ref class SceneGraphSource : KanziWrapper<KzuSceneGraphSource>
        {
        public:
            SceneGraphSource(User::Engine::Engine^ engine);
            virtual ~SceneGraphSource();
        private:
        };
    }
}
KANZI_ENGINE_NAMESPACE_END
