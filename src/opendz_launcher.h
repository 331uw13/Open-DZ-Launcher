#ifndef OPENDZ_LAUNCHER_H
#define OPENDZ_LAUNCHER_H

#include "dayz_mod.h"


#define OPENDZL_MAX_SERVER_MODS 32
#define OPENDZL_MAX_SERVER_NAME 256
#define OPENDZL_MAX_SERVER_MAP_NAME 256


struct dayz_server {

    struct dayz_mod mods[OPENDZL_MAX_SERVER_MODS];
    uint8_t         num_mods;

    char     name[OPENDZL_MAX_SERVER_NAME];
    uint8_t  name_length;

    char     map_name[OPENDZL_MAX_SERVER_MAP_NAME];
    uint8_t  map_name_length;

};


struct rtext {
    SDL_Rect rect;
    SDL_Texture* texture;
};


void opendzl_create_rtext(struct rtext* text, char* buf, int font_size, int color);
void opendzl_render_rtext(struct rtext* text, int x, int y);
void opendzl_free_rtext(struct rtext* text);

bool opendzl_get_server_info(char* addr, uint16_t port, struct dayz_server* server);

bool opendzl_init_sdl3();
bool opendzl_load_font(const char* path);
void opendzl_run();
void opendzl_free();


#endif
