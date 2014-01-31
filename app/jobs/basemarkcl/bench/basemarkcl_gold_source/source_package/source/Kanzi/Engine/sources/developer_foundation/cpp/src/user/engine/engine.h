#ifndef KANZIENGINE_H
#define KANZIENGINE_H

#include "predefinitions.h"

struct KzuEngine;

namespace Kanzi {

namespace System
{
    class Window;
}

namespace User {

class KANZI_API Engine
{
public:
	// Constructor
	Engine(KzuEngine* engine);
	
	// Destructor
	~Engine();

    void setActiveWindow(System::Window* window);

private:
    class EnginePrivate* d;
};

} // Kanzi::User
} // Kanzi


#endif
