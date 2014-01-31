/**
* \file
* Framebuffer object enumerations.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_FRAME_BUFFER_TYPE_H
#define KZC_FRAME_BUFFER_TYPE_H


/** Type of the FrameBuffer Target */
enum KzcFrameBufferTargetType
{
    KZC_FRAME_BUFFER_TARGET_INVALID, /**< Invalid frame buffer target. */
    KZC_FRAME_BUFFER_TARGET_COLOR, /**< Color buffer target. */
    KZC_FRAME_BUFFER_TARGET_DEPTH /**< Depth buffer target. */
};

/** FrameBuffer depth bits */
enum KzcFrameBufferDepthFormat
{
    KZC_FRAME_BUFFER_DEPTH_16, /**< 16 bit frame buffer. */
    KZC_FRAME_BUFFER_DEPTH_24, /**< 24 bit frame buffer. */
    KZC_FRAME_BUFFER_DEPTH_32 /**< 32 bit frame buffer. */
};


#endif
