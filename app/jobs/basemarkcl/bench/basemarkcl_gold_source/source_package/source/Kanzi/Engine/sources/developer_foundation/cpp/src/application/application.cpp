extern "C"
{
#include <application/kza_application.h>
#include <application/kza_application_interface.h>
#include <user/common/kzu_user.h>

#include <system/kzs_system.h>
#include <system/display/kzs_display.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/memory/kzc_memory_manager.h>

#ifdef ANDROID
#include <jni.h>
#include <application/kza_android_application_interface.h>
#endif
}

#include "application.h"
#include "user/engine/engine.h"
#include "user/project/project.h"
#include "user/scene_graph/scene.h"
#include "system/display/window.h"
#include <exception>

void kzApplicationConfigure(const struct KzaSystemProperties* systemProperties, struct KzaApplicationProperties* configuration)
{
    configuration->memoryPoolSize = 30 * 1024 * 1024;
    //configuration->pickingEnabled = KZ_TRUE;
    //configuration->clearBackgroundEnabled = KZ_TRUE;
    //configuration->transformGizmoEnabled = KZ_TRUE;
    configuration->surfaceProperties.bitsDepthBuffer = 16;
}

kzsError kzApplicationInitialize(struct KzaApplication* application)
{
    return KZS_SUCCESS;
}

kzsError kzApplicationUninitialize(struct KzaApplication* application)
{
    return KZS_SUCCESS;
}

kzsError kzApplicationUpdate(struct KzaApplication* application, kzUint deltaTime)
{
    return KZS_SUCCESS;
}

kzsError kzApplicationRender(const struct KzaApplication* application)
{
    return KZS_SUCCESS;
}


namespace Kanzi
{
using namespace User;
using namespace System;

namespace Application
{

class ApplicationPrivate
{
public:
    Window* window;
    Engine* engine;
    Project* project;
    
    KzaApplication* application;
};

Application::Application() :
    d(new ApplicationPrivate())
{
    KzaSystemProperties systemProperties;
    systemProperties.programArgumentCount = 0;
    systemProperties.programArguments = KZ_NULL;   
    kzaApplicationCreate(&systemProperties, &d->application);

    kzaApplicationInitialize(d->application);

    KzuProject* project = (KzuProject*)kzaApplicationGetProject(d->application);
    d->project = new Project(project, kzaApplicationGetBinaryDirectory(d->application));

    KzuEngine* engine = (KzuEngine*)kzaApplicationGetEngine(d->application);
    d->engine = new Engine(engine);

    KzsWindow* window = (KzsWindow*)kzaApplicationGetWindow(d->application);
    d->window = new Window(window);
}

Application::~Application()
{
    kzaApplicationUninitialize(d->application);    
    kzaApplicationDelete(d->application);

    delete d;
}

User::Project* Application::project() const
{
    return d->project;
}

Window* Application::window() const
{
    return d->window;
}

void Application::update(kzUint time)
{
    kzaApplicationSetState(d->application, KZA_APPLICATION_STATE_RUNNING);
    kzaApplicationUpdate(d->application, time);
    kzaApplicationUpdateWindows(d->application);
}

void Application::run()
{
    kzUint deltaTime = 30;
    kzaApplicationSetState(d->application, KZA_APPLICATION_STATE_RUNNING);

    while(kzaApplicationGetState(d->application) == KZA_APPLICATION_STATE_RUNNING ||
        kzaApplicationGetState(d->application) == KZA_APPLICATION_STATE_PAUSED)
    {
        update(deltaTime);
    }
}

void Application::pause()
{
    kzaApplicationPause(d->application);
}

void Application::resume()
{
    kzaApplicationResume(d->application);
}

void Application::quit()
{
    kzaApplicationQuit(d->application);
}

void Application::setScene(ScenePtr& scene)
{
    kzaApplicationSetScene(d->application, scene->handle());
}

ScenePtr Application::scene() const
{
    Scene* result = KZ_NULL;

    KzuScene* scene = kzaApplicationGetScene(d->application);
    if (scene)
    {
        result = new Scene(scene);   
    }

    return ScenePtr(result);
}

void Application::setDebugObjectsEnabled(kzBool enabled)
{
    kzaApplicationSetDebugObjectsEnabled(d->application, enabled);
}

kzBool Application::debugObjectsEnabled() const
{
    return kzaApplicationIsDebugObjectsEnabled(d->application);
}

void Application::setGridEnabled(kzBool enabled)
{
    kzaApplicationSetGridEnabled(d->application, enabled);
}

kzBool Application::gridEnabled() const
{
    return kzaApplicationIsGridEnabled(d->application);
}

void Application::setFpsInfoEnabled(kzBool enabled)
{
    kzaApplicationSetFpsInfoEnabled(d->application, enabled);
}

kzBool Application::fpsInfoEnabled() const
{
    return kzaApplicationIsFpsInfoEnabled(d->application);
}

void Application::setBoundingBoxVisualizationEnabled(kzBool enabled)
{
    kzaApplicationSetBoundingBoxVisualizationEnabled(d->application, enabled);
}

kzBool Application::boundingBoxVisualizationEnabled() const
{
    return kzaApplicationIsBoundingBoxVisualizationEnabled(d->application);
}

void Application::setFreeCameraEnabled(kzBool enabled)
{
    kzaApplicationSetFreeCameraEnabled(d->application, enabled);
}

kzBool Application::freeCameraEnabled() const
{
    return kzaApplicationIsFreeCameraEnabled(d->application);
}

} // Kanzi::Application
} // Kanzi
