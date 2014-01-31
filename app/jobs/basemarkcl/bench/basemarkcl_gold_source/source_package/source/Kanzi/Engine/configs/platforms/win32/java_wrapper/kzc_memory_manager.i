// Memory manager
// Wrapped as Java class
%{
#include <core/memory/kzc_memory_manager.h>
#include <core/memory/kzc_memory_system.h>
%}

%nodefaultctor KzcMemoryManager;
%nodefaultdtor KzcMemoryManager;

struct KzcMemoryManager {};

checkErrorOut(KzcMemoryManager::KzcMemoryManager)
%extend KzcMemoryManager {
    KzcMemoryManager(kzsError* out_error) {
        struct KzcMemoryManager* memoryManager;
        *out_error = kzcMemoryManagerCreateSystemManager(&memoryManager);
        return memoryManager;
    }
    
    ~KzcMemoryManager() {
        kzcMemoryManagerDelete(self);
    }
}
