#ifndef IO_WRITER_H
#define IO_WRITER_H

typedef struct {
    bool (*setup)(void *instance, bool writing_to_stdout, wchar_t *full_output_path);
    void (*teardown)(void *instance);
    bool (*write_output)(void *instance,
                         const char *mb_out,
                         size_t total_size,
                         long unsigned int *written);
    void *user_args;
} writer_t;

#endif // IO_WRITER_H
