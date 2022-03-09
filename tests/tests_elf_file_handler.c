
#include <criterion/criterion.h>
#include "vilya.h"

Test(is_valid_elf, parse_a_valid_elf)
{
    file_t result = {0};
    input_t settings = {0};

    strncpy(settings.target, "./unit_tests", DEFAULT_BUFFER_SIZE);
    cr_assert_eq(is_valid_elf(&result, &settings), 1);
}

Test(is_valid_elf, invalid_elf)
{
    file_t result = {0};
    input_t settings = {0};

    strncpy(settings.target, "./Makefile", DEFAULT_BUFFER_SIZE);
    cr_assert_eq(is_valid_elf(&result, &settings), 0);
}

Test(is_valid_elf, unknown_file)
{
    file_t result = {0};
    input_t settings = {0};

    strncpy(settings.target, "gamer", DEFAULT_BUFFER_SIZE);
    cr_assert_eq(is_valid_elf(&result, &settings), 0);
}
