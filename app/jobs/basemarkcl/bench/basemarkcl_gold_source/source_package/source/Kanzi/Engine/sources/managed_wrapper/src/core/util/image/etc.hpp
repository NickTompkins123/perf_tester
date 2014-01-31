#pragma once

#include "macros.hpp"
#include "util.hpp"

extern "C" {
#include <core/util/image/kzc_etc.h>
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
        namespace Image
        {
            public enum class ETCCompressionMode
            {
                FAST = 0,
                MEDIUM = 1,
                SLOW = 2,
                FAST_PERCEPTUAL = 3,
                MEDIUM_PERCEPTUAL = 4,
                SLOW_PERCEPTUAL = 5
            };

            public ref class ETC
            {
            public:
                static array<unsigned char>^ CompressImage(
                    Core::Memory::MemoryManager^ memoryManager, 
                    array<unsigned char>^ compressedImageData, 
                    kzInt width, 
                    kzInt height,
                    ETCCompressionMode compressionMode);
                
                static array<unsigned char>^ ETC::UncompressImage(
                    Core::Memory::MemoryManager^ memoryManager, 
                    array<unsigned char>^ compressedImageData, 
                    kzInt width, 
                    kzInt height);

                static kzUint ImageDataSize(kzUint width, kzUint height);
            };
        }
    }
}
KANZI_ENGINE_NAMESPACE_END