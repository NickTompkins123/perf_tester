#include "mesh_optimizer.hpp"

#include <nvtristrip.h>


KANZI_ENGINE_NAMESPACE_BEGIN

namespace Util
{
    void MeshOptimizer::EnableRestart(const unsigned int restartVal)
    {
        ::SetListsOnly(restartVal);
    }

    void MeshOptimizer::DisableRestart()
    {
        ::DisableRestart();
    }

    void MeshOptimizer::SetCacheSize(const unsigned int cacheSize)
    {
        ::SetCacheSize(cacheSize);
    }

    void MeshOptimizer::SetStitchStrips(const bool bStitchStrips)
    {
        ::SetStitchStrips(bStitchStrips);
    }

    void MeshOptimizer::SetListsOnly(bool listsOnly)
    {
        ::SetListsOnly(listsOnly);
    }

    void MeshOptimizer::SetMinStripSize(const unsigned int minSize)
    {
        ::SetMinStripSize(minSize);
    }

    bool MeshOptimizer::GenerateStrips()
    {
        return false;
    }

    bool MeshOptimizer::RemapIndices()
    {
        return false;
    }
}

KANZI_ENGINE_NAMESPACE_END
