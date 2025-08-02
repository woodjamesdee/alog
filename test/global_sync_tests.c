#include "../alog.h"

/* gcc alog.c test/global_sync_tests.c -o global_test --std=c89 -DALOG_GLOBAL */

int main()
{
    alog_global_init(10);
    alog_global_set_flags(ALOG_CONF_NEWLINE);
    alog_global_set_severity_level(ALOG_WARNING);
    alog_global_conf_callback_add_stdout();

    alog_global(ALOG_WARNING, "Global hello, world!");

    alog_global(ALOG_INFO, "This global log should not appear!");

    alog_global_clean();

    return 0;
}