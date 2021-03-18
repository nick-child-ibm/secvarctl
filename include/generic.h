// SPDX-License-Identifier: Apache-2.0
/* Copyright 2021 IBM Corp.*/
#ifndef GENERIC_H
#define GENERIC_H
#include <stdlib.h>
struct command {
	char name[32];
	int (*func)(int, char**);
    char *short_desc;
};

char * getDataFromFile(const char *file, size_t* size);
int writeData(const char * file, const char * buff, size_t size);
int createFile(const char * file, const char * buff, size_t size);
void printRaw(const char* c, size_t size) ;
int isFile(const char* path);
size_t getLeadingWhitespace(unsigned char* data, size_t dataSize);
void printHex(unsigned char* data, size_t length);
#endif