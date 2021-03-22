// SPDX-License-Identifier: Apache-2.0
/* Copyright 2021 IBM Corp.*/
#include "backends/efivarfs/include/efivarfs.h"
struct command efi_var_command_table[] = {
    { .name = "read", .func = performReadCommand, .short_desc = "prints info on secure variables" },
};

struct translation_table variable_renames[] = {
    { .from = "PK", .to = "PK-8be4df61-93ca-11d2-aa0d-00e098032b8c" },
    { .from = "db", .to = "db-d719b2cb-3d3a-4596-a3bc-dad00e67656f" },
    { .from = "dbx", .to = "dbx-d719b2cb-3d3a-4596-a3bc-dad00e67656f" },
    { .from = "KEK", .to = "KEK-8be4df61-93ca-11d2-aa0d-00e098032b8c" },
};