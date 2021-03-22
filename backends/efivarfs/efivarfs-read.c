// SPDX-License-Identifier: Apache-2.0
/* Copyright 2021 IBM Corp.*/
#include <sys/stat.h> // needed for stat struct for file info
#include <sys/types.h>
#include <fcntl.h> // O_RDONLY
#include <unistd.h> // has read/open funcitons
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "backends/efivarfs/include/efivarfs.h"

/**
 *This function reads from the path given, assuming the sysfs is for efivarfs
 *meaning variables can be read from <path>/<varName>-<guid>
 *@param outData , contains the raw buffer of data contained in the sec var
 *@param outSize, the length of the returned data
 *@param path , the path to the secure variable subdirectory, usually /sys/firmware/efi/efivars
 *@param variable , the variable name ex: 'PK'
 *@return SUCCESS if reading file was successfull, else error nummber
 *NOTE: REMEMBER TO DEALLOC outData
 */
int readFileFromSysfs_efivarfs(char **outData, size_t *outSize, const char *path, const char *variable) 
{
    int rc, fptr;
    char *fullPath = NULL, *rename = NULL;
    struct stat fileInfo;
    size_t read_size;

    for (int i = 0; i < ARRAY_SIZE(variable_renames); i++) {
        if (strcmp(variable, variable_renames[i].from) == 0) {
            rename = variable_renames[i].to;
            break;
        }
    }
    if (!rename) {
        prlog(PR_ERR, "ERROR: Unknown variable %s\n", variable);
        return INVALID_VAR_NAME;
    }

    fullPath = malloc(strlen(path) + strlen(rename) + 1);
    if (!fullPath) { 
        prlog(PR_ERR, "ERROR: failed to allocate memory\n");
        return ALLOC_FAIL;
    }

    strcpy(fullPath, path);
    strcat(fullPath, rename);
    rc = isFile(fullPath);
    if (rc) {
        goto out;
    } 

    fptr = open(fullPath, O_RDONLY);            
    if (fptr < 0) {
        prlog(PR_WARNING,"-----opening %s failed: %s-----\n\n", fullPath, strerror(errno));
        rc = INVALID_FILE;
        goto out;
    }
    if (fstat(fptr, &fileInfo) < 0) {
        rc = INVALID_FILE;
        goto out;
    }
    // if file size is zero, warn . if less than zero, error
    *outSize = fileInfo.st_size;
    if (*outSize == 0) {
        prlog(PR_WARNING, "Secure Variable has size of zero\n");
    }
    else if (*outSize < 0) {
        prlog(PR_ERR, "ERROR: could not retrieve size of secvar in %s\n", fullPath);
        rc = INVALID_FILE;
        goto out;
    }

    prlog(PR_NOTICE,"-----opening %s is success: reading %zd bytes----- \n", fullPath, *outSize);
    *outData = malloc(*outSize);
    if (!*outData) {
        prlog(PR_ERR, "ERROR: failed to allocate memory\n");
        rc = ALLOC_FAIL;  
        goto out;
    }
    //the first four bytes of efi variables are UEFI variable attributes, we will iterate past this
    *outSize -= 4;
    read_size = pread(fptr, *outData, *outSize, 4);
    if (read_size != *outSize) {
        prlog(PR_ERR, "ERROR: did not read all data of %s in one go\n", fullPath);
        free(*outData);
        close(fptr);
        rc = INVALID_FILE;
        goto out;
    }
    close(fptr);
    if (!*outData) {
        prlog(PR_ERR, "ERROR: no data in file\n");
        rc = INVALID_FILE;
    }
out:
    if (fullPath) free(fullPath);

    return rc;
}