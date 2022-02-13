
#include <getopt.h>
#include <unistd.h>
#include "vilya.h"

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
            // TODO display_help
            _exit(0);
        case 'a':
            LOG_IF_VERBOSE(settings->verbose, "architecture should be %s", optarg);
            // TODO check if architecture valid
            break;
        case '?': LOG_IF_VERBOSE(settings->verbose, "unknown option: %s", optarg); return 0;
        }
    }
    return 1;
}
