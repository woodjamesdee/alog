#include "../alog.h"
#include <stdlib.h>

/* gcc alog.c test/sync_tests.c -o sync_test --std=c89 */

int main()
{
    alog_conf conf;
    conf.flags = ALOG_CONF_NEWLINE;
    conf.severity = ALOG_INFO;
    conf.callback_count = 0;
    conf.callback_capacity = 10;
    conf.callbacks = calloc(10, sizeof(void *));
    alog_conf_callback_add_stdout(&conf);

    alog_sync(conf, ALOG_INFO, "Writing to stdout! %d", 1);

    alog_conf_callback_add_stderr(&conf);

    alog_sync(conf, ALOG_WARNING, "Writing to stdout and stderr! %d", 2);

    alog_conf_callback_add_static_filepath(&conf, "temp_log.txt");

    alog(conf, ALOG_EMERGENCY, "Writing to stdout and stderr and a file! %d", 3);

    alog(conf, ALOG_DEBUG, "This log should not appear!");

    return 0;
}