#include "etc.hpp"

#include "util.hpp"
#include "core/memory/memory_manager.hpp"

KANZI_ENGINE_NAMESPACE_BEGIN
namespace Core
{
    namespace Util
    {
        namespace Image
        {
            array<unsigned char>^ ETC::CompressImage(
                Core::Memory::MemoryManager^ memoryManager, 
                array<unsigned char>^ imageData, 
                kzInt width, 
                kzInt height,
                ETCCompressionMode compressionMode) 
            {

                pin_ptr<unsigned char> nativeImageData = &imageData[0];

                int outputBufferSize = (int)kzcEtcGetCompressedBufferMemorySize(width, height);
                array<unsigned char>^ out_bytes = gcnew array<unsigned char>(outputBufferSize);
                pin_ptr<unsigned char> nativeDestinationBuffer = &out_bytes[0];

                kzUint bytesWritten;
                kzcEtcCompressImageToBuffer(memoryManager->GetNative(), nativeImageData, width, height, 
                                            nativeDestinationBuffer, (enum KzcEtcCompressionMode)compressionMode, &bytesWritten);

                return out_bytes;
            }

            array<unsigned char>^ ETC::UncompressImage(
                Core::Memory::MemoryManager^ memoryManager, 
                array<unsigned char>^ compressedImageData, 
                kzInt width, 
                kzInt height) 
            {

                pin_ptr<unsigned char> nativeCompressedImageData = &compressedImageData[0];

                int outputBufferSize = width * height * 3;
                array<unsigned char>^ out_bytes = gcnew array<unsigned char>(outputBufferSize);
                pin_ptr<unsigned char> nativeDestinationBuffer = &out_bytes[0];

                kzUint bytesWritten;
                kzcEtcUncompressImageToBuffer(memoryManager->GetNative(), nativeCompressedImageData, width, height, 
                                            width, height, nativeDestinationBuffer, &bytesWritten);

                return out_bytes;
            }

            kzUint ETC::ImageDataSize(kzUint width, kzUint height)
            {
                return kzcEtcGetCompressedBufferMemorySize(width, height);
            }
        }
    }
}
KANZI_ENGINE_NAMESPACE_END
