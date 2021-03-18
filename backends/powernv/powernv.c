#include "backends/powernv/include/edk2-svc.h"// include last, pragma pack(1) issue
#include "generic.h"
struct command edk2_compat_command_table[] = {
    { .name = "read", .func = performReadCommand, .short_desc = "prints info on secure variables" },
    { .name = "write", .func = performWriteCommand, .short_desc =  "updates secure variable with new auth"},
    { .name = "validate", .func = performValidation, .short_desc = "validates format of given esl/cert/auth" },
    { .name = "verify", .func = performVerificationCommand, .short_desc = "compares proposed variable to the current variables" },
#ifndef NO_CRYPTO
    { .name = "generate", .func = performGenerateCommand, .short_desc = "creates relevant files for secure variable management" }
#endif
};