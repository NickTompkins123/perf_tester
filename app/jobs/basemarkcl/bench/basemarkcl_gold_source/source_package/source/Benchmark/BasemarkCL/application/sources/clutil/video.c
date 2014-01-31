/* Video utility library. Uses avcodec
* Copyright 2011 by Rightware. All rights reserved.
*/

#include <benchmarkutil/bf_benchmark_framework.h>
#include <benchmarkutil/scene/bf_scene.h>
#include <benchmarkutil/util/bf_util.h>

#include <core/memory/kzc_memory_manager.h>
#include <system/wrappers/kzs_memory.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>

/* TODO: Cleanup & don't use kzerror to pass runtime parameters etc */

/* 
Example on how the current videos have been encoded:
ffmpeg -i ..\..\..\bin\data\StreetDance-Short-2D_800x480.mp4 -f mpeg -qmax 4 -qmin 1 -s 512x512 -y  ..\..\..\bin\data\movie3.mpeg
*/

struct VideoUtil
{
    kzByte *buffer;
    AVFormatContext *formatContext;
    AVCodecContext *codecContext;
    AVCodec *codec;
    AVFrame *curFrame;
    AVFrame *convertedFrame;

    struct SwsContext *scalingContext;

    kzUint width;
    kzUint height;
    kzInt format;
    kzInt videoStream;
};

/* TODO: Investigate if getting pointer to mapped memory area from opencl in order to decode the frame directly to the OpenCL device would be beneficial */
kzsError videoInit(struct BenchmarkFramework *framework, kzString filename, kzUint width, kzUint height, struct VideoUtil **outVideo)
{
    struct VideoUtil *video;
    struct KzcMemoryManager *memoryManager;
    kzInt result;
    kzUint i;

    memoryManager = bfGetMemoryManager(framework);
    result = kzcMemoryAllocPointer(memoryManager, &video, sizeof(struct VideoUtil), "Video util state");
    kzsErrorForward(result);
    kzsMemset(video, 0, sizeof(struct VideoUtil));


    /* TODO: Register only MPEG-1 codec and compile ffmpeg only with support for it */
    av_register_all();
    
    if(av_open_input_file(&video->formatContext, filename, NULL, 0, NULL) || av_find_stream_info(video->formatContext) < 0)
    {
        /* Die */
    }

    video->videoStream = -1;

    for(i = 0; i < video->formatContext->nb_streams; i++)
    {
        if(video->formatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            video->videoStream = i;
            break;
        }
    }
    if(video->videoStream == -1)
    {
        /* DIE! */
    }

    video->codecContext = video->formatContext->streams[video->videoStream]->codec;
    video->width = width? width: video->codecContext->width;
    video->height = height? height: video->codecContext->height;
    video->codec = avcodec_find_decoder(video->codecContext->codec_id);
    
    if(!video->codec ||
        avcodec_open(video->codecContext, video->codec) < 0)
    {
        /* DIE */
    }


    /* TODO: Check if we actually receive these */
    video->curFrame = avcodec_alloc_frame();
    video->convertedFrame = avcodec_alloc_frame();

    kzcMemoryAllocPointer(memoryManager, &video->buffer, avpicture_get_size(PIX_FMT_RGBA, video->width, video->height), "Framebuffer");

    /* Init buffers */
    avpicture_fill((AVPicture *) video->convertedFrame, video->buffer, PIX_FMT_RGBA, 
        video->width, video->height);

    /* Init scale & convert */
    video->scalingContext = sws_getContext(video->codecContext->width, video->codecContext->height, video->codecContext->pix_fmt,
        video->width, video->height, PIX_FMT_RGBA, SWS_BICUBIC, NULL, NULL, NULL);


    *outVideo = video;
    kzsSuccess();
}


kzsError videoNext(struct BenchmarkFramework *framework, struct VideoUtil *video)
{
	AVPacket packet;
	int finished = 0;
	
	/* Read a frame */
	if(av_read_frame(video->formatContext, &packet) < 0)
		return -1;
	
	/* Check if the packet comes from the correct stream */
	if(packet.stream_index != video->videoStream) {
		av_free_packet(&packet);
		return -2;
	}
	
    /*  Decode the next frame */
	/*  TODO: Do not decode frames if we want to skip some */
	avcodec_decode_video2(video->codecContext, video->curFrame, &finished, &packet);
	
	/* Does this packet complete a single frame? */
	if(!finished) {
		av_free_packet(&packet);
		return -3;
	}
	

    /* TODO: Figure out which format would allow us to skip the format conversion */
	sws_scale(video->scalingContext, video->curFrame->data, video->curFrame->linesize, 
		0, video->codecContext->height, video->convertedFrame->data, video->convertedFrame->linesize);
	av_free_packet(&packet);
	


    kzsSuccess();
}

kzsError videoNextFrame(struct BenchmarkFramework *framework, struct VideoUtil *video)
{
    kzInt result;

    do 
    {   
    result = videoNext(framework, video);
    
    } while (result == -3 || result == -2);

    return result;
}

kzsError videoUnitialize(struct BenchmarkFramework *framework, struct VideoUtil *video)
{
    sws_freeContext(video->scalingContext);
    av_free(video->curFrame);
    av_free(video->convertedFrame);

    avcodec_close(video->codecContext);
    av_close_input_file(video->formatContext);

    kzcMemoryFreePointer(video->buffer);
    kzcMemoryFreePointer(video);
    kzsSuccess();
}


kzByte* videoGetFramePointer(struct VideoUtil* video)
{
    return video->buffer;    
}
