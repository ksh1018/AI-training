/**
 * @file ptz_control.hpp
 * @author Yurim Seo, Beomseong Choi
 * @brief Ptz control module
 * @version 0.1
 * @date 2020-10-13
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef PTZ_CONTROL_HPP
#define PTZ_CONTROL_HPP

#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>
extern "C" {
    #include <curl/curl.h>
}

using namespace std;
/**
 * @brief Set request body from xml file, and send http request to url
 * 
 * @param curl Lib curl object
 * @param slist Setting list of lib curl object
 * @param op_code Operation code for each ptz control commands
 * */
void request_send(CURL *curl, struct curl_slist *slist, int op_code);

/**
 * @brief Set request header to lib curl object
 * 
 * @param curl Lib curl object
 * @param slit Setting list of lib curl object
 * @param url Detination of http request
 * */
void request_header_set(CURL *curl, struct curl_slist *slist, string url );

/**
 * @brief Load request body xml for each ptz commands
 */
void load_xml(void);

/**
 * @brief Loop for key board input to send ptz control command
 */
void* control_loop(void* ptr);

#endif