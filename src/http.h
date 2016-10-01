//
// Created by Nba_Yoh on 06/06/2016.
//

#ifndef BASEHAXX_SPLOIT_INSTALLER_HTTP_H
#define BASEHAXX_SPLOIT_INSTALLER_HTTP_H

#include "globals.h"
#include <3ds.h>

Result get_redirect(char *url, char *out, size_t out_size, char *user_agent);
Result download_file(httpcContext *context, u8** buffer, u32* size);
Result download_payload(u8** payload_buffer, u32* payload_size, int* firmware_version);

#endif //BASEHAXX_SPLOIT_INSTALLER_HTTP_H
