//
// Created by Nba_Yoh on 06/06/2016.
//

#ifndef BASEHAXX_SPLOIT_INSTALLER_SAVE_H
#define BASEHAXX_SPLOIT_INSTALLER_SAVE_H

#include "globals.h"
#include <3ds.h>

unsigned short ccitt16(unsigned char *data, unsigned int length);

Result read_savedata(const char* path, void** data, size_t* size);
Result write_from_file(u8* dest, char* path, u32* file_size);
Result write_savedata(const char* path, const void* data, size_t size);

#endif //BASEHAXX_SPLOIT_INSTALLER_SAVE_H
