#include "interpolation.hpp"
#include "util.hpp"

KANZI_ENGINE_NAMESPACE_BEGIN
namespace User
{
    namespace Animation
    {
        kzFloat Interpolation::Linear(kzFloat t, kzFloat value1, kzFloat value2, kzFloat t0, kzFloat t1)
        {
            return kzuAnimationInterpolateLinear(t, value1, value2, t0, t1);
        }

        kzFloat Interpolation::Bezier(kzFloat t, kzFloat value1, kzFloat value2, kzFloat t0, kzFloat t1, kzFloat outTangentX, kzFloat outTangentY, kzFloat inTangentX, kzFloat inTangentY)
        {
            KzcVector2 o = {outTangentX, outTangentY};
            KzcVector2 i = {inTangentX, inTangentY};

            return kzuAnimationInterpolateBezier(t, value1, value2, t0, t1, &o, &i);
        }

        kzFloat Interpolation::Hermite(kzFloat t, kzFloat value1, kzFloat value2, kzFloat t0, kzFloat t1, kzFloat outTangentX, kzFloat outTangentY, kzFloat inTangentX, kzFloat inTangentY)
        {
            KzcVector2 o = {outTangentX, outTangentY};
            KzcVector2 i = {inTangentX, inTangentY};

            return kzuAnimationInterpolateHermite(t, value1, value2, t0, t1, &o, &i);
        }

        kzFloat Interpolation::SmoothStep(kzFloat edge1, kzFloat edge2, kzFloat t)
        {
            kzFloat result = t;

            // Kanzi engine smooth step algorithm requires that the edge 1 < t < edge2
            // Make sure that we fulfill this requirement.
            if(edge1 < edge2)
            {
                result = kzcInterpolateSmoothStep(edge1, edge2, kzcInterpolateLinear(edge1, edge2, t));
            }
            else if(edge1 > edge2)
            {
                result = kzcInterpolateSmoothStep(edge2, edge1, kzcInterpolateLinear(edge1, edge2, 1.0f-t));
            }

            return kzcInterpolateLinear(edge1, edge2, result);
        }
    }
}
KANZI_ENGINE_NAMESPACE_END
