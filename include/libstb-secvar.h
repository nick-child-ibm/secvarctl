//The following has been taken from edk2-compat-process.h and retains its license
#define key_equals(a,b) (!strncmp(a, b, EDK2_MAX_KEY_LEN))
#define uuid_equals(a,b) (!memcmp(a, b, UUID_SIZE))
#define EDK2_MAX_KEY_LEN        SECVAR_MAX_KEY_LEN
//the following has been taken from secvar.h and retains its license
#define SECVAR_MAX_KEY_LEN		1024

#include "external/skiboot/include/endian.h"
