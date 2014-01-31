%{
#include <user/binary/kzu_binary_loader.h>
%}

%nodefaultctor KzuBinaryLoader;
%nodefaultdtor KzuBinaryLoader;

struct KzuBinaryLoader {};

checkErrorOut(kzuBinaryLoaderCreateFromConfig_temp)
%rename kzuBinaryLoaderCreateFromConfig_temp kzuBinaryLoaderCreateFromConfig;
%ignore kzuBinaryLoaderCreateFromConfig;
%inline %{
struct KzuBinaryLoader* kzuBinaryLoaderCreateFromConfig_temp(struct KzcMemoryManager* memoryManager, kzString configPath, kzsError* out_error) {
    struct KzuBinaryLoader* binaryLoader;
    *out_error = kzuBinaryLoaderCreateFromConfig(memoryManager, configPath, &binaryLoader);
    return binaryLoader;
}
%}

%include <user/binary/kzu_binary_loader.h>
