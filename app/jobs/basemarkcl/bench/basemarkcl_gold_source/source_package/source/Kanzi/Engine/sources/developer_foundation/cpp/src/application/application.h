#ifndef KANZIAPPLICATION_H
#define KANZIAPPLICATION_H

#include "user/scene_graph/scene.h"

namespace Kanzi {

namespace System
{
    class Window;
}

namespace User
{
    class Project;
    class Scene;
}

namespace Application {

class KANZI_API Application
{
public:
 
    // Constructor
	Application();
	
	// Destructor
	~Application();

    /*
    * \brief Returns active project.
    */
    User::Project* project() const;

    /*
    * \brief Returns active window.
    */
    System::Window* window() const;

    /*
	* \brief Performs periodical application update.
	*/
    void update(kzUint time);

    /*
    * \brief Starts application execution in closed loop.
    */
    void run();
    
    /*
    * \brief Pauses the application, no rendering or update is performed.
    */
    void pause();

    /*
    * \brief Resumes the application.
    */
    void resume();

    /*
    * \brief Signals application to quit, doesn't happen immediately.
    */
    void quit();

    /*
	* \brief Sets active scene.
	*/
    void setScene(ScenePtr& scene);

    /*
	* \brief Returns active scene.
	*/
    ScenePtr scene() const;

    /*
    * \brief Sets debug objects enabled / disabled.
    */
    void setDebugObjectsEnabled(kzBool enabled);
    
    /*
    * \brief Checks if debug objects are enabled.
    */
    kzBool debugObjectsEnabled() const;

    /*
    * \brief Sets grid enabled / disabled.
    */
    void setGridEnabled(kzBool enabled);

    /*
    * \brief Checks if grid is enabled.
    */
    kzBool gridEnabled() const;

    /*
    * \brief Sets fps info enabled / disabled.
    */
    void setFpsInfoEnabled(kzBool enabled);
    
    /*
    * \brief Checks if fps info is enabled.
    */
    kzBool fpsInfoEnabled() const;

    /*
    * \brief Sets bounding box visualization enabled / disabled.
    */
    void setBoundingBoxVisualizationEnabled(kzBool enabled);
    
    /*
    * \brief Checks if bounding box visualization is enabled.
    */
    kzBool boundingBoxVisualizationEnabled() const;

    /*
    * \brief Sets free camera enabled / disabled.
    */
    void setFreeCameraEnabled(kzBool enabled);

    /*
    * \brief Checks if free camera is enabled.
    */
    kzBool freeCameraEnabled() const;

private:
    class ApplicationPrivate* d;
};

} // Kanzi::Application
} // Kanzi

#endif
