
#include <criterion/criterion.h>
#include "utils.h"
#include "vilya.h"

Test(has_valid_input, default_case)
{
    input_t settings = {0};
    const int argc = 2;
    const char *argv[] = {"./vilya", "./vilya", NULL};

    cr_assert_eq(has_valid_input(argc, (char **)argv, &settings), 1);
    cr_assert_eq(settings.verbose, 0);
}

Test(has_valid_input, require_verbose)
{
    input_t settings = {0};
    const int argc = 3;
    const char *argv[] = {"./vilya", "-v", "./vilya", NULL};

    cr_assert_eq(has_valid_input(argc, (char **)argv, &settings), 1);
    cr_assert_neq(settings.verbose, 0);
}
