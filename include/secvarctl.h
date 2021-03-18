// SPDX-License-Identifier: Apache-2.0
/* Copyright 2021 IBM Corp.*/
#ifndef SECVARCTL_H
#define SECVARCTL_H
#include <stdint.h> //for uint_16 stuff like that
#include "err.h"
#include "prlog.h"
#include "backends/powernv/include/edk2-svc.h"
#include "backends/efivarfs/include/efivarfs.h"


struct backend {
	char name[32];
	size_t countCmds;
	struct command *commands;
};

extern int verbose;


#endif
