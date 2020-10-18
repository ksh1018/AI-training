#pragma RETAIN

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <iostream>
#include <string>

#include "rtsp_ffmpeg.hpp"

extern "C" {
#include <libavcodec/avcodec.h> 
#include <libavformat/avformat.h> 
#include <libavformat/avio.h> 
#include <libavdevice/avdevice.h>
#include <libavutil/imgutils.h>
}

using namespace std;

int rtsp_ffmpeg::saveYUVFrameToFile(AVFrame* pFrame, int width, int height)
{
    FILE* fileHandle;
    int y, writeError;
    char filename[32];
    static int frameNumber = 0;

    sprintf(filename, "frame%d.yuv", frameNumber);

    fileHandle = fopen(filename, "wb");
    if (fileHandle == NULL)
    {
        printf("Unable to open %s...\n", filename);
        return -1;
    }

    /*Writing Y plane data to file.*/
    for (y = 0; y < height; y++)
    {
        writeError = fwrite(pFrame->data[0] + y*pFrame->linesize[0], 1, width, fileHandle);
        if (writeError != width)
        {
            printf("Unable to write Y plane data!\n");
            return -1;
        }
    }

    /*Dividing by 2.*/
    height >>= 1;
    width >>= 1;

    /*Writing U plane data to file.*/
    for (y = 0; y < height; y++)
    {
        writeError = fwrite(pFrame->data[1] + y*pFrame->linesize[1], 1, width, fileHandle);
        if (writeError != width)
        {
            printf("Unable to write U plane data!\n");
            return -1;
        }
    }

    /*Writing V plane data to file.*/
    for (y = 0; y < height; y++)
    {
        writeError = fwrite(pFrame->data[2] + y*pFrame->linesize[2], 1, width, fileHandle);
        if (writeError != width)
        {
            printf("Unable to write V plane data!\n");
            return -1;
        }
    }

    fclose(fileHandle);
    frameNumber++;

    return 0;
}

int rtsp_ffmpeg::rtsp_ffmpeg_find_video_stream(AVFormatContext *pFormatContext)
{
    //search video stream
    inputStreamIndex = av_find_best_stream(pFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	if (inputStreamIndex == -1) {
		cout << "Didn't find a video stream." << endl;
		return EXIT_FAILURE;
	}
    return EXIT_SUCCESS;
}

int rtsp_ffmpeg::rtsp_ffmpeg_alloc_FormatContext(AVFormatContext** ppFormatContext)
{
    *ppFormatContext = avformat_alloc_context(); 
    return 0;
}

int rtsp_ffmpeg::rtsp_ffmpeg_get_FormatContext(AVFormatContext* pFormatContext)
{
    // get context
    if (0 > avformat_find_stream_info(pFormatContext, NULL))
    {
        return EXIT_FAILURE; 
    } 
    rtsp_ffmpeg_find_video_stream(pFormatContext);
    return EXIT_SUCCESS;
}

int rtsp_ffmpeg::rtsp_ffmpeg_open_stream(AVFormatContext* pFormatContext, string rtspAddress)
{
    AVDictionary *pDictionary = NULL;
    // set stream is rtsp
    int ret = av_dict_set(&pDictionary, "rtsp_transport", "tcp", 0); // default udp. Set tcp interleaved mode
    if (0 > ret) 
    {
        return EXIT_FAILURE;
    }

    //open rtsp 
    if (0 != avformat_open_input(&pFormatContext, rtspAddress.c_str(), NULL, &pDictionary))
    { 
        return EXIT_FAILURE; 
    } 
    av_dict_free(&pDictionary);
    return EXIT_SUCCESS;
}

int rtsp_ffmpeg::rtsp_ffmpeg_free_FormatContext(AVFormatContext * pFormatContext)
{
    avio_close(pFormatContext->pb); 
    avformat_free_context(pFormatContext); 
    return 0;
}

int rtsp_ffmpeg::rtsp_ffmpeg_get_CodecContext(AVCodecContext** ppCodecContext, AVFormatContext* pFormatContext)
{
    AVStream* pInputStream = pFormatContext-> streams[inputStreamIndex]; 
    AVCodecParameters *pInputCodecParameters = pInputStream->codecpar;
    AVCodec *pInputCodec = avcodec_find_decoder(pInputCodecParameters->codec_id);   // find h.264 decoder

    if (pInputCodec == NULL) {
		printf("Codec not found.\n");
		return EXIT_FAILURE;
	}
    
    *ppCodecContext = avcodec_alloc_context3(pInputCodec);
	avcodec_parameters_to_context(*ppCodecContext, pInputCodecParameters);
	
    // codec open
    if (avcodec_open2(*ppCodecContext, pInputCodec, NULL) < 0) {
		printf("Could not open codec.\n");
		return EXIT_FAILURE;
	}
}   
int rtsp_ffmpeg::rtsp_ffmpeg_free_CodecContext(AVCodecContext *pCodecContext)
{
    avcodec_free_context(&pCodecContext);
    return 0;
} 

int rtsp_ffmpeg::rtsp_ffmpeg_alloc_frame(AVFrame** ppFrame)
{
    *ppFrame = av_frame_alloc();
    return 0;
}

int rtsp_ffmpeg::rtsp_ffmpeg_free_frame(AVFrame* pFrame)
{
    av_frame_free(&pFrame);
    return 0;
}

int rtsp_ffmpeg::rtsp_ffmpeg_play_stream(AVFormatContext* pFormatContext)
{
    return av_read_play(pFormatContext);
}

int rtsp_ffmpeg::rtsp_ffmpeg_pause_stream(AVFormatContext* pFormatContext)
{
    return av_read_pause(pFormatContext); 
}


int rtsp_ffmpeg::rtsp_ffmpeg_init(string rtspAddress)
{ 
    // Initialize
    avdevice_register_all();
    avformat_network_init(); 
    rtsp_ffmpeg_alloc_FormatContext(&pInputFormatContext);
    rtsp_ffmpeg_open_stream(pInputFormatContext, rtspAddress);
    rtsp_ffmpeg_get_FormatContext(pInputFormatContext);
    rtsp_ffmpeg_get_CodecContext(&pInputCodecContext, pInputFormatContext);
    rtsp_ffmpeg_alloc_frame(&pInputFrame);
    rtsp_ffmpeg_play_stream(pInputFormatContext);
    saveFlag = 0;
    return 0;
}

int rtsp_ffmpeg::rtsp_ffmpeg_loop(uint8_t *pFrameBuffer)
{
    static int frameCount = 0;
    AVPacket packet;
    av_init_packet(&packet); 
    int nRecvPacket = av_read_frame(pInputFormatContext, &packet);   // read frame from stream

    if (0 > nRecvPacket) {
        cout << "read frame failed!" << endl;
        return EXIT_FAILURE;
    }
    if (packet.stream_index == inputStreamIndex) // video frame
    {
        avcodec_send_packet(pInputCodecContext, &packet);           // send h.264 frame to decoder
        if (0 == avcodec_receive_frame(pInputCodecContext, pInputFrame)) {  // receive raw frame from decoder
            if((0 == (frameCount % 15)) && saveFlag) {
                // save to raw file
                saveYUVFrameToFile(pInputFrame, pInputCodecContext->width, pInputCodecContext->height);
            }
            // copy to custom buffer
            av_image_copy_to_buffer(pFrameBuffer, av_image_get_buffer_size(pInputCodecContext->pix_fmt, pInputCodecContext->width, pInputCodecContext->height, 1),
                    pInputFrame->data, pInputFrame->linesize, pInputCodecContext->pix_fmt, pInputCodecContext->width, pInputCodecContext->height, 1);
            frameCount++;
        }
    }    
    av_packet_unref(&packet); 
    return EXIT_SUCCESS;
}

int rtsp_ffmpeg::rtsp_ffmpeg_deinit()
{   
    // deinitialize
    rtsp_ffmpeg_pause_stream(pInputFormatContext);
    rtsp_ffmpeg_free_frame(pInputFrame);
    rtsp_ffmpeg_free_CodecContext(pInputCodecContext);
    rtsp_ffmpeg_free_FormatContext(pInputFormatContext);    
}

int rtsp_ffmpeg::rtsp_ffmpeg_set_save(int flag)
{
    saveFlag = flag;
    return 0;
}

int rtsp_ffmpeg::rtsp_ffmpeg_get_frame_info(rex_frame *pFrameBuffer)
{
    pFrameBuffer->width = pInputCodecContext->width;
    pFrameBuffer->height = pInputCodecContext->height;
    return 0;
}