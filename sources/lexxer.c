
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "elf.h"
#include "vilya.h"

static int is_valid_arch(char const *requested_arch, architecture_t *arch)
{
    int ctr = -1;
    const char *supported_architectures[] = {"x86_64", "x64_x64_x32", NULL};

    while (!strcmp(supported_architectures[++ctr], requested_arch))
        ;
    *arch = (architecture_t)ctr;
    return (*arch != UNKNOWN);
}

static int is_valid_file(const char *filepath, file_t *result)
{
    struct stat st;

    if (filepath == NULL || *filepath == 0)
        return 0;
    else if (stat(filepath, &st) && st.st_mode & S_IXUSR)
        return is_valid_elf(filepath, st, result);
    return 0;
}

static void display_help(char const *bin_name)
{
    // TODO: show supported_architectures[] here
    dprintf(STDERR_FILENO, "%s: simple polymorphic ELF packer.\n\n", bin_name);
    dprintf(STDERR_FILENO, "Usage:\n\t--verbose -v\t\tenables verbose mode\n");
    dprintf(STDERR_FILENO, "\t--help -h\t\tdisplays this message\n");
    dprintf(STDERR_FILENO, "\t--arch -a\t\tforces a specific architecture\n");
}

int has_valid_input(int argc, char **argv, file_t *parsed, input_t *settings)
{
    int opt = 0;
    int index = 0;
    static struct option long_options[] = {{"verbose", no_argument, 0, 'v'},
    {"help", no_argument, 0, 'h'}, {"arch", required_argument, 0, 'a'}, {0, 0, 0, 0}};

    while ((opt = getopt_long(argc, argv, "vha:", long_options, &index)) != -1) {
        switch (opt) {
        case 'v':
            LOG_IF_VERBOSE(settings->verbose, "Setting verbose to TRUE");
            settings->verbose = 1;
            break;
        case 'h':
            LOG_IF_VERBOSE(settings->verbose, "Requesting HELP dialog");
            display_help(argv[0]);
            _exit(0);
        case 'a':
            LOG_IF_VERBOSE(settings->verbose, "architecture should be %s", optarg);
            if (!is_valid_arch(optarg, &parsed->arch)) {
                LOG("unsupported architecture: %s.\n", optarg);
                _exit(1);
            } else
                break;
        case '?': LOG_IF_VERBOSE(settings->verbose, "unknown option: %s", optarg); return 0;
        }
    }
    return is_valid_file(argv[index], parsed);
}
