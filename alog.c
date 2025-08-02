/*
MIT License

Copyright (c) 2025 James Dee Wood

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "alog.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifdef ALOG_GLOBAL
alog_conf global_conf;
#endif

static int stdout_wrapper(const char * format)
{
    return printf(format) >= 0;
}

static int stderr_wrapper(const char * format)
{
    return fprintf(stderr, format) >= 0;
}

static int path_cb_wrapper(const char * filepath, const char * format)
{
    FILE * f;

    f = fopen(filepath, "a");
    if (f == NULL)
    {
        return -1;
    }

    if (fprintf(f, format) < 0)
    {
        return 1;
    }

    if (fclose(f))
    {
        return 2;
    }

    return 0;
}

const char * alog_severity_str(alog_severity severity)
{
    switch (severity)
    {
        case ALOG_EMERGENCY:
            return "EMERGENCY";
        case ALOG_ALERT:
            return "ALERT";
        case ALOG_CRITICAL:
            return "CRITICAL";
        case ALOG_ERROR:
            return "ERROR";
        case ALOG_WARNING:
            return "WARNING";
        case ALOG_NOTICE:
            return "NOTICE";
        case ALOG_INFO:
            return "INFO";
        case ALOG_DEBUG:
            return "DEBUG";
        default:
            return "UNKNOWN";
    }
}

int alog_conf_callback_add(alog_conf * conf, alog_callback callback)
{
    if (conf->callback_count == conf->callback_capacity)
    {
        conf->callbacks = realloc(conf->callbacks, 
                                  conf->callback_capacity + 
                                  ALOG_CALLBACK_INCREMENT);
        if (conf->callbacks == NULL)
        {
            return 1;
        }
        conf->callback_capacity += ALOG_CALLBACK_INCREMENT;
    }
    conf->callbacks[conf->callback_count] = callback;
    conf->callback_count++;

    return 0;
}

int alog_conf_callback_add_stdout(alog_conf * conf)
{
    alog_callback cb;
    cb.flags = ALOG_CB_NONE;
    cb.callback.cb = stdout_wrapper;
    return alog_conf_callback_add(conf, cb);
}

int alog_conf_callback_add_stderr(alog_conf * conf)
{
    alog_callback cb;
    cb.flags = ALOG_CB_NONE;
    cb.callback.cb = stderr_wrapper;
    return alog_conf_callback_add(conf, cb);
}

int alog_conf_callback_add_static_filepath(alog_conf * conf, 
                                            const char * path)
{
    alog_callback cb;
    cb.flags = ALOG_CB_PATH;
    cb.filepath = path;
    cb.callback.cb_path = path_cb_wrapper;
    return alog_conf_callback_add(conf, cb);
}

static int alog_sync_internal(alog_conf conf, alog_severity severity, 
                              const char * format, va_list args)
{
    int i, rc, len;
    char log[ALOG_MAX_LENGTH], log_with_sev[ALOG_MAX_LENGTH];
    
    if (conf.severity < severity)
    {
        return 0;
    }

    len = vsnprintf(log, sizeof(log), format, args);

    if (len < 0)
    {
        return 1;
    }
    else if (conf.flags & ALOG_CONF_NEWLINE)
    {
        if (len < (ALOG_MAX_LENGTH - 1))
        {
            log[len] = '\n';
            log[len + 1] = '\0';
        }
        else
        {
            log[ALOG_MAX_LENGTH - 2] = '\n';
        }
    }

    len = snprintf(log_with_sev, sizeof(log), "[%s] %s", 
             alog_severity_str(severity), log);

    if (len < 0)
    {
        return 2;
    }
    else if (conf.flags & ALOG_CONF_NEWLINE)
    {
        if (len >= (ALOG_MAX_LENGTH - 1))
        {
            log_with_sev[ALOG_MAX_LENGTH - 2] = '\n';
        }
    }

    /* TODO: add color support */

    rc = 0;
    for (i = 0; i < conf.callback_count; i++)
    {
        if ((conf.callbacks[i].flags & ALOG_CB_SEV) && 
            (conf.callbacks[i].flags & ALOG_CB_FILE))
        {
            rc |= conf.callbacks[i].callback.cb_sev_file(severity, 
                                                   conf.callbacks[i].stream, 
                                                   log);
        }
        else if ((conf.callbacks[i].flags & ALOG_CB_SEV) && 
                 (conf.callbacks[i].flags & ALOG_CB_PATH))
        {
            rc |= conf.callbacks[i].callback.cb_sev_path(severity,
                                                   conf.callbacks[i].filepath,
                                                   log);
        }
        else if (conf.callbacks[i].flags & ALOG_CB_SEV)
        {
            rc |= conf.callbacks[i].callback.cb_sev(severity, log);
        }
        else if (conf.callbacks[i].flags & ALOG_CB_FILE)
        {
            rc |= conf.callbacks[i].callback.cb_file(conf.callbacks[i].stream, 
                                               log_with_sev);
        }
        else if (conf.callbacks[i].flags & ALOG_CB_PATH)
        {
            
            rc |= conf.callbacks[i].callback.cb_path(conf.callbacks[i].filepath, 
                                               log_with_sev);
        }
        else
        {
            rc |= conf.callbacks[i].callback.cb(log_with_sev);
        }
    }

    return rc;
}

static int alog_async_internal(alog_conf conf, alog_severity severity,
                               const char * format, va_list args)
{
    /* TODO: implement (thread safe) */
    return 0;
}

static int alog_internal(alog_conf conf, alog_severity severity,
                         const char * format, va_list args)
{
    int rc;

    if (conf.flags & ALOG_CONF_ASYNC)
    {
        rc = alog_async_internal(conf, severity, format, args);
    }
    else
    {
        rc = alog_sync_internal(conf, severity, format, args);
    }

    return rc;
}

int alog(alog_conf conf, alog_severity severity, const char * format, ...)
{
    int rc;
    va_list args;

    va_start(args, format);
    rc = alog_internal(conf, severity, format, args);
    va_end(args);

    return rc;
}

int alog_sync(alog_conf conf, alog_severity severity, const char * format, ...)
{
    int rc;
    va_list args;

    va_start(args, format);
    rc = alog_sync_internal(conf, severity, format, args);
    va_end(args);

    return rc;
}

int alog_async(alog_conf conf, alog_severity severity, const char * format, ...)
{
    /* TODO: implement (thread safe) */
    return 0;
}

#ifdef ALOG_GLOBAL

int alog_global_init(int callback_capacity)
{
    global_conf.callback_capacity = callback_capacity;
    global_conf.callbacks = calloc(callback_capacity, sizeof(alog_callback *));
}

void alog_global_set_flags(int flags)
{
    global_conf.flags = flags;
}

void alog_global_set_severity_level(alog_severity severity)
{
    global_conf.severity = severity;
}

void alog_global_clean()
{
    free(global_conf.callbacks);
}

int alog_global_conf_callback_add(alog_callback callback)
{
    return alog_conf_callback_add(&global_conf, callback);
}

int alog_global_conf_callback_add_stdout()
{
    return alog_conf_callback_add_stdout(&global_conf);
}

int alog_global_conf_callback_add_stderr()
{
    return alog_conf_callback_add_stderr(&global_conf);
}

int alog_global_conf_callback_add_static_filepath(const char * path)
{
    return alog_conf_callback_add_static_filepath(&global_conf, path);
}

int alog_global(alog_severity severity, const char * format, ...)
{
    int rc;
    va_list args;

    va_start(args, format);
    rc = alog_internal(global_conf, severity, format, args);
    va_end(args);

    return rc;
}

int alog_global_sync(alog_severity severity, const char * format, ...)
{
    int rc;
    va_list args;

    va_start(args, format);
    rc = alog_sync_internal(global_conf, severity, format, args);
    va_end(args);

    return rc;
}

int alog_global_async(alog_severity severity, const char * format, ...)
{
    /* TODO: implement (thread safe) */
    return 0;
}

#endif /* ALOG_GLOBAL */