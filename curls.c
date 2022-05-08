//#include <stdlib.h>
//#include <stdio.h>
#include <string.h>
#include "curls.h"


/*Function to get all the page's content.*/
size_t write_callback(void *buffer, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;              /*Calculate buffer size*/
    struct memory *mem = (struct memory *) userp; /*Transform the void *userp to struct memory *userp;*/

    char *ptr = realloc(mem->payload, mem->size + realsize + 1); /*Expand buffer*/

    if(!ptr)
    {
        fprintf(stderr, "ERROR: Failed to expand buffer in write_callback()");

        free(mem->payload);

        return 1;
    }

    mem->payload = ptr;

    memcpy(&(mem->payload[mem->size]), buffer, realsize); /*Copy mem->response to buffer.*/
    
    mem->size += realsize;
    
    mem->payload[mem->size] = 0;

    return realsize;
}

CURLcode curl_setopt_init(CURL *easyhandle, char *url, struct memory *chunk)
{
    CURLcode rcode;

    curl_easy_setopt(easyhandle, CURLOPT_URL, url);
    curl_easy_setopt(easyhandle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, (void *)chunk);
    curl_easy_setopt(easyhandle, CURLOPT_DEFAULT_PROTOCOL, "https");
    curl_easy_setopt(easyhandle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(easyhandle, CURLOPT_TIMEOUT, 15);
    curl_easy_setopt(easyhandle, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(easyhandle, CURLOPT_MAXREDIRS, 1);

    rcode = curl_easy_perform(easyhandle); /*Perform the connection to the site.*/
    return rcode;
}

char *curl_fetch_json(char* url)
{
    CURL *easyhandle;
    CURLcode rcode;


    struct memory chunk; /*Create a chunk variable and then referenciate its address on *cf*/
    chunk.payload = malloc(1);
    chunk.size = 0;

    easyhandle = curl_easy_init();

    if(easyhandle)
    {
        rcode = curl_setopt_init(easyhandle, url, &chunk);

        curl_easy_cleanup(easyhandle);
        if(rcode != CURLE_OK)
        {
            fprintf(stderr, "\nLibcurl: %d ", rcode);
        }

        else
        {
            return chunk.payload;
        }
    }
}