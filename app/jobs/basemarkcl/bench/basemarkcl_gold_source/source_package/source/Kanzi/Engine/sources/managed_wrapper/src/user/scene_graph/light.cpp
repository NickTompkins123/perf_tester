#include "light.hpp"

#include "user/project/project.hpp"

KANZI_ENGINE_NAMESPACE_BEGIN
namespace User 
{
    namespace SceneGraph 
    {
        Light::Light(Core::Memory::MemoryManager^ memoryManager) : ObjectNode(memoryManager, KZU_OBJECT_TYPE_LIGHT, KZ_NULL)
        {
            KzuLight* native;
            /* TODO: Pass light property as parameter */
            Utilities::WrapError(kzuLightCreate(memoryManager->GetNative(), KZ_NULL, &native));
            this->native = native;
        }

        Light::~Light()
        {
        }
    }
}
KANZI_ENGINE_NAMESPACE_END

