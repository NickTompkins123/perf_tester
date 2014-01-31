/**
 * \file
 * Application tool module.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "application.hpp"

using namespace Rightware::Kanzi::Applications::DefaultApplication::ToolModule;

extern "C"
{
#include <application/kza_application.h>
}

namespace Rightware {
namespace Examples {
namespace Custom_Toolmodule {
namespace ToolModule {

IEnumerable<PreviewOption^>^ ApplicationToolModule::GetPreviewOptions()
{
    List<PreviewOption^>^ debugOptions = gcnew List<PreviewOption^>();

    debugOptions->Add(gcnew PreviewOption("Grid", kzaApplicationIsGridEnabled(application) == KZ_TRUE));

    debugOptions->Add(gcnew PreviewOption("Debug objects", kzaApplicationIsDebugObjectsEnabled(application) == KZ_TRUE));

    debugOptions->Add(gcnew PreviewOption("Free camera", kzaApplicationIsFreeCameraEnabled(application) == KZ_TRUE));

    return debugOptions;
}

void ApplicationToolModule::SetPreviewOptionValue(String^ previewOptionName, Object^ previewOptionValue)
{
    if (previewOptionName == "Grid")
    {
        bool booleanValue = (System::Boolean)previewOptionValue;
        kzaApplicationSetGridEnabled(application, booleanValue ? KZ_TRUE : KZ_FALSE);
    }
    else if (previewOptionName == "Debug objects")
    {
        bool booleanValue = (System::Boolean)previewOptionValue;
        kzaApplicationSetDebugObjectsEnabled(application, booleanValue ? KZ_TRUE : KZ_FALSE);
    }
    else if (previewOptionName == "Free camera")
    {
        bool booleanValue = (System::Boolean)previewOptionValue;
        WrapError(kzaApplicationSetFreeCameraEnabled(application, booleanValue ? KZ_TRUE : KZ_FALSE));
    }
    else
    {
        throw gcnew ArgumentException("Invalid preview option value");
    }
}

void ApplicationToolModule::SetTime(Int64 time)
{
    /* TODO: change time into 32 bit in the interface or pass 64 bit int forward. */
    kzaApplicationSetAnimationPlayerTime(application, (kzInt)time);
}


}}}}
