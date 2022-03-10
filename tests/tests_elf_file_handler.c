
#include "format_elf.h"
#include <criterion/criterion.h>
#include "vilya.h"

Test(open_file, parse_a_valid_elf)
{
    file_t result = {0};
    input_t settings = {0};

    strncpy(settings.filepath, "./unit_tests", DEFAULT_BUFFER_SIZE);
    cr_assert_eq(read_elf(&result, &settings), 1);
}

Test(open_file, invalid_elf)
{
    file_t result = {0};
    input_t settings = {0};

    strncpy(settings.filepath, "./Makefile", DEFAULT_BUFFER_SIZE);
    cr_assert_eq(read_elf(&result, &settings), 0);
}

Test(open_file, unknown_file)
{
    file_t result = {0};
    input_t settings = {0};

    strncpy(settings.filepath, "gamer", DEFAULT_BUFFER_SIZE);
    cr_assert_eq(read_elf(&result, &settings), 0);
}
