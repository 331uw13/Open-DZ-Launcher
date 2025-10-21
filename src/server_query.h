#ifndef SERVER_QUERY_H
#define SERVER_QUERY_H


#include <stdint.h>

#include "string.h"


void query_dayz_server(const char* host, uint16_t query_port,
        struct string_t* server_info,
        struct string_t* server_mod_info);



#endif
