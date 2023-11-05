#include "simple_logger.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static FILE * __log_file = NULL;
static int dirty = 0;

void close_logger()
{
    if (__log_file != NULL)
    {
        fclose(__log_file);
        __log_file = NULL;
    }
}

void init_logger(const char *log_file_path,int append_mode)
{
    int mode = 0;
    const char *modes[] = 
    {
        "w",
        "a"
    };
    if (append_mode)mode = 1;
    if (log_file_path == NULL)
    {
        __log_file = fopen("simple.log",modes[mode]);
    }
    else
    {
        __log_file = fopen(log_file_path,modes[mode]);
    }
    atexit(close_logger);
}

void _slog(char *f,int l,char *msg,...)
{
    va_list ap;
    dirty = 1;
    /*echo all logging to stdout*/
    va_start(ap,msg);
    fprintf(stdout,"%s:%i: ",f,l);
    vfprintf(stdout,msg,ap);
    fprintf(stdout,"\n");
    va_end(ap);
    fprintf(stdout,"\n");
    if (__log_file != NULL)
    {
        va_start(ap,msg);
        fprintf(__log_file,"%s:%i: ",f,l);
        vfprintf(__log_file,msg,ap);
        fprintf(__log_file,"\n");
        va_end(ap);
    }
}

void slog_sync()
{
    if ((__log_file == NULL)||(!dirty))return;
    dirty = 0;
    fflush(__log_file);
}
/*eol@eof*/
