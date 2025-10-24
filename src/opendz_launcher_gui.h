#ifndef OPENDZL_GUI_H
#define OPENDZL_GUI_H

#include "vec2.h"



struct SDL_T;
typedef int guielem_handle_t;


void opendzlgui_free();
void opendzlgui_render(struct SDL_T* sdl);



guielem_handle_t  opendzlgui_create_text
(
    struct SDL_T* sdl,
    char* label,
    struct vec2i pos,
    int font_size,
    int flags
);

guielem_handle_t  opendzlgui_create_button
(
    struct SDL_T* sdl,
    char* label,
    struct vec2i pos,
    SDL_Color color,
    void(*callback)(void*),
    int flags
);








#endif
