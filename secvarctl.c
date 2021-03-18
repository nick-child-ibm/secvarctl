// SPDX-License-Identifier: Apache-2.0
/* Copyright 2021 IBM Corp.*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "prlog.h"
#include "secvarctl.h"

int verbose = PR_WARNING;
static struct backend *getBackend();

static struct backend backends [] = {
	{ .name = "ibm,edk2-compat-v1", .countCmds = sizeof(edk2_compat_command_table) / sizeof(struct command), .commands = edk2_compat_command_table },
};

void printCommandOptions(size_t countCmds, struct command *commands) 
{
	for (size_t i = 0; i < countCmds; i++) {
		printf("\t%-12s %-12s\n", commands[i].name,  commands[i].short_desc );
	}
}

void usage(struct backend *backend) 
{
	printf("USAGE: \n\t$ secvarctl [COMMAND]\n"
		"A command line tool for simplifying the reading and writing of secure boot variables.\n"
		"COMMANDs:\n"
		"\t--help/--usage\n");
		printCommandOptions(backend->countCmds, backend->commands);
	

}

	

 
int main(int argc, char *argv[])
{
	int rc, i;
	char *subcommand = NULL;
	//set default backend to powernv so usage/help work
	struct backend *backend = &backends[0];
	
	if (argc < 2) {
		usage(backend);
		return ARG_PARSE_FAIL;
	}
	argv++;
	argc--;
	for (; argc > 0 && *argv[0] == '-'; argc--, argv++) {
		if (!strcmp(*argv, "--usage")) {
			usage(backend);
			return SUCCESS;
		}
		else if (!strcmp(*argv, "--help")) {
			usage(backend);
			return SUCCESS;
		}
		if (!strcmp(*argv, "-v")) {
			verbose = PR_DEBUG;
		}
	}
	if (argc <= 0) {
		prlog(PR_ERR,"ERROR: No command found\n");
		return ARG_PARSE_FAIL;
	} 

	// if backend is not power print continuing despite some funtionality not working 
	backend = getBackend();
	if (!backend) { 
		prlog(PR_WARNING, "WARNING: Unsupported backend detected, assuming ibm,edk2-compat-v1 backend\nRead/write may not work as expected\n for EFI Secure Boot\n");
		backend = &backends[0];
	}
	// next command should be one of main subcommands
	subcommand = *argv; 
	argv++;
	argc--;

	rc = UNKNOWN_COMMAND;
	for (i = 0; i < backend->countCmds; i++) {
		if (!strncmp(subcommand, backend->commands[i].name, 32)) {
			rc = backend->commands[i].func(argc, argv);
			break;
		}
	}
	if (rc == UNKNOWN_COMMAND) {
		prlog(PR_ERR, "ERROR:Unknown command %s\n", subcommand);
		usage(backend);
	}
	
	return rc;
}

/*
 *Checks what backend the platform is running, CURRENTLY ONLY KNOWS EDK2
 *@return type of backend, or NULL if file could not be found or contained wrong contents,
 */
static struct backend *getBackend()
{
	char *buff = NULL, *secVarFormatLocation = "/sys/firmware/secvar/format";
	size_t buffSize;
	struct backend *result = NULL;
	// if file doesnt exist then print warning and keep going
	if (isFile(secVarFormatLocation)) {
		prlog(PR_WARNING, "WARNING!! Platform does not support secure variables\n");
		goto out;
	}
	buff = getDataFromFile(secVarFormatLocation, &buffSize);
	if (!buff) {
		prlog(PR_WARNING, "WARNING!! Could not extract data from %s , assuming platform does not support secure variables\n", secVarFormatLocation);
		goto out;
	}
	//loop through all known backends
	for (int i = 0; i < sizeof(backends) / sizeof(struct backend); i++) {
		if (!strncmp(buff, backends[i].name, strlen(backends[i].name))) {
			prlog(PR_NOTICE, "Found Backend %s\n", backends[i].name);
			result = &backends[i];
			goto out;
		}
	}
	prlog(PR_WARNING, "WARNING!! %s  does not contain known backend format.\n", secVarFormatLocation);

out:
	if (buff) 
		free(buff);

	return result;

}

