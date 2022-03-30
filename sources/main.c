
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include "utils.h"
#include "vilya.h"

int main(int argc, char *argv[])
{
    file_t target = {0};
    input_t user_input = {0};

    strncpy(user_input.packer_filepath, argv[0], DEFAULT_BUFFER_SIZE);
    if (!has_valid_input(argc, argv, &user_input)) {
        fprintf(stderr, "%s: Please provide a valid input.\n", argv[0]);
        return 1;
    } else if (!parse(&user_input, &target)) {
        fprintf(stderr, "Could not validate file.\n");
        return 1;
    }
    dump_ehdr(target.binary_dump, target.binary_dump_size);
    return 0;
}
