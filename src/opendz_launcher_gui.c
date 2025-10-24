


#include "opendz_launcher.h"
#include "opendz_launcher_gui.h"


#define OPENDZLGUI_LABEL_MAX 32
#define OPENDZLGUI_BUTTONS_MAX 16
#define OPENDZLGUI_TEXTS_MAX 16


#define GUICOLOR_TEXT (SDL_Color){ 230, 200, 160, 255 }
#define GUICOLOR_BUTTON_FG (SDL_Color){ 230, 200, 160, 255 }
#define GUICOLOR_BUTTON_BG (SDL_Color){ 50, 50, 50, 255 }
#define GUICOLOR_BUTTON_SHADOW (SDL_Color){ 20, 20, 20, 255 }

#define GUIBUTTON_FONT_SIZE 18
#define GUIBUTTON_TEXT_XPADDN 5
#define GUIBUTTON_TEXT_YPADDN 2


struct guielem_timers {
    float focus_enter;
    float elem_active;
};

struct opendzlgui_text {
    SDL_Rect rect;
    SDL_Texture* texture;
};

struct opendzlgui_button {
    char label[OPENDZLGUI_LABEL_MAX];
    guielem_handle_t text_handle;
    SDL_Rect rect;
    SDL_Color bg_color;
    void(*callback)(void*);

    struct guielem_timers timers;
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

static inline void clampf(float* f, const float min, const float max) {
    if(*f < min) { *f = min; }
    else
    if(*f > max) { *f = max; }
}
static inline void clampi(int* f, const int min, const int max) {
    if(*f < min) { *f = min; }
    else
    if(*f > max) { *f = max; }
}

static bool mouse_on_rect(struct SDL_T* sdl, const SDL_Rect* rect) {
    return((sdl->mouse_x > rect->x) && (sdl->mouse_x <= rect->x + rect->w) &&
           (sdl->mouse_y > rect->y) && (sdl->mouse_y <= rect->y + rect->h));
}

static void init_guielem_timers(struct guielem_timers* timers) {
    timers->focus_enter = 0.0f;
    timers->elem_active = 0.0f;
}
static void update_guielem_timers(struct SDL_T* sdl, const SDL_Rect* rect, struct guielem_timers* timers) {
    if(mouse_on_rect(sdl, rect)) {
        timers->focus_enter += sdl->frame_time * 10;
    
        if(sdl->mouse_down) {
            timers->elem_active = 1.0f;
        }
    }
    else {
        timers->focus_enter -= sdl->frame_time * 5;
    }
    timers->elem_active -= sdl->frame_time * 3;
            
    clampf(&timers->focus_enter, 0.0f, 1.0f);
    clampf(&timers->elem_active, 0.0f, 1.0f);
}

static void draw_rect(struct SDL_T* sdl, const SDL_Rect* rect, SDL_Color color) {
    SDL_SetRenderDrawColor(sdl->renderer, color.r, color.g, color.b, 255);
    SDL_RenderFillRect(sdl->renderer, rect);
}

static void draw_rect_ln(struct SDL_T* sdl, const SDL_Rect* rect, SDL_Color color) {
    SDL_SetRenderDrawColor(sdl->renderer, color.r, color.g, color.b, 255);
    SDL_RenderDrawRect(sdl->renderer, rect);
}

static void opendzlgui_render_text(
        struct SDL_T* sdl,
        struct opendzlgui_text* text
){
    SDL_RenderCopy(sdl->renderer, text->texture, NULL, &text->rect);
}

static inline SDL_Color color_mult(const SDL_Color color, float factor) {
    int r = color.r * factor;
    int g = color.g * factor;
    int b = color.b * factor;

    clampi(&r, 0, 255);
    clampi(&g, 0, 255);
    clampi(&b, 0, 255);
    
    return (SDL_Color) { r, g, b, color.a };
}


static void opendzlgui_render_button(
        struct SDL_T* sdl,
        struct opendzlgui_button* button
){
    update_guielem_timers(sdl, &button->rect, &button->timers);

    SDL_Rect shadow_rect = button->rect;
    shadow_rect.x += 3;
    shadow_rect.y += 3;
    draw_rect(sdl, &shadow_rect, GUICOLOR_BUTTON_SHADOW);

    draw_rect(sdl, &button->rect, 
            color_mult(button->bg_color, 1.0f + button->timers.elem_active * 0.85f));

    draw_rect_ln(sdl, &button->rect, 
            color_mult(button->bg_color, 1.2f + button->timers.focus_enter * 0.65f));
}

static void opendzlgui_update_button(struct SDL_T* sdl, struct opendzlgui_button* button) {
    if(!sdl->mouse_down) {
        return;
    }

    if(mouse_on_rect(sdl, &button->rect)) {
        button->callback(sdl);
    }
}


guielem_handle_t  opendzlgui_create_text
(
        struct SDL_T* sdl,
        char* buffer,
        struct vec2i pos,
        int font_size,
        int flags
){
    guielem_handle_t handle = -1;
    if(GUI.num_texts+1 >= OPENDZLGUI_TEXTS_MAX) {
        fprintf(stderr, "%s: Too many text elements.\n", __func__);
        return handle;
    }

    handle = GUI.num_texts;
    struct opendzlgui_text* text = &GUI.texts[handle];
    GUI.num_texts++;


    TTF_SetFontSize(sdl->font, font_size);

    SDL_Surface* surface = TTF_RenderText_Blended(sdl->font, buffer, GUICOLOR_TEXT);
    text->texture = SDL_CreateTextureFromSurface(sdl->renderer, surface);

    text->rect = (SDL_Rect) { pos.x, pos.y, surface->w, surface->h };
    SDL_FreeSurface(surface);

    return handle;
}

guielem_handle_t  opendzlgui_create_button
(
    struct SDL_T* sdl,
    char* label,
    struct vec2i pos,
    SDL_Color color,
    void(*callback)(void*),
    int flags
){
    guielem_handle_t handle = -1;
    if(GUI.num_buttons+1 >= OPENDZLGUI_BUTTONS_MAX) {
        fprintf(stderr, "%s: Too many button elements.\n", __func__);
        return handle;
    }

    handle = GUI.num_buttons;
    struct opendzlgui_button* button = &GUI.buttons[handle];
    GUI.num_buttons++;

    struct vec2i text_pos = pos;
    text_pos.x += GUIBUTTON_TEXT_XPADDN;
    text_pos.y += GUIBUTTON_TEXT_YPADDN;

    button->bg_color = color;
    button->callback = callback;
    button->text_handle = opendzlgui_create_text(sdl, label, text_pos, GUIBUTTON_FONT_SIZE, 0);

    button->rect = (SDL_Rect){ pos.x, pos.y, 0, 0 };

    TTF_SizeText(sdl->font, label, &button->rect.w, &button->rect.h);
    button->rect.w += GUIBUTTON_TEXT_XPADDN*2;
    button->rect.h += GUIBUTTON_TEXT_YPADDN*2;

    init_guielem_timers(&button->timers);

    return handle;
}


void opendzlgui_render(struct SDL_T* sdl) {
    struct opendzlgui_button* button = NULL;
    for(uint8_t i = 0; i < GUI.num_buttons; i++) {
        button = &GUI.buttons[i];
        opendzlgui_render_button(sdl, button);
        opendzlgui_update_button(sdl, button);
    }

    struct opendzlgui_text* text = NULL;
    for(uint8_t i = 0; i < GUI.num_texts; i++) {
        text = &GUI.texts[i];
        opendzlgui_render_text(sdl, text);
    }

}



