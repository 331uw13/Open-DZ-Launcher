#ifndef OPENDZL_FILEIO_H
#define OPENDZL_FILEIO_H

#include <stdbool.h>

bool opendzl_file_exists(const char* path);
bool opendzl_dir_exists(const char* path);
bool opendzl_mkdir_p(const char* path, mode_t perm);

#endif
