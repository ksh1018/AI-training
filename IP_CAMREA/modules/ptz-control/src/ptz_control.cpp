/**
 * @file ptz_control.cc
 * @author Yurim Seo, Beomseong Choi
 * @brief Ptz control module
 * @version 0.1
 * @date 2020-10-13
 * 
 * @copyright Copyright (c) 2020
 */


#include "ptz_control.hpp"


// Array for save ptz request body from loaded xml files
string command[5];

// Array for ptz request xml file names
string request_xml[5] = {"tilt_up.xml", "pan_right.xml", "tilt_down.xml", "pan_left.xml", "stop.xml"};

// Destination url of ip camera ptz service
string url = "http://192.168.0.122:10080/onvif/ptz_services";


void request_send(CURL *curl, struct curl_slist *slist, int op_code) {
    CURLcode res;
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, command[op_code].c_str());
    res = curl_easy_perform(curl);
    curl_slist_free_all(slist);
}

void request_header_set(CURL *curl, struct curl_slist *slist, string url ) {

    // set http header
    slist = curl_slist_append(slist, "Accept-Charset: utf-8");
    slist = curl_slist_append(slist, "Content-Type: application/soap+xml");
    slist = curl_slist_append(slist, "Expect:");
  
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
    curl_easy_setopt(curl, CURLOPT_HEADER, 0);
    curl_easy_setopt(curl, CURLOPT_USERAGENT,  "Linux C  libcurl");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    // option for printing request and response info
    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    
}

void load_xml(void) {
    for(int i=0 ; i<5 ; i++) {
        ifstream inFile;
        // directory of ptz request xml files
        string load_dir = "./ptz_request/";
        load_dir.append(request_xml[i]);
        inFile.open(load_dir);
        stringstream strStream;
        strStream << inFile.rdbuf();
        string str = strStream.str();
        command[i] = str;
    }
    
}

void* control_loop(void* ptr) {


    // lib curl object for connection
    CURL *curl;

    CURLcode res;
        
    while(1){
        char op;
        op = getchar();
        fflush(stdin); 
        // key for escape
        if(op == 'q' || op == 'Q')
            break;

        //      up      right        down       left          
        if(op == 'w' || op == 'd' || op == 's' || op == 'a'){
            curl = curl_easy_init();
  
            struct curl_slist *slist = NULL;
            request_header_set(curl, slist, url);

            if(op == 'w') { 
                // continuous move. tilting up, operation code 0
                request_send(curl, slist, 0);
            } else if (op == 'd') {
                // continuous move. panning right, operation code 1
                request_send(curl, slist, 1);
            } else if (op == 's') {
                // continuous move. tilting down, operation code 2
                request_send(curl, slist, 2);
            } else if (op == 'a') {
                // continuous move. panning left, operation code 3
                request_send(curl, slist, 3);
            }

            // delay
            usleep(100000);    

            // stop requests, operation code 4
            request_header_set(curl, slist, url);
            request_send(curl, slist, 4);

            // free curl connnection
            curl_easy_cleanup(curl);
        }

    }
}
