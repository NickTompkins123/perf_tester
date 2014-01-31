#ifndef KANZISCENE_H
#define KANZISCENE_H

#include "predefinitions.h"
#include "smartptr.h"

struct KzuScene;

namespace Kanzi {
namespace User {

class KANZI_API Scene
{
public:
    Scene(const Scene& scene);
    
    // Constructor
    Scene(KzuScene* scene);
	
	// Destructor
	~Scene();

	/*
	* \brief Returns Kanzi scene handle.
	*/
    KzuScene* handle() const;

private:
    class ScenePrivate* d;
};
}
}

typedef SmartPtr<Kanzi::User::Scene> ScenePtr;

#endif
