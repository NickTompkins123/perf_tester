#pragma once

#include "macros.hpp"
#include "util.hpp"

extern "C" {
#include <core/util/io/kzc_input_stream.h>
}


KANZI_ENGINE_NAMESPACE_BEGIN

// Forward declarations
namespace Core
{
    namespace Memory
    {
        ref class MemoryManager;
    }
}


namespace Core 
{
    namespace Util 
    {
        namespace IO
        {
            public ref class InputStream : KanziWrapper<KzcInputStream>
            {
            public:
                virtual ~InputStream();

                void ReadBytes(unsigned int numBytes, array<unsigned char>^ out_bytes);

            internal:
                InputStream(KzcInputStream* native);
            };
        }
    }
}
KANZI_ENGINE_NAMESPACE_END