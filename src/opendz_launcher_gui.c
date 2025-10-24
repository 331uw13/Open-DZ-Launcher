


#include "opendz_launcher.h"
#include "opendz_launcher_gui.h"


#define OPENDZLGUI_LABEL_MAX 32
#define OPENDZLGUI_BUTTONS_MAX 16
#define OPENDZLGUI_TEXTS_MAX 16




struct opendzlgui_text {
    SDL_Rect rect;
    SDL_Texture* texture;
};

struct opendzlgui_button {
    char label[OPENDZLGUI_LABEL_MAX];
    void(*callback)(void*);
};


static struct GUI_T {

    struct opendzlgui_text texts[OPENDZLGUI_TEXTS_MAX];
    uint8_t num_texts;

    struct opendzlgui_button buttons[OPENDZLGUI_BUTTONS_MAX];
    uint8_t num_buttons;


}
GUI = (struct GUI_T) {
    .num_buttons = 0,
    .num_texts = 0
};



void opendzlgui_free() {
    for(uint8_t i = 0; i < GUI.num_texts; i++) {
        SDL_DestroyTexture(GUI.texts[i].texture);
    }
}


static SDL_Color hex_to_rgb(int hex_color_rgb) {
    return (SDL_Color){ 
        (hex_color_rgb & 0xFF0000) >> 16,
        (hex_color_rgb & 0x00FF00) >> 8,
        (hex_color_rgb & 0x0000FF),
        255
    };
}



static void opendzlgui_render_text(
        struct SDL_T* sdl,
        struct opendzlgui_text* text,
        int x, int y) {
    text->rect.x = x;
    text->rect.y = y;
    SDL_RenderCopy(sdl->renderer, text->texture, NULL, &text->rect);
}


guielem_handle_t  opendzlgui_create_text
(
        struct SDL_T* sdl,
        char* buffer,
        struct vec2i pos,
        int font_size,
        int hex_color_rgb,
        int flags
){
    guielem_handle_t handle = -1;
    if(GUI.num_texts+1 >= OPENDZLGUI_TEXTS_MAX) {
        fprintf(stderr, "%s: Too many text elements.\n", __func__);
        return handle;
    }

    handle = GUI.num_texts;
    struct opendzlgui_text* text = &GUI.texts[GUI.num_texts];
    GUI.num_texts++;


    TTF_SetFontSize(sdl->font, font_size);

    SDL_Color text_color = hex_to_rgb(hex_color_rgb);
    SDL_Surface* surface = TTF_RenderText_Blended(sdl->font, buffer, text_color);
    text->texture = SDL_CreateTextureFromSurface(sdl->renderer, surface);

    text->rect = (SDL_Rect) { pos.x, pos.y, surface->w, surface->h };
    SDL_FreeSurface(surface);

    return handle;
}



void opendzlgui_render(struct SDL_T* sdl) {
    struct opendzlgui_text* text = NULL;
    for(uint8_t i = 0; i < GUI.num_texts; i++) {
        text = &GUI.texts[i];
        opendzlgui_render_text(sdl, text, text->rect.x, text->rect.y);
    }
}



