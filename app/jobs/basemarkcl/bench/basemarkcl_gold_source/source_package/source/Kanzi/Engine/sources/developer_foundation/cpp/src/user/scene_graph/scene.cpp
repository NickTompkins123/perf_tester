extern "C"
{
#include <user/scene_graph/kzu_scene.h>
#include <user/scene_graph/kzu_transformed_scene.h>
}

#include "user/scene_graph/scene.h"

namespace Kanzi {
namespace User {

class ScenePrivate
{
public:
    ScenePrivate(struct KzuScene* scene) : 
      scene(scene), destroyScene(KZ_FALSE) {}

    ScenePrivate(const ScenePrivate& p) : 
      scene(p.scene), destroyScene(p.destroyScene) {}

    bool destroyScene;
    KzuScene* scene;
};

Scene::Scene(const Scene& scene) :
    d(new ScenePrivate(*scene.d))
{
}

Scene::Scene(KzuScene* scene) : 
    d(new ScenePrivate(scene))
{   
}

Scene::~Scene()
{
    if (d->destroyScene)
    {
        kzuSceneDelete(d->scene);
    }
    delete d;
}

KzuScene* Scene::handle() const
{
    return d->scene;
}

} // Kanzi::User
} // Kanzi
