#include "io.h"
#include "io_default_writer.h"

int default_writer_setup_stdout(void **state) {
    will_return_always(__wrap_GetStdHandle, (HANDLE)0);
    default_writer.setup(&default_writer, true, nullptr);
    return 0;
}

int default_writer_teardown(void **state) {
    default_writer.teardown(&default_writer);
    return 0;
}
