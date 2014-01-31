/**
 * \file
 * Application framework splash screen.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZA_SPLASH_H
#define KZA_SPLASH_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzcResourceManager;
struct KzcRenderer;
struct KzcMemoryManager;
struct KzsWindow;


/**
 * \struct KzaSplashScreen
 * Splash screen structure.
 */
struct KzaSplashScreen;


/** Create splash screen structure. */
kzsError kzaSplashScreenCreate(const struct KzcMemoryManager* memoryManager, struct KzcResourceManager* resourceManager, 
                               struct KzcRenderer* renderer, struct KzaSplashScreen** out_splash);

/** Delete splash screen structure. */
kzsError kzaSplashScreenDelete(struct KzaSplashScreen* splash, struct KzcRenderer* renderer);

/** Render splash screen. */
kzsError kzaSplashScreenRender(const struct KzaSplashScreen* splash, struct KzcRenderer* renderer, const struct KzsWindow* window);


#endif
