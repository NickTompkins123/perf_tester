/**
* \file
* User interface dock layout component.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_ui_dock_layout.h"
#include "kzu_ui_component_common.h"

#include <user/ui/kzu_ui_component_base.h>
#include <user/ui/kzu_ui_component_type.h>
#include <user/ui/kzu_ui_component.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/string/kzc_string.h>


const KzuComponentTypeIdentifier KZU_COMPONENT_TYPE_DOCK_LAYOUT = (void*)&KZU_COMPONENT_TYPE_DOCK_LAYOUT;


KZ_CALLBACK static kzsError kzuUiDockLayoutMeasure(struct KzuTransformedObjectNode* node)
{
    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuUiDockLayoutArrange(const struct KzuScene* scene, struct KzuTransformedObjectNode* node, struct KzuTransformedObjectNode* parentNode)
{
    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiDockLayoutInitialize(const struct KzcMemoryManager* memoryManager, struct KzuUiComponentNode* componentNode)
{
    kzuUiComponentTypeSetTypeIdentifier(kzuUiComponentNodeGetType(componentNode), KZU_COMPONENT_TYPE_DOCK_LAYOUT);

    kzuUiComponentNodeSetMeasureFunction(componentNode, kzuUiDockLayoutMeasure);
    kzuUiComponentNodeSetArrangeFunction(componentNode, kzuUiDockLayoutArrange);
    
    kzsSuccess();
}
