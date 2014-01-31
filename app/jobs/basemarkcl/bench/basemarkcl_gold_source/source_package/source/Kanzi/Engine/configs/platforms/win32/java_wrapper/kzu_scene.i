%{
#include <user/scene_graph/kzu_scene.h>
#include <user/project/kzu_project_loader_scene.h>
%}

%nodefaultctor KzuScene;
%nodefaultdtor KzuScene;

struct KzuScene {};

checkErrorOut(kzuSceneLoadFromProject)
%ignore kzuSceneCreate;
%inline %{
    struct KzuScene* kzuSceneLoadFromProject(struct KzuProject* project, struct KzuBinaryDirectory* directory, kzString path, kzsError* out_error) {
        struct KzuScene* scene;
        *out_error = kzuProjectLoaderLoadScene(project, directory, path, &scene);
        return scene;
    }
%}

%include <system/input/kzs_input.h>
%include <user/scene_graph/kzu_scene.h>
