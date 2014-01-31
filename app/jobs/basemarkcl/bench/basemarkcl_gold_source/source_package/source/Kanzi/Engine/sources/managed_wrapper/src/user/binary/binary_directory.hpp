#pragma once

#include "macros.hpp"
#include "util.hpp"

extern "C" {
#include <user/binary/kzu_binary_directory.h>
}


using namespace System::Collections::Generic;


KANZI_ENGINE_NAMESPACE_BEGIN

// Forward declarations
namespace Core
{
    namespace Memory
    {
        ref class MemoryManager;
    }

    namespace Util
    {
        namespace IO
        {
            ref class InputStream;
        }
    }
}


namespace User 
{
    namespace Binary
    {
        public ref class BinarySource : KanziWrapper<KzuBinarySource>
        {
        public:
            virtual ~BinarySource();

            String^ GetReferencedFilePath(unsigned int index);

        internal:
            BinarySource(KzuBinarySource* native);
        };

        public ref class BinaryFileInfo : KanziWrapper<KzuBinaryFileInfo>
        {
        public:
            enum class FileType
            {
                FILE_TYPE_BINARY = KZU_BINARY_FILE_TYPE_BINARY
            };

            virtual ~BinaryFileInfo();

            String^ GetName();
            BinarySource^ GetSource();
            unsigned int GetSize();
            FileType GetFileType();

        internal:
            BinaryFileInfo(KzuBinaryFileInfo* native);
        };

        public ref class BinaryFolderInfo : KanziWrapper<KzuBinaryFolderInfo>
        {
        public:
            virtual ~BinaryFolderInfo();

            String^ GetName();
            array<BinaryFolderInfo^>^ GetSubFolders();
            array<BinaryFileInfo^>^ GetFiles();

        internal:
            BinaryFolderInfo(KzuBinaryFolderInfo* native);
        };

        public ref class BinaryDirectory : KanziWrapper<KzuBinaryDirectory>
        {
        public:
            BinaryDirectory(Core::Memory::MemoryManager^ memoryManager, array<Byte>^ buffer);
            virtual ~BinaryDirectory();

            void Merge(BinaryDirectory^ sourceDirectory);

            BinaryFolderInfo^ GetRootFolder();

            unsigned int GetFlags();

            IEnumerable<String^>^ GetDeletedFiles();

            Core::Util::IO::InputStream^ OpenFileWithReferences(Core::Memory::MemoryManager^ memoryManager,
                                                                BinaryFileInfo^ file,
                                                                array<unsigned int>^% out_referencePositions);
        };
    }
}
KANZI_ENGINE_NAMESPACE_END