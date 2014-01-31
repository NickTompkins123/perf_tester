#pragma once

#include "macros.hpp"

extern "C" {
#include <system/debug/kzs_error.h>
}


KANZI_ENGINE_NAMESPACE_BEGIN
// Class definitions

namespace Util
{
    public ref class MeshOptimizer
    {
    public:
        static void EnableRestart(const unsigned int restartVal);
        static void DisableRestart();
        static void SetCacheSize(const unsigned int cacheSize);
        static void SetStitchStrips(const bool bStitchStrips);
        static void SetMinStripSize(const unsigned int minSize);
        static void SetListsOnly(bool listsOnly);
        
        // TODO: add managed arguments
        bool GenerateStrips();

        // TODO: add managed arguments
        bool RemapIndices();
    };
}

KANZI_ENGINE_NAMESPACE_END
