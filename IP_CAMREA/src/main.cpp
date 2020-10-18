#include <stdlib.h> 
#include <pthread.h>
#include <time.h>
#include "rex_display.hpp"
#include "rtsp_ffmpeg.hpp"
#include "ptz_control.hpp"

#define SAVE 1

#define SRC_WIDTH 640
#define SRC_HEIGHT 320
#define SRC_BPP 12

using namespace std;

string inputStream = "rtsp://admin:888888@";
string outputFileName = "Frame";
string ip_addr;
int maxFrameCount = 0;


int main (int argc, char *argv[]) 
{ 
    if((1 == argc) || (5 <= argc)){
        cout << "Usage: " << argv[0] << " [IP] [FRAMECOUNT]" << endl;
    }
    else if (2 == argc){
        ip_addr = argv[1];
        inputStream.append(argv[1]);
        inputStream.append(":10554/tcp/av0_1");
    }
    else if (3 == argc){
        ip_addr = argv[1];
        inputStream.append(argv[1]);
        inputStream.append(":10554/tcp/av0_1");
        maxFrameCount = atoi(argv[2]);
    }

    // load ptz control requst body xml
    load_xml();

    // ffmpeg setting for get rtsp camera
    rtsp_ffmpeg stream;
    stream.rtsp_ffmpeg_init(inputStream);

    // set frame save
    stream.rtsp_ffmpeg_set_save(SAVE);
    
    // set custom buffer
    rex_frame frameBuffer;
    stream.rtsp_ffmpeg_get_frame_info(&frameBuffer);
    frameBuffer.data = (uint8_t *)malloc(sizeof(uint8_t) * frameBuffer.width * frameBuffer.height * SRC_BPP / 8);

    //display init
    int drm_fd = drmDisplayInit("/dev/dri/card0");

    // thread for ptz control 
    pthread_t control_thread;
    int p_ret;
    p_ret = pthread_create(&control_thread, NULL, control_loop, NULL);
    if (p_ret < 0) {
		perror("control pthread create error: ");
		exit(EXIT_FAILURE);
	}
    
    // for frame_count
    int cnt = 0;

    while (true)
    {
        stream.rtsp_ffmpeg_loop(frameBuffer.data);
        drawFrame(frameBuffer.data);
        cnt++;
        if (0 == maxFrameCount) {
        } else if (cnt > maxFrameCount) {
            break;
        }
    }

    // deinitialize
    stream.rtsp_ffmpeg_deinit();
    drmDisplayDeInit(drm_fd);
    p_ret = pthread_join(control_thread, NULL);
    
    return (EXIT_SUCCESS); 
}