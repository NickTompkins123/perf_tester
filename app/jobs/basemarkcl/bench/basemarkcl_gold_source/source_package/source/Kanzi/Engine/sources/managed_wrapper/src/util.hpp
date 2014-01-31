#pragma once

#include "macros.hpp"

extern "C" {
#include <core/util/color/kzc_color.h>
#include <system/debug/kzs_error.h>
}


KANZI_ENGINE_NAMESPACE_BEGIN
// Class definitions

public ref class EngineException : Exception
{
public:
    EngineException(kzsError innerError);
    kzsError GetError();

private:
    kzsError innerError;
};


public ref class Utilities
{
public:
    static void WrapError(kzsError error);
};

template <class T>
public ref class KanziWrapper
{
public:
    T* GetNative()
    {
        return native;
    }
protected:
    T* native;
};

KANZI_ENGINE_NAMESPACE_END
