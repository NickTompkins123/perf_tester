%{
#include <user/project/kzu_project.h>
%}

%nodefaultctor KzuProject;
%nodefaultdtor KzuProject;

struct KzuProject {};

checkErrorOut(kzuProjectCreate_temp)
%rename kzuProjectCreate_temp kzuProjectCreate;
%ignore kzuProjectCreate;
%inline %{
struct KzuProject* kzuProjectCreate_temp(struct KzcMemoryManager* memoryManager, kzsError* out_error) {
    struct KzuProject* project;
    *out_error = kzuProjectCreate(memoryManager, &project);
    return project;
}
%}

%include <user/project/kzu_project.h>
