#pragma once

using namespace System;
using namespace System::Runtime::InteropServices;

extern "C" {
#include <system/debug/kzs_error.h>
#include <user/scene_graph/kzu_light.h>
}

namespace Rightware
{
    namespace Kanzi 
    {
        namespace Engine 
        {
            namespace User 
            {
                namespace SceneGraph 
                {
                    public ref class Light
                    {
                    public:
                        Light(float red, float green, float blue)
                        {
                            KzuLight* light2;
                            this->light = light2;
                        }

                    private:
                        KzuLight* light;
                    };
                }
            }
        }
    }
}