
#include <stdio.h>
#include "vilya.h"

int main(int argc, char *argv[])
{
    file_t target = {0};
    input_t user_input = {0};

    if (!has_valid_input(argc, argv, &target, &user_input)) {
        fprintf(stderr, "%s: Please provide a valid input.\n", argv[0]);
        return 1;
    }

    return 0;
}
