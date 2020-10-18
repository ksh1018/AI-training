cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(IP_CAMERA_CONTROL)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -Og -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Og -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -std=c++14 -O3 -Ofast -ftree-vectorize")

set(PROJECT_SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR})

# modules
set(REX_DISPLAY ${PROJECT_SOURCE_DIR}/modules/rex-display)
set(RTSP_FFMPEG ${PROJECT_SOURCE_DIR}/modules/rtsp-ffmpeg)
set(PTZ_CONTROL ${PROJECT_SOURCE_DIR}/modules/ptz-control)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/common/include
    ${PROJECT_SOURCE_DIR}/common/include/libdrm
    ${PROJECT_SOURCE_DIR}/common/include/libdrm/libdrm
    ${PROJECT_SOURCE_DIR}/common/include/libdrm/libkms
    ${PROJECT_SOURCE_DIR}/common/include/libdrm/freedreno
    ${PROJECT_SOURCE_DIR}/common/include/opencv
    ${PROJECT_SOURCE_DIR}/common/include/ffmpeg
    ${REX_DISPLAY}/include
    ${PTZ_CONTROL}/include
    ${RTSP_FFMPEG}/include
)

link_directories(
    ${PROJECT_SOURCE_DIR}/common/libs/libdrm
    ${PROJECT_SOURCE_DIR}/common/libs/opencv
    ${PROJECT_SOURCE_DIR}/common/libs/rga
    ${PROJECT_SOURCE_DIR}/common/libs/ffmpeg
    ${PROJECT_SOURCE_DIR}/common/libs/curl
)

add_executable(${PROJECT_NAME}.out
    ${PROJECT_SOURCE_DIR}/src/main.cpp
    ${REX_DISPLAY}/src/rex_display.cpp
    ${RTSP_FFMPEG}/src/rtsp_ffmpeg.cpp
    ${PTZ_CONTROL}/src/ptz_control.cpp
)

target_link_libraries(${PROJECT_NAME}.out
    dl                      # for dynamic library(*.so) link
    pthread                 # for posix thread
    drm
    m
    rga
    libavutil.so
    libavcodec.so
    libavformat.so
    libavdevice.so
    curl
)