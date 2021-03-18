#include "backends/efivarfs/include/efivarfs.h"// include last, pragma pack(1) issue
struct command efi_var_command_table[] = {
    { .name = "foo", .func = foo, .short_desc = "this is a test" },
};

int foo() {
    return 0;
}