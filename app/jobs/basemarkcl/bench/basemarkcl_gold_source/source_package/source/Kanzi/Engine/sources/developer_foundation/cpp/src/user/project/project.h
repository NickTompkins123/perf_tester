#ifndef KANZIPROJECT_H
#define KANZIPROJECT_H

#include "predefinitions.h"
#include "user/scene_graph/scene.h"

struct KzuProject;
struct KzuBinaryDirectory;

namespace Kanzi {
namespace User {


class KANZI_API Project
{
public:
	// Constructor
	Project(KzuProject* project, KzuBinaryDirectory* directory);
	
	// Destructor
	~Project();

    /*
    * \brief Clears the contents of the project.
    */
    void clear();

    /*
    * \brief Loads project file.
    */
    kzBool load(const char* filename);

    /*
    * \brief Gets scene by path.
    */
    ScenePtr scene(const char* scenePath);

private:
    class ProjectPrivate* d;
};

} // Kanzi::User
} // Kanzi

#endif
