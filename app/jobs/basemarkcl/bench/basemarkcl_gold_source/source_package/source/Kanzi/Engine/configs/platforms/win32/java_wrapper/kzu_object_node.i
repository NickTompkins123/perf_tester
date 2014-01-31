%{
#include <user/scene_graph/kzu_object.h>
#include <user/project/kzu_project_loader_object_node.h>
#include <core/util/math/kzc_vector3.h>
%}

%nodefaultctor KzuObjectNode;
%nodefaultdtor KzuObjectNode;

struct KzuObjectNode {};

checkErrorOut(kzuObjectNodeLoadFromProject)
%inline %{
    struct KzuObjectNode* kzuObjectNodeLoadFromProject(struct KzuProject* project, struct KzuBinaryDirectory* directory, kzString path, kzsError* out_error) {
        struct KzuObjectNode* objectNode;
        *out_error = kzuProjectLoaderLoadObjectNode(project, directory, path, &objectNode);
        return objectNode;
    }
%}

%include <user/scene_graph/kzu_object.h>
%include <user/project/kzu_project_loader_object_node.h>
