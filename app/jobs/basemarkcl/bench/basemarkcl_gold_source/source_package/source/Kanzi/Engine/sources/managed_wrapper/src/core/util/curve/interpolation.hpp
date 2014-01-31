#pragma once

#include "macros.hpp"
#include "util.hpp"

extern "C" {
#include <user/animation/kzu_animation_key.h>
#include <core/util/curve/kzc_interpolation.h>
}


KANZI_ENGINE_NAMESPACE_BEGIN

namespace User 
{
    namespace Animation
    {
        public ref class Interpolation
        {
        public:
            static kzFloat Linear(kzFloat t, kzFloat value1, kzFloat value2, kzFloat t0, kzFloat t1);
            static kzFloat Bezier(kzFloat t, kzFloat value1, kzFloat value2, kzFloat t0, kzFloat t1, kzFloat outTangentX, kzFloat outTangentY, kzFloat inTangentX, kzFloat inTangentY);
            static kzFloat Hermite(kzFloat t, kzFloat value1, kzFloat value2, kzFloat t0, kzFloat t1, kzFloat outTangentX, kzFloat outTangentY, kzFloat inTangentX, kzFloat inTangentY);
            
            static kzFloat SmoothStep(kzFloat edge1, kzFloat edge2, kzFloat t);
        };
    }
}
KANZI_ENGINE_NAMESPACE_END