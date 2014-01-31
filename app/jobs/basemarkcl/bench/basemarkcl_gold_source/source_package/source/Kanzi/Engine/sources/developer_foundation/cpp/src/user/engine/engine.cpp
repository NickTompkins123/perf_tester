extern "C"
{
#include <user/engine/kzu_engine.h>
#include <user/renderer/kzu_renderer.h>
#include <user/scene_graph/kzu_scene.h>
#include <user/scene_graph/kzu_transformed_scene.h>

#include <user/scene_graph/kzu_camera.h>
#include <user/scene_graph/kzu_transformed_object.h>


#include <core/renderer/kzc_renderer.h>
}

#include "user/engine/engine.h"
#include "system/display/window.h"

namespace Kanzi {
namespace User {

class EnginePrivate
{
public:
    EnginePrivate(KzuEngine* engine) : engine(engine) {};
    KzuEngine* engine;
};

Engine::Engine(KzuEngine* engine) : 
    d(new EnginePrivate(engine))
{   
}

Engine::~Engine()
{
    delete d;
}

void Engine::setActiveWindow(System::Window* window)
{
    kzuEngineSetActiveWindow(d->engine, window->handle());
}

} // Kanzi::User
} // Kanzi
