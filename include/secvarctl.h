// SPDX-License-Identifier: Apache-2.0
/* Copyright 2021 IBM Corp.*/
#ifndef SECVARCTL_H
#define SECVARCTL_H
#include <stdint.h> //for uint_16 stuff like that
#include "err.h"
#include "prlog.h"
#include "generic.h"
#include "backends/powernv/include/edk2-svc.h"
#include "backends/efivarfs/include/efivarfs.h"


/*static struct backend powernv_backend = { 
    .name = "ibm,edk2-compat-v1", 
    .countCmds = sizeof(edk2_compat_command_table) / sizeof(struct command), 
    .commands = edk2_compat_command_table,
    .default_secvar_path = POWERNV_SECVARPATH,
    .sb_variables = POWERNV_VARIABLES

};
static struct backend efivarfs_backend = { 
    .name = "efi_vars", 
    .countCmds = sizeof(efi_var_command_table) / sizeof(struct command), 
    .commands = efi_var_command_table,
    .default_secvar_path = EFI_SECVARPATH,
    .sb_variables = EFI_VARIABLES 
};
*/
//array of currently supported backends and their commands/info
struct backend backends [] = {
    //power_nv_backend
    { 
    .name = "ibm,edk2-compat-v1", 
    .countCmds = sizeof(edk2_compat_command_table) / sizeof(struct command), 
    .commands = edk2_compat_command_table,
    .default_secvar_path = POWERNV_SECVARPATH,
    .sb_variables = POWERNV_VARIABLES,
    .sb_variables_count = ARRAY_SIZE(POWERNV_VARIABLES),
    .readFileFromSysfs = readFileFromSysfs_powernv
    },

    //efivarfs_backend
    { 
    .name = "efi_vars", 
    .countCmds = sizeof(efi_var_command_table) / sizeof(struct command), 
    .commands = efi_var_command_table,
    .default_secvar_path = EFI_SECVARPATH,
    .sb_variables = EFI_VARIABLES,
    .sb_variables_count = ARRAY_SIZE(EFI_VARIABLES),
    .readFileFromSysfs = readFileFromSysfs_efivarfs
    }
};
//enum corresponding to index of respective backend from backends[]
enum backendsIndex {
    POWERNV_BACKEND = 0,
    EFI_BACKEND,
    UNKNOWN_BACKEND 
};

extern int verbose;
extern struct backend* current_backend;

#endif
