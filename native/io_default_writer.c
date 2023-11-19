#include "io.h"

typedef struct {
    HANDLE out_fp;
    bool writing_to_stdout;
} default_writer_user_args_t;

static bool default_writer_setup(void *instance, bool writing_to_stdout, wchar_t *full_output_dir) {
    writer_t *writer = (writer_t *)instance;
    default_writer_user_args_t *args = malloc(sizeof(default_writer_user_args_t));
    if (!args) { // LCOV_EXCL_START
        return false;
    } // LCOV_EXCL_STOP
    memset(args, 0, sizeof(default_writer_user_args_t));
    args->writing_to_stdout = writing_to_stdout;
    args->out_fp = !writing_to_stdout ? CreateFile(full_output_dir,
                                                   GENERIC_READ | GENERIC_WRITE,
                                                   0,
                                                   nullptr,
                                                   CREATE_ALWAYS,
                                                   FILE_ATTRIBUTE_NORMAL,
                                                   nullptr) :
                                        GetStdHandle(STD_OUTPUT_HANDLE);
    if (args->out_fp == INVALID_HANDLE_VALUE) {
        goto fail;
    }
    writer->user_args = args;
    return true;
fail:
    free(args);
    return false;
}

static bool default_writer_write_output(void *instance,
                                        const char *mb_out,
                                        size_t total_size,
                                        long unsigned int *written) {
    writer_t *writer = (writer_t *)instance;
    default_writer_user_args_t *args = (default_writer_user_args_t *)writer->user_args;
    return WriteFile(args->out_fp, mb_out, (DWORD)total_size, (LPDWORD)written, nullptr);
}

static void default_writer_teardown(void *instance) {
    writer_t *writer = (writer_t *)instance;
    default_writer_user_args_t *args = (default_writer_user_args_t *)writer->user_args;
    if (!args->writing_to_stdout) {
        CloseHandle(args->out_fp);
    }
    free(writer->user_args);
}

writer_t *get_default_writer() {
    writer_t *out = malloc(sizeof(writer_t));
    if (!out) { // LCOV_EXCL_START
        return nullptr;
    } // LCOV_EXCL_STOP
    out->setup = default_writer_setup;
    out->write_output = default_writer_write_output;
    out->teardown = default_writer_teardown;
    return out;
}
