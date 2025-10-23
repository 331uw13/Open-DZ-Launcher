#include <stdio.h>
#include <string.h>

#include "string.h"
#include "server_query.h"
#include "a2s_parser.h"
#include "a2s_decoder.h"
#include "opendz_launcher.h"







bool opendzl_get_server_info(char* addr, uint16_t port, struct dayz_server* server) {
    bool result = false;

    struct string_t server_info = create_string();
    struct string_t server_mod_info = create_string();

    if(!get_server_a2s_responses(addr, port, &server_info, &server_mod_info)) {
        fprintf(stderr, "Failed to get correct server A2S responses.\n");
        goto out;
    }


    if(server_info.size < 4) {
        fprintf(stderr, "Failed to get good server info response. Got %i bytes.\n",
                server_info.size);
        goto out;
    }

    if(server_mod_info.size < 4) {
        fprintf(stderr, "Failed to get good server mod info response. Got %i bytes.\n",
                server_mod_info.size);
        goto out;
    }


    uint8_t decoded[4096] = { 0 };
    if(!decode_a2s_escapes((uint8_t*)server_mod_info.bytes, server_mod_info.size,
            decoded, sizeof(decoded))) {
        fprintf(stderr, "Failed to decode A2S escape sequences.\n");
        goto out;
    }

    if(!parse_a2s_rules(decoded, sizeof(decoded), server->mods, &server->num_mods)) {
        server->num_mods = 0;
        //goto out;
    }


    memset(server->name, 0, OPENDZL_MAX_SERVER_NAME);
    memset(server->map_name, 0, OPENDZL_MAX_SERVER_MAP_NAME);

    server->name_length = 0;
    server->map_name_length = 0;

    //
    //   Steam header |???| Server name | Game map name | 
    //                |   |             |               |
    //     FF FF FF FF I11 EXAMPLE NAME0 chernarusplus0 ...
    // 
    // First 6 bytes can be ignored.
    

    // Read server name.
    for(uint32_t i = 6; i < server_info.size; i++) {
        char ch = server_info.bytes[i];
        if(ch == 0) {
            break;
        }
        server->name[server->name_length++] = ch;
    }

    // Read server map name.
    for(uint32_t i = 7+server->name_length; i < server_info.size; i++) {
        char ch = server_info.bytes[i];
        if(ch == 0) {
            break;
        }
        server->map_name[server->map_name_length++] = ch;
    }

    /*
    for(size_t i = 0; i < server_info.size; i++) {
        char byte = server_info.bytes[i];
        if(byte >= 0x20 && byte <= 0x7E) {
            printf("%c", byte);
        }
        else {
            printf("\033[34m%02X\033[0m ", (uint8_t)byte);
        }
    }

    printf("\n");
    */

    result = true;

out:
    string_free(&server_info);
    string_free(&server_mod_info);

    return result;
}

