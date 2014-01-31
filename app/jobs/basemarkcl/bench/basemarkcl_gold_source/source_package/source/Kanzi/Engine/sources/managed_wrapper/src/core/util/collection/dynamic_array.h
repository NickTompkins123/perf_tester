#pragma once

using namespace System;
using namespace System::Runtime::InteropServices;

extern "C" {
#include <system/debug/kzs_error.h>
#include <core/util/collection/kzc_dynamic_array.h>
}

namespace Rightware
{
    namespace Kanzi 
    {
        namespace Engine 
        {
            namespace Core 
            {
                namespace Util 
                {
                    namespace Collection 
                    {
                        public ref class DynamicArray
                        {
                        public:
                            DynamicArray()
                            {
                                struct KzcDynamicArray* native2;
                            }

                            void add(Object^ object)
                            {
                                pin_ptr<void> element = &object;
                            }

                            int getSize()
                            {
                                return kzcDynamicArrayGetSize(native);
                            }

                        private:
                            struct KzcDynamicArray* native;
                        };
                    }
                }
            }
        }
    }
}

