// SPDX-License-Identifier: Apache-2.0
/* Copyright 2021 IBM Corp.*/
#include <stdio.h>
#include <string.h>
#include <sys/stat.h> // needed for stat struct for file info
#include <stdlib.h>
#include "prlog.h"
#include "secvarctl.h"

int verbose = PR_WARNING;
struct backend* current_backend = NULL;
static struct backend *getDesiredBackend(int backendDesired);

void printCommandOptions(size_t countCmds, struct command *commands) 
{
	for (size_t i = 0; i < countCmds; i++) {
		printf("\t%-12s%s\n\t\tuse 'secvarctl %s --usage/help' for more information\n", commands[i].name, commands[i].short_desc, commands[i].name );
	}
}

void usage(int backend) 
{
	backend = backend >= UNKNOWN_BACKEND ? POWERNV_BACKEND : backend;
	printf("USAGE: \n\t\tsecvarctl [COMMAND]\n\tor:\tsecvarctl --efivars [COMMAND]\n"
		"A command line tool for simplifying the reading and writing of secure boot variables.\n"
		"\t--efivars\tEXPERIMENTAL: assumes x86 EFI secure boot backend, default POWER Secure Boot\n"
		"COMMANDs:\n"
		"\t--help/--usage\n"
		);
		printCommandOptions(backends[backend].countCmds, backends[backend].commands);
	

}

	

 
int main(int argc, char *argv[])
{
	int rc, i;
	char *subcommand = NULL;
	int backendDesired = UNKNOWN_BACKEND;
	
	if (argc < 2) {
		usage(backendDesired);
		return ARG_PARSE_FAIL;
	}
	argv++;
	argc--;
	for (; argc > 0 && *argv[0] == '-'; argc--, argv++) {
		if (!strcmp(*argv, "--usage")) {
			usage(backendDesired);
			return SUCCESS;
		}
		else if (!strcmp(*argv, "--help")) {
			usage(backendDesired);
			return SUCCESS;
		}
		if (!strcmp(*argv, "-v")) {
			verbose = PR_DEBUG;
		}
		if (!strcmp(*argv, "--efivars")) {
			backendDesired = EFI_BACKEND;
		}
	}
	if (argc <= 0) {
		prlog(PR_ERR,"ERROR: No command found\n");
		usage(backendDesired);
		return ARG_PARSE_FAIL;
	} 

	// if backend is not power print continuing despite some funtionality not working 
	current_backend = getDesiredBackend(backendDesired);
	if (!current_backend) { 
		current_backend = &backends[POWERNV_BACKEND];
		prlog(PR_WARNING, "WARNING: Unsupported backend detected, assuming %s backend\nRead/write may not work as expected\n\n", current_backend->name);
	}
	// next command should be one of main subcommands
	subcommand = *argv; 
	argv++;
	argc--;

	rc = UNKNOWN_COMMAND;
	for (i = 0; i < current_backend->countCmds; i++) {
		if (!strncmp(subcommand, current_backend->commands[i].name, 32)) {
			rc = current_backend->commands[i].func(argc, argv);
			break;
		}
	}
	if (rc == UNKNOWN_COMMAND) {
		prlog(PR_ERR, "ERROR:Unknown command %s\n", subcommand);
		usage(backendDesired);
	}
	
	return rc;
}

//check if POWER machine has host secure boot enabled 
static int isPOWERNVBackend()
{
	char *buff = NULL, *secVarFormatLocation = "/sys/firmware/secvar/format";
	size_t buffSize;
	int rc = BACKEND_ID_FAIL;
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
	if (!strncmp(buff, backends[POWERNV_BACKEND].name, strlen(backends[POWERNV_BACKEND].name))) {
		prlog(PR_NOTICE, "Found Backend %s\n", backends[POWERNV_BACKEND].name);
		rc = SUCCESS;
		goto out;
	}
	
	prlog(PR_WARNING, "WARNING!! %s  does not contain known POWER backend format.\n", secVarFormatLocation);

out:
	if (buff) 
		free(buff);

	return rc;
}

//check that x86 machine has EFI secure boot enables
static int isEFIBackend() 
{
	int rc;
	struct stat statbuf;
	// a hack, efivarfs can be anywhere.
	// test for dir, not PK, as PK could be absent.
	rc = stat("/sys/firmware/efi/efivars/", &statbuf);

	if (rc == 0 && (statbuf.st_mode & S_IFMT) == S_IFDIR) {
		prlog(PR_NOTICE, "Found Backend %s\n", backends[EFI_BACKEND].name);
		return SUCCESS;
	}
	return BACKEND_ID_FAIL;
}

/*
 *Checks if the desired backend is what backend the platform is running
 *@param backendDesired, value from enum backendsIndex, if UNKNOWN_BACKEND assumes POWERNV
 *@return type of backend, or NULL if file could not be found or contained wrong contents,
 */
static struct backend *getDesiredBackend(int backendDesired)
{
	struct backend *result = NULL;
	//if user desires efivarfs with --efivars then check that they have the necessary requirements
	if (backendDesired == EFI_BACKEND && isEFIBackend() == SUCCESS)
		result = &backends[EFI_BACKEND];
	//else assume they want a POWER environment, only power envireonment at the moment is powernv
	else if  (isPOWERNVBackend() == SUCCESS)
		result = &backends[POWERNV_BACKEND];

	return result;
}

