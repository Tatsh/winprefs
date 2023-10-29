#ifndef IO_H
#define IO_H

#include <minwindef.h>

#include <stdio.h>

typedef struct {
    char path[MAX_PATH];
    char data[2048];
    long len;
    long pos;
} fake_file_t;

FILE *__real_fopen(const char *restrict path, const char *restrict mode);
int __real_fread(void *restrict ptr, size_t size, size_t nitems, FILE *restrict stream);
int __real_fseek(FILE *stream, long offset, int whence);
long __real_ftell(FILE *stream);
void __real_rewind(FILE *stream);
int __real_fclose(FILE *stream);

#endif // IO_H
