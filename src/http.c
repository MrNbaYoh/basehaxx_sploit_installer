//
// Created by Nba_Yoh on 06/06/2016.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http.h"

Result get_redirect(char *url, char *out, size_t out_size, char *user_agent)
{
    Result ret;

    httpcContext context;
    ret = httpcOpenContext(&context, HTTPC_METHOD_GET, url, 0);
    if(R_FAILED(ret)) return ret;

    ret = httpcAddRequestHeaderField(&context, "User-Agent", user_agent);
    if(R_SUCCEEDED(ret)) ret = httpcBeginRequest(&context);

    if(R_FAILED(ret))
    {
        httpcCloseContext(&context);
        return ret;
    }

    ret = httpcGetResponseHeader(&context, "Location", out, out_size);
    httpcCloseContext(&context);

    return 0;
}

Result download_file(httpcContext *context, u8** buffer, u32* size)
{
    Result ret;

    ret = httpcBeginRequest(context);
    if (R_FAILED(ret)) return ret;

    u32 status_code = 0;
    ret = httpcGetResponseStatusCode(context, &status_code);
    if (R_FAILED(ret)) return ret;

    if (status_code != 200) return -1;

    u32 sz = 0;
    ret = httpcGetDownloadSizeState(context, NULL, &sz);
    if (R_FAILED(ret)) return ret;

    void *buf = malloc(sz);
    if (!buf) return -2;

    memset(buf, 0, sz);

    ret = httpcDownloadData(context, buf, sz, NULL);
    if (R_FAILED(ret))
    {
        free(buf);
        return ret;
    }

    if(size) *size = sz;
    if(buffer) *buffer = buf;
    else free(buf);

    return 0;
}

Result download_payload(u8** payload_buffer, u32* payload_size, int* firmware_version)
{
    Result ret = 0;
    httpcContext context;
    static char in_url[512];
    static char out_url[512];

    snprintf(in_url, sizeof(in_url) - 1, "http://smea.mtheall.com/get_payload.php?version=%s-%d-%d-%d-%d-%s",
             firmware_version[0] ? "NEW" : "OLD", firmware_version[1], firmware_version[2], firmware_version[3], firmware_version[4], regions[firmware_version[5]]);

    char user_agent[64];
    snprintf(user_agent, sizeof(user_agent) - 1, "basehaxx_sploit_installer");
    ret = get_redirect(in_url, out_url, 512, user_agent);
    if(R_FAILED(ret))
    {
        sprintf(status, "An error occured! Failed to grab payload url\n    Error code: %08lX", ret);
        return ret;
    }

    //strcpy(out_url, "http://smealum.github.io/ninjhax2/JL1Xf2KFVm/otherapp/POST5_E_24576_9221.bin\0");
    ret = httpcOpenContext(&context, HTTPC_METHOD_GET, out_url, 0);
    if(R_FAILED(ret))
    {
        sprintf(status, "An error occured! Failed to open http context\n    Error code: %08lX", ret);
        return ret;
    }

    ret = download_file(&context, payload_buffer, payload_size);
    if(R_FAILED(ret))
        sprintf(status, "An error occured! Failed to download payload\n    Error code: %08lX", ret);

    return ret;
}