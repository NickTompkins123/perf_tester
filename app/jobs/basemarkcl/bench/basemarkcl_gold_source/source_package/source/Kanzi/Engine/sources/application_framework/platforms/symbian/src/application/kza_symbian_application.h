/**
* \file
* Application framework. Symbian specific implementation.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZA_SYMBIAN_APPLICATION_H
#define KZA_SYMBIAN_APPLICATION_H

#include <system/kzs_types.h>

struct KzaApplication;
struct KzaPlatform;

#ifdef __cplusplus
#include <e32base.h>  // for CBase
#include <w32std.h>   // for TWsEvent
#include <coemain.h>
#endif

#ifdef __cplusplus
extern "C"
#endif
void handleSystemMessages(kzBool isBlocking);

#ifdef __cplusplus
class CWsEventReceiver : public CActive
{
public: 
    virtual void RunL();
    virtual void DoCancel();

    static CWsEventReceiver* NewL(struct KzaApplication* application);
    ~CWsEventReceiver();

private: // Constructors
    CWsEventReceiver();
    void ConstructL(struct KzaApplication* application);

private:

    /** The Windows Server Session */
    RWsSession iWsSession;
    
};
#endif

#endif
