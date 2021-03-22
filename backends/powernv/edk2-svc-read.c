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
#include "backends/powernv/include/edk2-svc.h"

/*
 *gets the integer value from the ascii file "size"
 *@param size, the returned size of size file
 *@param path , lccation of "size" file
 *@return errror number if fail, <0
 */
static int getSizeFromSizeFile(size_t *returnSize, const char* path)
{
    int fptr, rc;
    ssize_t maxdigits = 8, read_size; 
    char *c = NULL;

    struct stat fileInfo;
    fptr = open(path, O_RDONLY);            
    if (fptr < 0) {
        prlog(PR_WARNING, "----opening %s failed : %s----\n", path, strerror(errno));
        return INVALID_FILE;
    }
    if (fstat(fptr, &fileInfo) < 0) {
        return INVALID_FILE;
    }
    if (fileInfo.st_size < maxdigits) {
        maxdigits = fileInfo.st_size;
    }
    // initiate string to empty, with null pointer
    c = calloc(maxdigits + 1, 1); 
    if (!c) {
        prlog(PR_ERR, "ERROR: failed to allocate memory\n");
        close(fptr);
        return ALLOC_FAIL;
    }
    prlog(PR_NOTICE, "----opening %s is success: reading %zd of %zd bytes----\n", path, maxdigits, fileInfo.st_size);
    read_size = read(fptr, c, maxdigits);
    if (read_size <= 0) {
        prlog(PR_ERR, "ERROR: error reading %s\n", path);
        free(c);
        close(fptr);
        return INVALID_FILE;
    }

    close(fptr);
    // turn string into base 10 int
    *returnSize = strtol(c, NULL, 0); 
    //strol likes to return zero if there is no conversion from string to int
    //so we need to differentiate an error from a file that actually contains 0
    if (*returnSize == 0 && c[0] != '0')
        rc = INVALID_FILE;
    else
        rc = SUCCESS;
    free(c);

    return rc;
}

/**
 *This function reads from the path given, assuming the sysfs is for power
 *meaning variables can be read from <path>/<varName>/data
 *additionally there is a 'size' file at <path>/<varName>/size that contains the length of the data file
 *@param outData , contains the raw buffer of data contained in the sec var
 *@param outSize, the length of the returned data
 *@param path , the path to the secure variable subdirectory, usually /sys/firmware/secvar/vars/
 *@param variable , the variable name ex: 'PK'
 *@return SUCCESS if reading file was successfull, else error nummber
 *NOTE: REMEMBER TO DEALLOC outData
 */
int readFileFromSysfs_powernv(char **outData, size_t *outSize, const char *path, const char *variable) 
{
    int rc, fptr;
    ssize_t read_size;
    char *sizePath = NULL, *fullPath = NULL;
    struct stat fileInfo;
    
    //generate full path to file, for powernv = <path>/<varName>/data
    fullPath = malloc(strlen(path) + strlen(variable) + strlen("/data") + 1);
    if (!fullPath) { 
        prlog(PR_ERR, "ERROR: failed to allocate memory\n");
        return ALLOC_FAIL;
    }
    strcpy(fullPath, path);
    strcat(fullPath, variable);
    strcat(fullPath, "/data");
    rc = isFile(fullPath);
    if (rc) {
        goto out;
    }

    sizePath = malloc( strlen(fullPath) + 1);
    if (!sizePath) {
        prlog(PR_ERR, "ERROR: failed to allocate memory\n");
        return ALLOC_FAIL;
    }
    // since we are reading from a powernv secvar, it can be assumed it has a <var>/size file for more accurate size
    // fullPath currently holds <path>/<var>/data we are going to take off data and add size to get the desired file
    strcpy(sizePath, fullPath);
    //add null terminator so strncat works
    sizePath[strlen(sizePath) - strlen("data")] = '\0';
    strncat(sizePath, "size", strlen("size") + 1); 
    rc = getSizeFromSizeFile(outSize, sizePath);
    if (rc < 0) {
        prlog(PR_WARNING, "ERROR: Could not get size of variable, TIP: does %s exist?\n", sizePath);
        rc = INVALID_FILE;
        goto out;
    }

    if (*outSize == 0) {
        prlog(PR_WARNING, "Secure Variable has size of zero, (specified by size file)\n");
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
    // if file size is less than expeced size, error
    if (fileInfo.st_size < *outSize) {
        prlog(PR_ERR, "ERROR: expected size (%zd) is less than actual size (%ld)\n", *outSize, fileInfo.st_size);
        rc = INVALID_FILE;
        goto out;
    }
    prlog(PR_NOTICE,"-----opening %s is success: reading %zd bytes-----\n", fullPath, *outSize);
    *outData = malloc(*outSize);
    if (!*outData) {
        prlog(PR_ERR, "ERROR: failed to allocate memory\n");
        rc = ALLOC_FAIL;  
        goto out;
    }

    read_size = read(fptr, *outData, *outSize);
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
    if (sizePath) free(sizePath);
    if (fullPath) free(fullPath);

    return rc;
}