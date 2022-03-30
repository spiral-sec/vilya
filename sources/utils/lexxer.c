
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"
#include "vilya.h"

static void display_help(char const *bin_name)
{
    dprintf(STDERR_FILENO, "%s: simple polymorphic ELF packer.\n\n", bin_name);
    dprintf(STDERR_FILENO, "Usage:\n\t--verbose -v\t\tenables verbose mode\n");
    dprintf(STDERR_FILENO, "\t--help -h\t\tdisplays this message\n");
}

int has_valid_input(int argc, char **argv, input_t *settings)
{
    int opt = 0;
    static struct option long_options[] = {
    {"verbose", no_argument, 0, 'v'}, {"help", no_argument, 0, 'h'}, {0, 0, 0, 0}};

    while ((opt = getopt_long(argc, argv, "vh", long_options, NULL)) != -1) {
        switch (opt) {
        case 'v':
            LOG_IF(settings->verbose, "Setting verbose to TRUE");
            settings->verbose = 1;
            break;
        case 'h':
            LOG_IF(settings->verbose, "Requesting HELP dialog");
            display_help(argv[0]);
            _exit(0);
        case '?': break;
        }
    }
    if (argc < optind)
        return 0;
    strncpy(settings->filepath, argv[optind], DEFAULT_BUFFER_SIZE);
    return 1;
}
