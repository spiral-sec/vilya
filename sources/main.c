
#include "format_elf.h"
#include <stdio.h>
#include "vilya.h"

int main(int argc, char *argv[])
{
    file_t target = {0};
    input_t user_input = {0};

    if (!has_valid_input(argc, argv, &user_input)) {
        fprintf(stderr, "%s: Please provide a valid input.\n", argv[0]);
        return 1;
    } else if (!read_elf(&target, &user_input)) {
        LOG_IF(user_input.verbose, "Could not parse ELF: %s", user_input.filepath);
        return 1;
    }
    return 0;
}
