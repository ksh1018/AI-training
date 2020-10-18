#ifndef __RTSP_FFMPEG_H__
#define __RTSP_FFMPEG_H__

#include <string>

extern "C" {
#include <libavcodec/avcodec.h> 
#include <libavformat/avformat.h> 
#include <libavformat/avio.h> 
#include <libavdevice/avdevice.h>
#include <libavutil/imgutils.h>
}

using namespace std;

typedef struct {
    uint8_t *data;
    int width;
    int height;
}rex_frame;
class rtsp_ffmpeg{
public:
    /**
     * @brief open and get rtsp stream by ffmpeg
     * 
     * @param rtspAddress rtsp stream address
     * */
    int rtsp_ffmpeg_init(string rtspAddress);

    /**
     * @brief get frame from rtsp stream and save it to pFrameBuffer
     * 
     * @param pFrameBuffer custom frame buffer
     * */
    int rtsp_ffmpeg_loop(uint8_t *pFrameBuffer);

    /**
     * @brief close stream and free memory
     * */
    int rtsp_ffmpeg_deinit();

    /**
     * @brief flag setting for raw frame save 
     * 
     * @param flag true/false(1/0)
     * */
    int rtsp_ffmpeg_set_save(int flag);

    /**
     * @brief get width and height
     * 
     * @param pFrameBuffer buffer which have width and height information
     * */
    int rtsp_ffmpeg_get_frame_info(rex_frame *pFrameBuffer);
private:
    int saveYUVFrameToFile(AVFrame* pFrame, int width, int height);
    int rtsp_ffmpeg_find_video_stream(AVFormatContext *pFormatContext);
    int rtsp_ffmpeg_alloc_FormatContext(AVFormatContext** ppFormatContext);
    int rtsp_ffmpeg_get_FormatContext(AVFormatContext* pFormatContext);
    int rtsp_ffmpeg_open_stream(AVFormatContext* pFormatContext, string rtspAddress);
    int rtsp_ffmpeg_free_FormatContext(AVFormatContext * pFormatContext);
    int rtsp_ffmpeg_get_CodecContext(AVCodecContext** ppCodecContext, AVFormatContext* pFormatContext);
    int rtsp_ffmpeg_free_CodecContext(AVCodecContext *pCodecContext);
    int rtsp_ffmpeg_alloc_frame(AVFrame** ppFrame);
    int rtsp_ffmpeg_free_frame(AVFrame* pFrame);
    int rtsp_ffmpeg_play_stream(AVFormatContext* pFormatContext);
    int rtsp_ffmpeg_pause_stream(AVFormatContext* pFormatContext);

    AVFormatContext* pInputFormatContext;
    AVCodecContext* pInputCodecContext;
    AVFrame* pInputFrame;
    int saveFlag;                           // raw frame save flag
    int inputStreamIndex;                   // Video Index
};
#endif