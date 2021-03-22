// SPDX-License-Identifier: Apache-2.0
/* Copyright 2021 IBM Corp.*/
#ifndef EDK2_SVC_SKIBOOT_H
#define EDK2_SVC_SKIBOOT_H
#include <stdint.h> //for uint_16 stuff like that
#include "external/skiboot/include/secvar.h" //for secvar struct
#include "generic.h"
#include "external/extraMbedtls/include/generate-pkcs7.h"
#include "secvar/include/general_secvar_cmds.h"
#include "external/skiboot/include/edk2.h" //include last or else problems from pragma pack(1)

#ifndef POWERNV_SECVARPATH
#define POWERNV_SECVARPATH "/sys/firmware/secvar/vars/"
#endif

#define POWERNV_VARIABLES  (const char* []){ "PK", "KEK", "db", "dbx", "TS" }

int performVerificationCommand(int argc, char *argv[]); 
int performWriteCommand(int argc, char *argv[]);
int performValidation(int argc, char* argv[]); 
int performGenerateCommand(int argc, char* argv[]);

void printTimestamp(struct efi_time t);

size_t get_pkcs7_len(const struct efi_variable_authentication_2 *auth);

int updateVar(const char* path, const char* var, const unsigned char* buff, size_t size);

int validateAuth(const unsigned char *authBuf, size_t buflen, const char *key);
int validateESL(const unsigned char *eslBuf, size_t buflen, const char *key);
int validateCert(const unsigned char *authBuf, size_t buflen, const char *varName);
int validatePKCS7(const unsigned char *cert_data, size_t len);
int validateTS(const unsigned char *data, size_t size);
int validateTime(struct efi_time *time);


int readFileFromSysfs_powernv(char **outData, size_t *outSize, const char *path, const char *variable);
#ifdef NO_CRYPTO
extern struct command edk2_compat_command_table[4];
#else
extern struct command edk2_compat_command_table[5];
#endif


#endif
