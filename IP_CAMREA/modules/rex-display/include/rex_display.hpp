/**
 * @file rex_display.hpp
 * @author Seunghun Kim (shkim@info-works.co.kr)
 * @brief Rex display module
 * @version 0.1
 * @date 2020-06-05
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef REX_DISPLAY_HPP
#define REX_DISPLAY_HPP

#include <string>

/**
 * @brief Initialize DRM display, set peripheral data variable addresses.
 * 
 * @param device DRM device
 * @param imu Address of IMU instance
 * @param uwb Address of UWB instance
 * @param battery Address of battery instance
 * @param pass Pass server instance
 * @return int DRM init result
 */
int drmDisplayInit(const char *device);

/**
 * @brief De-Initialize the display
 * 
 * @param fd File descriptor of display
 */
void drmDisplayDeInit(int fd);


/**
 * @brief Set asio session
 * 
 * @param asioSession 
 */
//void irSetSession(session_p asioSession);

void drawFrame(uint8_t *frame_buf);
#endif