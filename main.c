#include "curls.h"
#include <string.h>
#include <cjson/cJSON.h>
#include <sqlite3.h>


cJSON *parsePrice(char *data, cJSON *json)
{
    json = cJSON_Parse(data);
    cJSON *lastPrice = NULL;

    lastPrice = cJSON_GetObjectItemCaseSensitive(json, "last");
    if(lastPrice->valuestring == NULL)
    {        
        fprintf(stderr, "\ncJSON *parsePrice error.\n");
        cJSON_Delete(json);
        return NULL;
    }
    else
    {
        return lastPrice;
    }
}

cJSON *parseTimestamp(char *data, cJSON *json)
{
    //json = cJSON_Parse(data);
    json = cJSON_Parse(data);
    cJSON *timestamp = NULL;

    timestamp = cJSON_GetObjectItemCaseSensitive(json, "timestamp");
    if(timestamp->valuestring == NULL)
    {
        fprintf(stderr, "\ncJSON *parseTimestamp error.\n");
        return NULL;
    }
    else
    {
        return timestamp;
    }
}


int main()
{
    char *url = "https://www.bitstamp.net/api/v2/ticker_hour/btcusd/";
    char *data;



    /*cJSON*/
    cJSON *json = NULL;
    cJSON *lastPrice = NULL;
    cJSON *timestamp = NULL;
    /*cJSON*/

    /*sqlite3*/
    sqlite3 *db;
    int rcode;
    char *insertsql;
    char *err_msg = 0;
    /*sqlite3*/


    /*libcurl*/
    curl_global_init(CURL_GLOBAL_ALL); //Initialize libcurl globally, with all features enabled (CURL_GLOBAL_ALL).
    data = curl_fetch_json(url);
    curl_global_cleanup(); //Cleanup libcurl globally.
    /*libcurl*/

    if((lastPrice = parsePrice(data, json)) == NULL)
    {
        fprintf(stderr, "parsePrice returned NULL.");
        cJSON_Delete(lastPrice);
        free(data);        
        return 1;
    }

    if((timestamp = parseTimestamp(data, json)) == NULL)
    {
        fprintf(stderr, "parseTimestamp returned NULL.");
        cJSON_Delete(timestamp);
        cJSON_Delete(lastPrice);
        free(data);
        return 1;
    }


    /*sqlite3*/
    rcode = sqlite3_open("test.db", &db);

    if(rcode != SQLITE_OK)
    {
        fprintf(stderr, "Error opening DB: %s\n", sqlite3_errmsg(db));
        cJSON_Delete(lastPrice);
        cJSON_Delete(timestamp);
        free(data);
        sqlite3_close(db);
        return 1;
    }


    /*Ugly code!*/
    insertsql = sqlite3_mprintf("CREATE TABLE IF NOT EXISTS BTC(LastPrice TEXT, Timestamp TEXT); INSERT INTO BTC VALUES('%q', '%q')", lastPrice->valuestring, timestamp->valuestring);

    rcode = sqlite3_exec(db, insertsql, 0, 0, &err_msg);

    if(rcode != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);
        free(data);
        cJSON_Delete(lastPrice);
        cJSON_Delete(timestamp);
        return 1;
    }
    /*sqlite3*/




    free(data);
    cJSON_Delete(lastPrice);
    cJSON_Delete(timestamp);
    sqlite3_free(insertsql);
    sqlite3_close(db);
}