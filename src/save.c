//
// Created by Nba_Yoh on 06/06/2016.
//

#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "save.h"

#define CRC_POLY 0x1021

unsigned short ccitt16(unsigned char *data, unsigned int length)
{
    unsigned short crc = 0xFFFF;

    for (unsigned int i = 0; i < length; i++)
    {
        crc ^= (unsigned short)data[i] << 8;

        for (int j = 0; j < 8; j++)
            if (crc & 0x8000)
                crc = crc << 1 ^ CRC_POLY;
            else
                crc = crc << 1;
    }

    return crc;
}

Result read_savedata(const char* path, void** data, size_t* size)
{
    if(!path || !data || !size) return -1;

    Result ret = -1;
    int fail = 0;
    void* buffer = NULL;

    fsUseSession(save_session);
    ret = FSUSER_OpenArchive(&save_archive, ARCHIVE_SAVEDATA, (FS_Path){PATH_EMPTY, 1, (u8*)""});
    if(R_FAILED(ret))
    {
        fail = -1;
        goto readFail;
    }

    Handle file = 0;
    ret = FSUSER_OpenFile(&file, save_archive, fsMakePath(PATH_ASCII, path), FS_OPEN_READ, 0);
    if(R_FAILED(ret))
    {
        fail = -2;
        goto readFail;
    }

    u64 file_size = 0;
    ret = FSFILE_GetSize(file, &file_size);

    buffer = malloc(file_size);
    if(!buffer)
    {
        fail = -3;
        goto readFail;
    }

    u32 bytes_read = 0;
    ret = FSFILE_Read(file, &bytes_read, 0, buffer, file_size);
    if(R_FAILED(ret))
    {
        fail = -4;
        goto readFail;
    }

    ret = FSFILE_Close(file);
    if(R_FAILED(ret))
    {
        fail = -5;
        goto readFail;
    }

    readFail:
    FSUSER_CloseArchive(save_archive);
    fsEndUseSession();
    if(fail)
    {
        sprintf(status, "An error occured! Failed to read file: %d\n     %08lX %08lX", fail, ret, bytes_read);
        if(buffer) free(buffer);
    }
    else
    {
        sprintf(status, "Successfully read file.\n     %08lX               ", bytes_read);
        *data = buffer;
        *size = bytes_read;
    }

    return ret;
}

Result write_from_file(u8* dest, char* path, u32* file_size)
{
    FILE *file = fopen(path, "r");

    if(file == NULL) return -1;

    fseek(file, 0, SEEK_END);
    u32 size = ftell(file);
    *file_size = size;
    fseek(file, 0, SEEK_SET);

    u8* buffer = malloc(size);

    if(!buffer) return -2;

    fread(buffer, size, 1, file);
    fclose(file);

    memcpy(dest, buffer, size);
    free(buffer);

    return 0;
}


Result write_savedata(const char* path, const void* data, size_t size)
{
    if(!path || !data || size == 0) return -1;

    Result ret = -1;
    int fail = 0;

    fsUseSession(save_session);
    ret = FSUSER_OpenArchive(&save_archive, ARCHIVE_SAVEDATA,(FS_Path){PATH_EMPTY, 1, (u8*)""});
    if(R_FAILED(ret))
    {
        fail = -1;
        goto writeFail;
    }

    // delete file
    FSUSER_DeleteFile(save_archive, fsMakePath(PATH_ASCII, path));
    FSUSER_ControlArchive(save_archive, ARCHIVE_ACTION_COMMIT_SAVE_DATA, NULL, 0, NULL, 0);

    Handle file = 0;
    ret = FSUSER_OpenFile(&file, save_archive, fsMakePath(PATH_ASCII, path), FS_OPEN_CREATE | FS_OPEN_WRITE, 0);
    if(R_FAILED(ret))
    {
        fail = -2;
        goto writeFail;
    }

    u32 bytes_written = 0;
    ret = FSFILE_Write(file, &bytes_written, 0, data, size, FS_WRITE_FLUSH | FS_WRITE_UPDATE_TIME);
    if(R_FAILED(ret))
    {
        fail = -3;
        goto writeFail;
    }

    ret = FSFILE_Close(file);
    if(R_FAILED(ret))
    {
        fail = -4;
        goto writeFail;
    }

    ret = FSUSER_ControlArchive(save_archive, ARCHIVE_ACTION_COMMIT_SAVE_DATA, NULL, 0, NULL, 0);
    if(R_FAILED(ret)) fail = -5;

    writeFail:
    FSUSER_CloseArchive(save_archive);
    fsEndUseSession();
    if(fail) sprintf(status, "An error occured! Failed to write to file: %d\n     %08lX %08lX", fail, ret, bytes_written);
    else sprintf(status, "Successfully wrote to file!\n     %08lX               ", bytes_written);

    return ret;
}
