/**
* \file
* Application framework. Symbian specific implementation.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kza_symbian_application.h"

extern "C"
{
#include <application/kza_application.h>
#include <application/kza_application_interface.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>

#include <system/wrappers/kzs_memory.h>
#include <system/display/kzs_window.h>
#include <system/kzs_types.h>
}

struct KzaApplication;

struct KzaPlatform
{
    int poop;
#if 0    
    CWsEventReceiver* wsEventReceiver;
#endif
};

kzsError kzaApplicationFrameworkPlatformCreate(struct KzaApplication* application, struct KzaPlatform** out_platform)
{
    kzsError result;
    struct KzaPlatform *platform;
    struct KzcMemoryManager* memoryManager;

    memoryManager = kzaApplicationGetFrameworkMemoryManager(application);
    result = kzcMemoryAllocVariable(memoryManager, platform, "Application framework platform");
    kzsErrorForward(result);
#if 0
    CCoeEnv* environment = new (ELeave) CCoeEnv();
    TRAPD( err, environment->ConstructL( ETrue, 0 ) );
    if( err != KErrNone )
    {
        result = 1;
        kzsErrorForward(result);
    }
#endif
    /*platform->wsEventReceiver = CWsEventReceiver::NewL(application);*/

    *out_platform = platform;
    kzsSuccess();
}

void kzaApplicationFrameworkPlatformUninitialize(struct KzaPlatform* platform)
{
    kzsFree(platform);
}

#if 0
CWsEventReceiver::CWsEventReceiver()
                 :CActive( CActive::EPriorityStandard )
{                      
}

CWsEventReceiver::~CWsEventReceiver()
{
    Cancel();
}

CWsEventReceiver* CWsEventReceiver::NewL(struct KzaApplication* application)
{
    CWsEventReceiver* self = new (ELeave) CWsEventReceiver;
    
    self->ConstructL(application);
    
    return self;    
}

void CWsEventReceiver::ConstructL(struct KzaApplication* application)
{
    if( !CCoeEnv::Static() )
        User::Leave( -1 );
    
    /*iParent = &application;*/
     
    // Register with the Windows Server for events.
    iWsSession = CCoeEnv::Static()->WsSession();
    
    iWsSession.EventReady( &iStatus );
    
    CActiveScheduler::Add( this );
    
    SetActive();
}

void CWsEventReceiver::RunL()
{
    TWsEvent wsEvent;
    iWsSession.GetEvent( wsEvent );
    
    // Request for more events.
    iWsSession.EventReady( &iStatus );
    
    SetActive();
}

void CWsEventReceiver::DoCancel()
{
    iWsSession.EventReadyCancel();
}

void handleSystemMessages(kzBool isBlocking)
{
    RThread thread;    
    TInt error = KErrNone;
    
    if (isBlocking && !thread.RequestCount())
    {
        // Block for a event.
        User::WaitForAnyRequest();

        // Run the Active Object corresponding to the event
        CActiveScheduler::RunIfReady(error, CActive::EPriorityIdle);
    }
    
    // Processes all outstanding requests
    while(thread.RequestCount()) 
    {
        // Decreases thread.RequestCount
        User::WaitForAnyRequest();
    }
}
#endif