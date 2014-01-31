#include "util.hpp"


KANZI_ENGINE_NAMESPACE_BEGIN

EngineException::EngineException(kzsError innerError)
{
    this->innerError = innerError;
}

kzsError EngineException::GetError()
{
    return innerError;
}

void Utilities::WrapError(kzsError error)
{
    if (error != KZS_SUCCESS)
    {
        throw gcnew EngineException(error);
    }
}

KANZI_ENGINE_NAMESPACE_END
