/**
 * \file
 * Application interface. The functions declared here have no implementation in the Kanzi engine framework.
 * Instead the application must provide the implementations.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZA_APPLICATION_INTERFACE_H
#define KZA_APPLICATION_INTERFACE_H


#include <system/display/kzs_window.h>
#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzaApplication;
struct KzaSystemProperties;
struct KzaApplicationProperties;


/**
* Application configuration function. This function is called by the application framework when
* the application is started, but before it is initialized. 
* Must be implemented in the application that uses the application framework.
*/
void kzApplicationConfigure(const struct KzaSystemProperties* systemProperties, struct KzaApplicationProperties* configuration); /*lint -efunc(715,818,kzApplicationConfigure) Warning disabled for call-backs. */

/**
* Application initialization function. This function is called by the application framework when
* running the application. Must be implemented in the application that uses the application framework.
*/
kzsError kzApplicationInitialize(struct KzaApplication* application); /*lint -efunc(715,818,kzApplicationInitialize) Warning disabled for call-backs. */

/**
* Application uninitialization function. This function is called when closing the application. 
* Must be implemented in the application that uses the application framework.
*/
kzsError kzApplicationUninitialize(struct KzaApplication* application); /*lint -efunc(715,818,kzApplicationUninitialize) Warning disabled for call-backs. */

/**
* Application update function. This function is called continuously. 
* Must be implemented in the application that uses the application framework.
* Delta time is in milliseconds.
*/
kzsError kzApplicationUpdate(struct KzaApplication* application, kzUint deltaTime); /*lint -efunc(715,818,kzApplicationUpdate) Warning disabled for call-backs. */

/**
* Application render callback.
* Must be implemented in the application that uses the application framework.
*/
kzsError kzApplicationRender(const struct KzaApplication* application); /*lint -efunc(715,818,kzApplicationRender) Warning disabled for call-backs. */


#endif
