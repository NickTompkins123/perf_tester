#include "input_stream.hpp"

#include "util.hpp"

extern "C" {
#include <core/renderer/kzc_renderer.h>
}

KANZI_ENGINE_NAMESPACE_BEGIN
namespace Core
{
    namespace Util
    {
        namespace IO
        {
            InputStream::InputStream(KzcInputStream* native)
            {
                this->native = native;
            }

            InputStream::~InputStream()
            {
                Utilities::WrapError(kzcInputStreamDelete(native));
            }

            void InputStream::ReadBytes(unsigned int numBytes, array<unsigned char>^ out_bytes)
            {
                pin_ptr<unsigned char> nativeBytes = &out_bytes[0];
                Utilities::WrapError(kzcInputStreamReadBytes(native, numBytes, nativeBytes));
            }
        }
    }
}
KANZI_ENGINE_NAMESPACE_END
