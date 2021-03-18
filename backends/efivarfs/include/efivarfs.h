#ifndef EFI_VAR_FS_H
#define EFI_VAR_FS_H
#include "generic.h"

#ifndef EFI_SECVARPATH
#define EFI_SECVARPATH "/sys/firmware/efi/efivars/"
#endif
extern struct command efi_var_command_table[1];
int foo();
#endif