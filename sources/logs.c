
#include <string.h>
#include <time.h>

#include "vilya.h"

char *timestamp(void)
{
    time_t now = time(NULL);
    struct tm *time_tm = gmtime(&now);
    char *time = !time_tm ? NULL : asctime(time_tm);

    if (!time)
        return NULL;
    time[strlen(time) - 1] = '\0';
    return time;
}
