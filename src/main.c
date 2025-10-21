#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "string.h"
#include "server_query.h"
#include "a2s_parser.h"
#include "a2s_decoder.h"





/*
void cmd_add_steam_applaunch(struct string_t* cmd) {
    char* applaunch = "steam -applaunch 221100 -nolauncher ";
    string_move(cmd, applaunch, strlen(applaunch));
}


void link_mods() {

    char* mods_dir = "~/.local/share/Steam/steamapps/workshop/content/221100/";
    char* dayz_path = "~/.local/share/Steam/steamapps/common/DayZ/";

    struct string_t cmd = create_string();

    for(size_t i = 0; i < num_mods; i++) {
       
        string_clear(&cmd);
        string_append(&cmd, "ln -sr ", 7);
        string_append(&cmd, mods_dir, strlen(mods_dir));
        string_append(&cmd, mods[i], strlen(mods[i]));
        string_pushbyte(&cmd, ' ');

        string_append(&cmd, dayz_path, strlen(dayz_path));
        string_pushbyte(&cmd, '@');
        string_append(&cmd, mods[i], strlen(mods[i]));

        system(cmd.bytes);

        printf("%s\n", cmd.bytes);

        //printf("%s, %s\n", dayz_path, mods[i]);
    }

    string_free(&cmd);
}

void cmd_add_mods(struct string_t* cmd) {
    string_append(cmd, "-mod='", 6);

    for(size_t i = 0; i < num_mods; i++) {
    
        string_pushbyte(cmd, '@');
        string_append(cmd, mods[i], strlen(mods[i]));
        if(i+1 < num_mods) {
            string_pushbyte(cmd, ';');
        }
    }

    string_pushbyte(cmd, '\'');
    string_pushbyte(cmd, ' ');
}

void cmd_add_destination(struct string_t* cmd, char* host, char* port) {
    string_append(cmd, "-connect=", 9);
    string_append(cmd, host, strlen(host));
    string_pushbyte(cmd, ' ');
    string_append(cmd, "-port=", 6);
    string_append(cmd, port, strlen(port));

}
    */

int main() {
    
    /*
    //link_mods();  // TODO: Create a check for symlink if they exists. create them.
    // And should maybe not use system() for that.

    struct string_t cmd = create_string();

    cmd_add_steam_applaunch(&cmd);
    cmd_add_mods(&cmd);
    cmd_add_destination(&cmd, "192.168.1.141", "2304");


    printf("%s\n", cmd.bytes);
   
    system(cmd.bytes);
    string_free(&cmd);
    */ 

    
    struct string_t server_info = create_string();
    struct string_t server_mod_info = create_string();

    query_dayz_server("192.168.1.141", 27016, &server_info, &server_mod_info);


    printf("\033[90m\n––––––––––––––––––––––––––––––\033[0m\n");
    for(size_t i = 0; i < server_info.size; i++) {
        char byte = server_info.bytes[i];
        if(byte >= 0x20 && byte <= 0x7E) {
            printf("%c", byte);
        }
        else {
            printf("\033[34m%x\033[0m", byte);
        }
    }
    printf("\033[90m\nRAW ––––––––––––––––––––––––––––––\033[0m\n\n");
    for(size_t i = 0; i < server_mod_info.size; i++) {
        char byte = server_mod_info.bytes[i];

        printf("%02X ", (uint8_t)byte);
    }
    printf("\n");    


    // ================================================================


    uint8_t decoded[4096] = { 0 };

    decode_a2s_escapes((uint8_t*)server_mod_info.bytes, server_mod_info.size,
            decoded, sizeof(decoded));


    printf("\033[90m\nDECODED ––––––––––––––––––––––––––––––\033[0m\n\n");
    for(size_t i = 0; i < server_mod_info.size; i++) {
        char byte = decoded[i];

        printf("%02X ", (uint8_t)byte);
    }
    printf("\n");    
    printf("\033[90m\n––––––––––––––––––––––––––––––\033[0m\n\n");




    parse_a2s_rules(decoded, sizeof(decoded));




    //parse_a2s_rules((uint8_t*)server_mod_info.bytes, server_mod_info.size);


    return 0;
}




