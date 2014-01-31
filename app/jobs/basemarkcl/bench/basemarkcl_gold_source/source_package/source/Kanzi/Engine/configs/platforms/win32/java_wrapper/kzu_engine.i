%{
#include <user/engine/kzu_engine.h>
%}

checkError(kzuEngineRender)
checkError(kzuEngineStartUpdate)
checkError(kzuEngineEndUpdate)
checkError(kzuEngineExtractScene)

%nodefaultctor KzuEngine;
%nodefaultdtor KzuEngine;

struct KzuEngine {};

checkErrorOut(kzuEngineCreate_temp)
%rename kzuEngineCreate_temp kzuEngineCreate;
%ignore kzuEngineCreate;
%inline %{
struct KzuEngine* kzuEngineCreate_temp(struct KzcMemoryManager* memoryManager, kzsError* out_error) {
    struct KzuEngine* engine;
    *out_error = kzuEngineCreate(memoryManager, KZU_ENGINE_DEFAULT_QUICK_MEMORY_SIZE, &engine);
    return engine;
}
%}

%include <user/engine/kzu_engine.h>
