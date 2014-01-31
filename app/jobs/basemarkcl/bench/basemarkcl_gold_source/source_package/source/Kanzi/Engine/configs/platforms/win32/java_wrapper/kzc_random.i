// Random
// Wrapped as static functions
%{
#include <core/util/math/kzc_random.h>
%}

checkErrorOut(kzcRandomCreate_temp)
%rename kzcRandomCreate_temp kzcRandomCreate;
%ignore kzcRandomCreate;
%inline %{
struct KzcRandom* kzcRandomCreate_temp(struct KzcMemoryManager* memoryManager, kzS32 seed, kzsError* out_error) {
    struct KzcRandom* random;
    *out_error = kzcRandomCreate(memoryManager, seed, &random);
    return random;
}
%}

checkErrorOut(kzcRandomCreateDefault_temp)
%rename kzcRandomCreateDefault_temp kzcRandomCreateDefault;
%ignore kzcRandomCreateDefault;
%inline %{
struct KzcRandom* kzcRandomCreateDefault_temp(struct KzcMemoryManager* memoryManager, kzsError* out_error) {
    struct KzcRandom* random;
    *out_error = kzcRandomCreateDefault(memoryManager, &random);
    return random;
}
%}

%ignore KzcRandom::seed_private;

%include <core/util/math/kzc_random.h>
