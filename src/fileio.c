#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "fileio.h"



bool opendzl_file_exists(const char* path) {
    return (access(path, F_OK) == 0);
}

bool opendzl_dir_exists(const char* path) {
    struct stat sb;
    if(lstat(path, &sb) != 0) {
        return false;
    }
    return (sb.st_mode & S_IFDIR);
}

bool opendzl_mkdir_p(const char* path, mode_t perm) {
    bool result = false;
    
    if(!path) {
        goto out;
    }

    const size_t path_length = strlen(path);

    char buffer[256] = { 0 };
    size_t buffer_idx = 0;

    for(size_t i = 0; i < path_length; i++) {
        char ch = path[i];

        
        buffer[buffer_idx++] = ch;
        if(buffer_idx >= sizeof(buffer)) {
            fprintf(stderr, "%s: The path is too long.\n", __func__);
            goto out;
        }

        if(((i > 0) && (ch == '/')) 
        || (i+1 >= path_length)) {
            if(!opendzl_dir_exists(buffer)) {
                if(mkdir(buffer, perm) != 0) {
                    fprintf(stderr, "%s: %s\n", __func__, strerror(errno));
                    goto out;
                }
            }
        }
    }
    
    result = true;

out:
    return result;
}


