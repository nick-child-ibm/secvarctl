// SPDX-License-Identifier: Apache-2.0
/* Copyright 2021 IBM Corp.*/
#ifndef EFI_VAR_FS_H
#define EFI_VAR_FS_H
#include "generic.h"
#include "secvar/include/general_secvar_cmds.h"

#ifndef EFI_SECVARPATH
#define EFI_SECVARPATH "/sys/firmware/efi/efivars/"
#endif
#define EFI_VARIABLES (const char* []){ "PK", "KEK", "db", "dbx"}
extern struct command efi_var_command_table[1];

struct translation_table {
    char *from;
    char *to;
};

extern struct translation_table variable_renames[4];

int readFileFromSysfs_efivarfs(char **outData, size_t *outSize, const char *path, const char *variable); 
#endif