// SPDX-License-Identifier: Apache-2.0
/* Copyright 2021 IBM Corp.*/
#ifndef  GENERAL_SECVAR_CMDS_H
#define GENERAL_SECVAR_CMDS_H
#include "err.h"
#include "prlog.h"
#include <mbedtls/x509_crt.h> // for reading certdata
#include "include/generic.h"
#include "external/skiboot/include/secvar.h" //okay this does look a bit backend specific, TODO!
#include "external/skiboot/include/edk2.h" //This file is not skiboot specific, it just comes from the skiboot source

#define ARRAY_SIZE(a) (sizeof (a) / sizeof ((a)[0]))
#define uuid_equals(a,b) (!memcmp(a, b, UUID_SIZE))
#define CERT_BUFFER_SIZE        2048

extern struct backend* current_backend;
// array holding different hash function information
static const struct hash_funct {
    char name[8];
    size_t size;
    int mbedtls_funct;
    uuid_t const* guid;
} hash_functions[] = {
    { .name = "SHA1", .size = 20 , .mbedtls_funct = MBEDTLS_MD_SHA1, .guid = &EFI_CERT_SHA1_GUID },
    { .name = "SHA224", .size = 28 , .mbedtls_funct = MBEDTLS_MD_SHA224, .guid = &EFI_CERT_SHA224_GUID },
    { .name = "SHA256", .size = 32, .mbedtls_funct = MBEDTLS_MD_SHA256, .guid = &EFI_CERT_SHA256_GUID },
    { .name = "SHA384", .size = 48, .mbedtls_funct = MBEDTLS_MD_SHA384, .guid = &EFI_CERT_SHA384_GUID },
    { .name = "SHA512", .size = 64, .mbedtls_funct = MBEDTLS_MD_SHA512, .guid = &EFI_CERT_SHA512_GUID },
};
//located in read.c
int performReadCommand(int argc, char *argv[]);
int printCertInfo(mbedtls_x509_crt *x509);
void printESLInfo(EFI_SIGNATURE_LIST *sigList);
void printGuidSig(const void *sig);
EFI_SIGNATURE_LIST* get_esl_signature_list(const char *buf, size_t buflen);
ssize_t get_esl_cert( const char *c,EFI_SIGNATURE_LIST *list ,char **cert);
const char* getSigType(const uuid_t);
int getSecVar(struct secvar **var, const char* name, const char *path);
void printTimestamp(struct efi_time t);


//located in util.c
int isVariable(const char *var);
int parseX509(mbedtls_x509_crt *x509, const unsigned char *certBuf, size_t buflen);
int getVariableString(char **outString, size_t var_count) ;
#endif
