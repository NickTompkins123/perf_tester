/**
 * \file
 * Application tool module.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "application.hpp"


extern "C"
{
#include <application/kza_application.h>

#include <user/util/kzu_free_camera.h>
}


namespace Rightware {
namespace Kanzi {
namespace Applications {
namespace DefaultApplication {
namespace ToolModule {


IEnumerable<PreviewOption^>^ ApplicationToolModule::GetPreviewOptions()
{
    List<PreviewOption^>^ debugOptions = gcnew List<PreviewOption^>();
    
    /* Grid */
    {
        debugOptions->Add(gcnew PreviewOption("Grid", kzaApplicationIsGridEnabled(application) == KZ_TRUE));

        debugOptions->Add(gcnew PreviewOption("Grid size", nullptr, (float)kzaApplicationGetTransformGizmoGridSize(application)));
    }

    debugOptions->Add(gcnew PreviewOption("Debug objects", kzaApplicationIsDebugObjectsEnabled(application) == KZ_TRUE));

    /* Layout */
    {
        debugOptions->Add(gcnew PreviewOption("Allocated layout", kzaApplicationIsAllocatedLayouBoundingBoxVisualizationEnabled(application) == KZ_TRUE));

        debugOptions->Add(gcnew PreviewOption("Actual layout", kzaApplicationIsActualLayouBoundingBoxVisualizationEnabled(application) == KZ_TRUE));
    }


    /* Snap */
    {
        debugOptions->Add(gcnew PreviewOption("Snap", kzaApplicationIsTransformGizmoGridSnapEnabled(application) == KZ_TRUE));

        KzuTransformGizmoGridSnapMode snapMode = kzaApplicationGetTransformGizmoGridSnapMode(application);
        array<String^>^ snapModes = { "Relative", "Absolute", "Object" }; /* TODO: Enable "Object" once it is implemented. */
        String^ snapModeString;
        switch(snapMode)
        {
            case KZU_TRANSFORM_GIZMO_GRID_SNAP_NONE:
            {
                snapModeString = "None";
                break;
            }
            case KZU_TRANSFORM_GIZMO_GRID_SNAP_RELATIVE:
            {
                snapModeString = "Relative";
                break;
            }
            case KZU_TRANSFORM_GIZMO_GRID_SNAP_ABSOLUTE:
            {
                snapModeString = "Absolute";
                break;
            }
            case  KZU_TRANSFORM_GIZMO_GRID_SNAP_OBJECT:
            {
                snapModeString = "Object";
                break;
            }
            default:
            {
                throw gcnew NotImplementedException("Snap mode enum out of range.");
                break;
            }
        }
        debugOptions->Add(gcnew PreviewOption("Snap mode", snapModes, snapModeString));
    }

    /* Free Camera */
    {
        debugOptions->Add(gcnew PreviewOption("Free camera", kzaApplicationIsFreeCameraEnabled(application) == KZ_TRUE));

        if(kzaApplicationGetFreeCamera(application) != KZ_NULL)
        {
            debugOptions->Add(gcnew PreviewOption("Lock free camera", kzuFreeCameraIsFreezed(kzaApplicationGetFreeCamera(application)) == KZ_TRUE));
            debugOptions->Add(gcnew PreviewOption("Save free camera values", false));
        }
    }

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
    else if (previewOptionName == "Allocated layout")
    {
        bool booleanValue = (System::Boolean)previewOptionValue;
        kzaApplicationSetAllocatedLayoutBoundingBoxVisualizationEnabled(application, booleanValue ? KZ_TRUE : KZ_FALSE);
    }
    else if (previewOptionName == "Actual layout")
    {
        bool booleanValue = (System::Boolean)previewOptionValue;
        kzaApplicationSetActualLayoutBoundingBoxVisualizationEnabled(application, booleanValue ? KZ_TRUE : KZ_FALSE);
    }
    else if (previewOptionName == "Snap")
    {
        bool booleanValue = (System::Boolean)previewOptionValue;
        kzaApplicationSetTransformGizmoGridSnapEnabled(application, booleanValue ? KZ_TRUE : KZ_FALSE);
    }
    else if (previewOptionName == "Snap mode")
    {
        String^ stringValue = (System::String^)previewOptionValue;
        KzuTransformGizmoGridSnapMode snapMode = KZU_TRANSFORM_GIZMO_GRID_SNAP_NONE;
        if (String::Equals(stringValue, "Relative"))
        {
            snapMode = KZU_TRANSFORM_GIZMO_GRID_SNAP_RELATIVE;
        }
        else if (String::Equals(stringValue, "Absolute"))
        {
            snapMode = KZU_TRANSFORM_GIZMO_GRID_SNAP_ABSOLUTE;
        }
        else if (String::Equals(stringValue, "Object"))
        {
            snapMode = KZU_TRANSFORM_GIZMO_GRID_SNAP_OBJECT;
        }
        kzaApplicationSetTransformGizmoGridSnapMode(application, snapMode);
    }
    else if (previewOptionName == "Grid size")
    {
        if (previewOptionValue != nullptr && previewOptionValue->GetType() == System::Single::typeid)
        {
            float floatValue = (System::Single)previewOptionValue;
            kzaApplicationSetTransformGizmoGridSize(application, (kzFloat)floatValue);
        }
    }
    else if (previewOptionName == "Lock free camera")
    {
        bool booleanValue = (System::Boolean)previewOptionValue;
        if(kzaApplicationGetFreeCamera(application) != KZ_NULL)
        {
            kzuFreeCameraSetFreezed(kzaApplicationGetFreeCamera(application), booleanValue);
        }
        
    }
    else if (previewOptionName == "Save free camera values")
    {
        bool booleanValue = (System::Boolean)previewOptionValue;
        if (booleanValue && kzaApplicationIsFreeCameraEnabled(application))
        {
            ToolModule::SetCameraTransformation();
        }
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


}}}}}
