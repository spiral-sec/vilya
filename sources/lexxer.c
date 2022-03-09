
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "elf.h"
#include "vilya.h"

static int is_valid_arch(char const *requested_arch, input_t *settings)
{
    int ctr = -1;
    const char *supported_architectures[] = {"x86-64", "x64-x64-x32", NULL};

    while (supported_architectures[++ctr] != NULL) {
        if (!strcmp(requested_arch, supported_architectures[ctr])) {
            settings->requested = (architecture_t)ctr;
            return 1;
        }
    }
    return 0;
}

static void display_help(char const *bin_name)
{
    dprintf(STDERR_FILENO, "%s: simple polymorphic ELF packer.\n\n", bin_name);
    dprintf(STDERR_FILENO, "Usage:\n\t--verbose -v\t\tenables verbose mode\n");
    dprintf(STDERR_FILENO, "\t--help -h\t\tdisplays this message\n");
    dprintf(STDERR_FILENO, "\t--arch -a\t\tforces a specific architecture\n");
}

int has_valid_input(int argc, char **argv, input_t *settings)
{
    int opt = 0;
    static struct option long_options[] = {{"verbose", no_argument, 0, 'v'},
    {"help", no_argument, 0, 'h'}, {"arch", required_argument, 0, 'a'}, {0, 0, 0, 0}};

    while ((opt = getopt_long(argc, argv, "vha:", long_options, NULL)) != -1) {
        switch (opt) {
        case 'v':
            LOG_IF(settings->verbose, "Setting verbose to TRUE");
            settings->verbose = 1;
            break;
        case 'h':
            LOG_IF(settings->verbose, "Requesting HELP dialog");
            display_help(argv[0]);
            _exit(0);
        case 'a':
            LOG_IF(settings->verbose, "architecture should be %s", optarg);
            if (!is_valid_arch(optarg, settings)) {
                LOG("unsupported architecture: %s.\n", optarg);
                return 0;
            } else
                break;
        case '?': break;
        }
    }
    if (argc < optind)
        return 0;
    strncpy(settings->target, argv[optind], DEFAULT_BUFFER_SIZE);
    return 1;
}
