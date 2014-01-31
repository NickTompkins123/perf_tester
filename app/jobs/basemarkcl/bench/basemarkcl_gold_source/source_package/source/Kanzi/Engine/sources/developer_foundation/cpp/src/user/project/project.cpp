extern "C"
{
#include <user/project/kzu_project.h>
#include <user/project/kzu_project_loader.h>
#include <user/binary/kzu_binary_loader.h>
#include <user/binary/kzu_binary_directory.h>

#include <core/memory/kzc_memory_manager.h>
}

#include "user/project/project.h"
#include "user/scene_graph/scene.h"

namespace Kanzi {
namespace User {

class ProjectPrivate
{
public:
    ProjectPrivate(KzuProject* project, KzuBinaryDirectory* directory) : 
      project(project), directory(directory) {};

    KzuProject* project;
    KzuBinaryLoader* loader;
    KzuBinaryDirectory* directory;
};

Project::Project(KzuProject* project, KzuBinaryDirectory* directory) : 
    d(new ProjectPrivate(project, directory))
{   
}

Project::~Project()
{
    kzuBinaryLoaderDelete(d->loader);
    delete d;
}

void Project::clear()
{
    kzuProjectClear(d->project);
}

ScenePtr Project::scene(const char *scenePath)
{
    Scene* result = KZ_NULL;

    KzuScene* scene;
    if (kzuProjectLoaderLoadScene(d->project, scenePath, &scene) == KZS_SUCCESS)
    {
        result = new Scene(scene);
    }

    return ScenePtr(result);
}

kzBool Project::load(const char* filename)
{
    struct KzcMemoryManager* manager = kzcMemoryGetManager(d->project);

    kzBool result = KZ_FALSE;

    struct KzuBinaryDirectory* tempDirectory;

    if (kzuBinaryDirectoryCreateFromFileResource(manager, filename, &tempDirectory) == KZS_SUCCESS)
    {
        kzuBinaryDirectoryMerge(d->directory, tempDirectory, KZ_NULL);

        result = KZ_TRUE;
    }

    return result;
}

} // Kanzi::User
} // Kanzi

