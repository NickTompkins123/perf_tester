/**
 * \file
 * Application tool module.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#pragma once


#include <tool_module.hpp>


namespace Rightware {
namespace Kanzi {
namespace Applications {
namespace DefaultApplication {
namespace ToolModule {


public ref class ApplicationToolModule : public ToolModule
{
public:
    virtual IEnumerable<PreviewOption^>^ GetPreviewOptions() override;

    virtual void SetPreviewOptionValue(String^ previewOptionName, Object^ previewOptionValue) override;

    virtual void SetTime(Int64 time) override;
};


}}}}}
