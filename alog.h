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

#ifndef ALOG_H
#define ALOG_H

#include <stdio.h>

#ifndef ALOG_MAX_LENGTH
#define ALOG_MAX_LENGTH 1024
#endif

#ifndef ALOG_CALLBACK_INCREMENT
#define ALOG_CALLBACK_INCREMENT 10
#endif

/* syslog severity levels */
typedef enum alog_severity
{
    ALOG_EMERGENCY  = 0,
    ALOG_ALERT      = 1,
    ALOG_CRITICAL   = 2,
    ALOG_ERROR      = 3,
    ALOG_WARNING    = 4,
    ALOG_NOTICE     = 5,
    ALOG_INFO       = 6,
    ALOG_DEBUG      = 7
} alog_severity;

typedef enum alog_callback_flag
{
    ALOG_CB_NONE    = 0b00000000,   /* No flags */
    ALOG_CB_SEV     = 0b00000010,   /* CB function supports an 
                                    explicit severity parameter */
    ALOG_CB_FILE    = 0b00000100,   /* CB function uses FILE parameter */
    ALOG_CB_PATH    = 0b00001000    /* CB function uses file path parameter */
} alog_callback_flag;

/*
    Callbacks should return 0 on success and non-zero on failure.
*/
typedef struct alog_callback
{
    int flags;
    const char * filepath;
    FILE * stream;
    union callback_data
    {
        int (* cb) (const char *);
        int (* cb_sev) (int, const char *);
        int (* cb_file) (FILE *, const char *);
        int (* cb_path) (const char *, const char *);
        int (* cb_sev_file) (int, FILE *, const char *);
        int (* cb_sev_path) (int, const char *, const char *);
    } callback;
} alog_callback;

typedef enum alog_conf_flag
{
    ALOG_CONF_NONE      = 0b00000000,   /* No flags */
    ALOG_CONF_ASYNC     = 0b00000001,   /* Sets default log type to async */
    ALOG_CONF_NEWLINE   = 0b00000010,   /* Append a new line char to 
                                        each log operation */
    ALOG_CONF_COLOR     = 0b00000100    /* Use color in logs */
} alog_conf_flag;

typedef struct alog_conf
{
    int flags;
    alog_severity severity;             /* Filter out logs below this level */
    int callback_count;
    int callback_capacity;
    alog_callback * callbacks;
} alog_conf;

/* Utility */
const char * alog_severity_str(alog_severity severity);

/* Callback Configuration */
int alog_conf_callback_add(alog_conf * conf, alog_callback callback);
int alog_conf_callback_add_stdout(alog_conf * conf);
int alog_conf_callback_add_stderr(alog_conf * conf);
int alog_conf_callback_add_static_filepath(alog_conf * conf, 
                                           const char * path);
                                           
/* Logging */
int alog(alog_conf conf, alog_severity severity, const char * format, ...);
int alog_sync(alog_conf conf, alog_severity severity, const char * format, ...);
int alog_async(alog_conf conf, alog_severity severity, 
               const char * format, ...);

#ifdef ALOG_GLOBAL

/* Configuration */
int alog_global_init(int callback_capacity);
void alog_global_set_flags(int flags);
void alog_global_set_severity_level(alog_severity severity);
void alog_global_clean();

/* Callback Configuration */
int alog_global_conf_callback_add(alog_callback callback);
int alog_global_conf_callback_add_stdout();
int alog_global_conf_callback_add_stderr();
int alog_global_conf_callback_add_static_filepath(const char * path);

/* Logging */
int alog_global(alog_severity severity, const char * format, ...);
int alog_global_sync(alog_severity severity, const char * format, ...);
int alog_global_async(alog_severity severity, const char * format, ...);

#endif /* ALOG_GLOBAL */

#endif /* ALOG_H */