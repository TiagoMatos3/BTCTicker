#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>


#ifndef HEADER_FILE_NAME
#define HEADER_FILE_NAME
    struct memory
    {
        char *payload;
        size_t size;
    };
    
    size_t write_callback(void *buffer, size_t size, size_t nmemb, void *userp);
    CURLcode curl_setopt_init(CURL *easyhandle, char *url, struct memory *chunk);
    char *curl_fetch_json(char *url);
#endif
