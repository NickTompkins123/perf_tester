/**
* \file
* Specifies a stereoscopic rendering modes.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_STEREOSCOPIC_RENDERING_H
#define KZU_STEREOSCOPIC_RENDERING_H


/** List of stereoscopic rendering modes. */
enum KzuStereoscopicRenderingMode
{
    KZU_STEREOSCOPIC_RENDERING_ANAGLYPHIC_RED_CYAN, /**< Anaglyphic red-cyan rendering. */
    KZU_STEREOSCOPIC_RENDERING_VERTICAL_LINE_INTERLACED, /**< Vertical line interlaced. */
    KZU_STEREOSCOPIC_RENDERING_HORIZONTAL_LINE_INTERLACED, /**< Vertical line interlaced. */
    KZU_STEREOSCOPIC_RENDERING_TOP_BOTTOM, /**< Top bottom viewports */
    KZU_STEREOSCOPIC_RENDERING_BOTTOM_TOP, /**< Bottom top viewports */
    KZU_STEREOSCOPIC_RENDERING_LEFT_RIGHT, /**< Left right viewports */
    KZU_STEREOSCOPIC_RENDERING_RIGHT_LEFT, /**< Right left viewports */
    KZU_STEREOSCOPIC_RENDERING_DISABLED /**< Disabled stereoscopic rendering. */
};


#endif
