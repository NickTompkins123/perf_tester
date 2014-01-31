#ifndef PREDEFINITIONS_H
#define PREDEFINITIONS_H

#include <system/kzs_types.h>

#ifdef _WIN32
#ifdef EXPORT_API
#define KANZI_API __declspec( dllexport ) 
#else
#define KANZI_API __declspec( dllimport ) 
#endif
#else
#define KANZI_API
#endif

#define ASSERT(x, location, msg)

#endif
