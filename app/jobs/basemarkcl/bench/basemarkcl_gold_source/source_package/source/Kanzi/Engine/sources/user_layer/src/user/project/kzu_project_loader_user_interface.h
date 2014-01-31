/**
* \file
* Project loader for user interface files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_LOADER_USER_INTERFACE_H
#define KZU_PROJECT_LOADER_USER_INTERFACE_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuBinaryDirectory;
struct KzuProject;
struct KzuUiComponent;
struct KzuUiComponentNode;
struct KzuUiComponentType;


/** Loads user interface component type data structure from binary. */
kzsException kzuProjectLoaderLoadUiComponentType(struct KzuProject* project, kzString path, struct KzuUiComponentType** out_componentType);

/** Loads UI component node from binary. */
kzsException kzuProjectLoaderLoadUiComponentNode(struct KzuProject* project, kzString path, struct KzuUiComponentNode** out_uiComponentNode);

/** Loads component event listeners for component. */
kzsException kzuProjectLoaderLoadUiComponentEventListenerTargets(struct KzuProject* project, const struct KzuUiComponentNode* componentNode);

/** Unloads user interface component type from project. */
kzsException kzuProjectLoaderUnloadUiComponentType(struct KzuProject* project, const struct KzuUiComponentType* componentType);


#endif
