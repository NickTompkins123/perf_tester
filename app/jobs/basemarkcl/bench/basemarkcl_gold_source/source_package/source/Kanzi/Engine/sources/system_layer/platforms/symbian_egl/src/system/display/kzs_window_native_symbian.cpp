/**
* \file
* Kanzi window, native portion for the symbian platform.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_window_native_symbian.h"

extern "C"
{
#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_memory.h>
#include <system/kzs_error_codes.h>
#include <system/display/kzs_egl.h>
}

#include <e32base.h>
#include <w32std.h>
#include <coemain.h>
#include <stdio.h>

/* Forward declarations. */
struct KzsWindow;
class RWindow;

/** Platform specific window variables. */
struct KzsWindowNativeSymbian
{
    RWsSession wsSession;
    RWindow* window;
    RWindowGroup windowGroup;
    
    CWsScreenDevice* wsScreenDevice;
};

kzsError kzsWindowNativeSymbianCreate(struct KzsWindowNativeSymbian** out_windowNativeSymbian)
{
    struct KzsWindowNativeSymbian* windowNativeSymbian = (struct KzsWindowNativeSymbian*)kzsMalloc(sizeof(*windowNativeSymbian));
    /*kzsErrorTest(windowNativeSymbian != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Unable to allocate memory for Symbian window.");*/
      
    CCoeEnv* env = CCoeEnv::Static();
    /*CCoeEnv* env = new (ELeave) CCoeEnv();
    TRAPD( err, env->ConstructL( ETrue, 0 ) );
    if( err != KErrNone )
    {
        kzsErrorForward(1);
    }*/
    
    
    windowNativeSymbian->wsScreenDevice = env->ScreenDevice();
    
    windowNativeSymbian->wsSession = env->WsSession();
    windowNativeSymbian->windowGroup = env->RootWin();
    
    windowNativeSymbian->window = new (ELeave) RWindow(windowNativeSymbian->wsSession);
    TInt err = windowNativeSymbian->window->Construct(windowNativeSymbian->windowGroup, reinterpret_cast<TUint32>(windowNativeSymbian));
    
    windowNativeSymbian->windowGroup.EnableScreenChangeEvents();  

    TPixelsTwipsAndRotation pixnrot; 
    windowNativeSymbian->wsScreenDevice->GetScreenModeSizeAndRotation(windowNativeSymbian->wsScreenDevice->CurrentScreenMode(), pixnrot);
        
    /* scale window to fit the whole screen */
    windowNativeSymbian->window->SetExtent(TPoint(0, 0), pixnrot.iPixelSize);
                       
    windowNativeSymbian->window->SetRequiredDisplayMode(windowNativeSymbian->wsScreenDevice->DisplayMode());
    
    // Activate window and bring it to the foreground
    windowNativeSymbian->window->Activate();
    windowNativeSymbian->window->SetVisible(ETrue);
    windowNativeSymbian->window->SetNonFading(ETrue);
    windowNativeSymbian->window->SetShadowDisabled(ETrue); 
    windowNativeSymbian->window->EnableRedrawStore(EFalse); 
    windowNativeSymbian->window->EnableVisibilityChangeEvents();
    windowNativeSymbian->window->SetNonTransparent(); 
    windowNativeSymbian->window->SetBackgroundColor(); 
    windowNativeSymbian->window->SetOrdinalPosition(0);     
    
    *out_windowNativeSymbian = windowNativeSymbian;
    
    kzsSuccess();
}

kzsError kzsWindowNativeSymbianDelete(struct KzsWindowNativeSymbian* windowNativeSymbian)
{
    if(windowNativeSymbian->window != NULL)
    {
        windowNativeSymbian->window->SetOrdinalPosition(KOrdinalPositionSwitchToOwningWindow);
        windowNativeSymbian->window->Close();
        delete windowNativeSymbian->window;
        windowNativeSymbian->window = NULL;
    }
    
    kzsSuccess();
}

void* kzsWindowNativeSymbianGetWindowHandle(struct KzsWindowNativeSymbian* windowNativeSymbian)
{
    return (void*)windowNativeSymbian->window;
}
