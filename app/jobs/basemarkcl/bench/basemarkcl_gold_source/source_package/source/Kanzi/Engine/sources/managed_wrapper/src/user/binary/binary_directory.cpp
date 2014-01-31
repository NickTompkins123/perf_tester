#include "binary_directory.hpp"

#include "util.hpp"

#include "core/memory/memory_manager.hpp"
#include "core/util/io/input_stream.hpp"

extern "C" {
#include <core/util/collection/kzc_hash_map.h>
#include <core/util/string/kzc_string.h>
#include <core/util/io/kzc_input_stream.h>
}


KANZI_ENGINE_NAMESPACE_BEGIN
namespace User
{
    namespace Binary
    {
        // Binary source
        BinarySource::BinarySource(KzuBinarySource* native)
        {
            this->native = native;
        }

        BinarySource::~BinarySource()
        {
        }

        String^ BinarySource::GetReferencedFilePath(unsigned int index)
        {
            String^ result = nullptr;
            kzString filePath = kzuBinarySourceGetReferencedFilePath(native, index);
            if (filePath != KZ_NULL)
            {
                unsigned int pathLength = kzcStringLength(filePath);
                array<unsigned char>^ filePathBytes = gcnew array<unsigned char>(pathLength);
                System::Runtime::InteropServices::Marshal::Copy((IntPtr)(unsigned char*)filePath, filePathBytes, 0, pathLength);
                result = Text::Encoding::UTF8->GetString(filePathBytes);
            }
            return result;
        }

        // Binary file info
        BinaryFileInfo::BinaryFileInfo(KzuBinaryFileInfo* native)
        {
            this->native = native;
        }

        BinaryFileInfo::~BinaryFileInfo()
        {
        }

        String^ BinaryFileInfo::GetName()
        {
            return gcnew String(kzuBinaryFileInfoGetName(native));
        }

        BinarySource^ BinaryFileInfo::GetSource()
        {
            return gcnew BinarySource(kzuBinaryFileInfoGetSource(native));
        }

        unsigned int BinaryFileInfo::GetSize()
        {
            return kzuBinaryFileInfoGetSize(native);
        }

        BinaryFileInfo::FileType BinaryFileInfo::GetFileType()
        {
            return (BinaryFileInfo::FileType)kzuBinaryFileInfoGetType(native);
        }


        // Binary folder info
        BinaryFolderInfo::BinaryFolderInfo(KzuBinaryFolderInfo* native)
        {
            this->native = native;
        }

        BinaryFolderInfo::~BinaryFolderInfo()
        {
        }

        String^ BinaryFolderInfo::GetName()
        {
            return gcnew String(kzuBinaryFolderInfoGetName(native));
        }

        array<BinaryFolderInfo^>^ BinaryFolderInfo::GetSubFolders()
        {
            KzcHashMap* subFolders = kzuBinaryFolderInfoGetSubFolders(native);
            array<BinaryFolderInfo^>^ list = gcnew array<BinaryFolderInfo^>(kzcHashMapGetSize(subFolders));
            KzcHashMapIterator it = kzcHashMapGetIterator(subFolders);
            for (unsigned int i = 0; kzcHashMapIterate(it); ++i)
            {
                KzuBinaryFolderInfo* value = (KzuBinaryFolderInfo*)kzcHashMapIteratorGetValue(it);
                list[i] = gcnew BinaryFolderInfo(value);
            }
            return list;
        }

        array<BinaryFileInfo^>^ BinaryFolderInfo::GetFiles()
        {
            KzcHashMap* files = kzuBinaryFolderInfoGetFiles(native);
            array<BinaryFileInfo^>^ list = gcnew array<BinaryFileInfo^>(kzcHashMapGetSize(files));
            KzcHashMapIterator it = kzcHashMapGetIterator(files);
            for (unsigned int i = 0; kzcHashMapIterate(it); ++i)
            {
                KzuBinaryFileInfo* value = (KzuBinaryFileInfo*)kzcHashMapIteratorGetValue(it);
                list[i] = gcnew BinaryFileInfo(value);
            }
            return list;
        }


        // Binary directory
        BinaryDirectory::BinaryDirectory(Core::Memory::MemoryManager^ memoryManager, array<Byte>^ buffer)
        {
            // Create a copy of the data into a native buffer to keep the data permanently pinned.
            // TODO: This buffer should be freed after it's no longer used
            unsigned char* bufferNative;
            kzcMemoryAllocPointer(memoryManager->GetNative(), &bufferNative, buffer->Length, "Wrapper::BinaryDirectory Native memory buffer.");
            ::System::Runtime::InteropServices::Marshal::Copy(buffer, 0, (IntPtr)bufferNative, buffer->Length);

            KzuBinaryDirectory* native;
            Utilities::WrapError(kzuBinaryDirectoryCreateFromMemory(memoryManager->GetNative(), bufferNative, buffer->Length, &native));
            this->native = native;
        }

        BinaryDirectory::~BinaryDirectory()
        {
            Utilities::WrapError(kzuBinaryDirectoryDelete(native));
        }

        void BinaryDirectory::Merge(BinaryDirectory^ sourceDirectory)
        {
            Utilities::WrapError(kzuBinaryDirectoryMerge(native, sourceDirectory->native, KZ_NULL));
        }

        unsigned int BinaryDirectory::GetFlags()
        {
            return kzuBinaryDirectoryGetFlags(native);
        }

        BinaryFolderInfo^ BinaryDirectory::GetRootFolder()
        {
            return gcnew BinaryFolderInfo(kzuBinaryDirectoryGetRootFolder(native));
        }

        IEnumerable<String^>^ BinaryDirectory::GetDeletedFiles()
        {
            struct KzcHashSetIterator it = kzuBinaryDirectoryGetDeletedFiles(native);
            List<String^>^ deletedFiles = gcnew List<String^>();
            while (kzcHashSetIterate(it))
            {
                deletedFiles->Add(gcnew String((kzString)kzcHashSetIteratorGetValue(it)));
            }
            return deletedFiles;
        }

        Core::Util::IO::InputStream^ BinaryDirectory::OpenFileWithReferences(Core::Memory::MemoryManager^ memoryManager,
                                                                             BinaryFileInfo^ file,
                                                                             array<unsigned int>^% out_referencePositions)
        {
            unsigned int* referencePositions;
            KzcInputStream* inputStream;
            Utilities::WrapError(kzuBinaryDirectoryOpenFileWithReferences(memoryManager->GetNative(), file->GetNative(),
                                                                          &referencePositions, KZC_IO_STREAM_ENDIANNESS_UNSPECIFIED,
                                                                          &inputStream));
            unsigned int length = kzcArrayLength(referencePositions);
            out_referencePositions = gcnew array<unsigned int>(length);
            for (unsigned int i = 0; i < length; ++i)
            {
                out_referencePositions[i] = referencePositions[i];
            }
            Utilities::WrapError(kzcMemoryFreeArray(referencePositions));
            return gcnew Core::Util::IO::InputStream(inputStream);
        }
    }
}
KANZI_ENGINE_NAMESPACE_END
