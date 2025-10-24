#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "string.h"
#include "server_query.h"
#include "a2s_parser.h"
#include "a2s_decoder.h"
#include "opendz_launcher.h"


char* mods[] = {
    "3031784065",
    "3417067137",
    "1559212036",
    "2572331007",
    "2394804821",
    "2545327648",
    "1582756848",
    "1644467354",
    "3324786374"
};

int num_mods = 9;



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

        //system(cmd.bytes);

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

void start_dayz() {
  
    //link_mods();  // TODO: Create a check for symlink if they exists. create them.
    // And should maybe not use system() for that.

    struct string_t cmd = create_string();

    cmd_add_steam_applaunch(&cmd);
    cmd_add_mods(&cmd);
    cmd_add_destination(&cmd, "192.168.1.141", "2304");


    printf("%s\n", cmd.bytes);
   
    system(cmd.bytes);
    string_free(&cmd);
     


}

/*
void print_server_info(char* addr, uint16_t port) {

    struct dayz_server server_info;


    printf("–––––––––––––––––––––––––––––––––––––––––––––––––––\n");
    printf("\033[90mA2S Response from: (%s : %i)\033[0m\n", addr, port);
    printf("\n");
    if(!opendzl_get_server_info(addr, port, &server_info)) {
        fprintf(stderr, "opendzl_get_server_info() Failed\n");
        return;
    }

    printf("'%s'\n", server_info.name);
    printf("'%s'\n", server_info.map_name);
    printf(" %i Mods\n", server_info.num_mods);

    for(uint8_t i = 0; i < server_info.num_mods; i++) {
        printf("%s \033[32m%u\033[0m\n", 
                server_info.mods[i].name,
                server_info.mods[i].workshop_id);
    }

    printf("-----------------------------------------------------\n");
}
*/


int main() {

    
    if(!opendzl_init_sdl3()) {
        return 1;
    }

    if(!opendzl_load_font("DejaVuSansCondensed.ttf")) {
        return 1;
    }

    opendzl_run();
    opendzl_free();
    
    //link_mods();
    //start_dayz();
 
    //print_server_info("192.168.1.141", 27016);
    //print_server_info("168.100.163.22", 27016);
    //print_server_info("103.152.197.191", 2303);
    //print_server_info("189.127.165.207", 27016);


     

    /*
    struct string_t server_info = create_string();
    struct string_t server_mod_info = create_string();

    get_server_a2s_responses("185.207.214.54", 2305, &server_info, &server_mod_info);



    printf("\033[90m\n––––––––––––––––––––––––––––––\033[0m\n");
    for(size_t i = 0; i < server_info.size; i++) {
        char byte = server_info.bytes[i];
        if(byte >= 0x20 && byte <= 0x7E) {
            printf("%c", byte);
        }
        else {
            printf("\033[34m%02X\033[0m ", (uint8_t)byte);
        }
    }
   

    uint8_t decoded[4096] = { 0 };

    decode_a2s_escapes((uint8_t*)server_mod_info.bytes, server_mod_info.size,
            decoded, sizeof(decoded));


    struct dayz_mod  mods[OPENDZL_MAX_SERVER_MODS];
    uint8_t          num_mods = 0;

    
    parse_a2s_rules(decoded, sizeof(decoded), 
            mods, &num_mods);

    for(uint16_t i = 0; i < num_mods; i++) {
        printf("'%s' %u\n", mods[i].name, mods[i].workshop_id);
    }

    printf("Server has %i mods\n", num_mods);




    //parse_a2s_rules((uint8_t*)server_mod_info.bytes, server_mod_info.size);
    */

    return 0;
}




