#ifndef OPENDZL_GUI_H
#define OPENDZL_GUI_H

#include "vec2.h"
#include "string.h"

#define OPENDZLGUI_LABEL_MAX 32


struct SDL_T;
typedef int guielem_handle_t;

struct guielem_timers {
    float focus_enter;
    float elem_active;
};

struct opendzlgui_text {
    bool visible;
    SDL_Rect rect;
    SDL_Texture* texture;
    SDL_Color color;
    int font_size;
};

struct opendzlgui_button {
    bool visible;
    //char label[OPENDZLGUI_LABEL_MAX];
    guielem_handle_t text_handle;     // Handle for opendzlgui_text
    SDL_Rect rect;
    SDL_Color bg_color;
    void(*callback)(void*);

    struct guielem_timers timers;
};

struct opendzlgui_checkbox {
    bool visible;
    guielem_handle_t text_handle;     // Handle for opendzlgui_text
    SDL_Rect rect;
    SDL_Rect box_rect;
    bool* value_ptr;
    
    struct guielem_timers timers;
};

struct opendzlgui_inputfield {
    bool visible;
    struct string_t input;
    guielem_handle_t handle;
    guielem_handle_t input_text_handle;
    guielem_handle_t place_holder_handle; // Handle for opendzlgui_text
    SDL_Rect rect;
    
    struct guielem_timers timers;
};

enum guielem_type_e {
    GUI_TEXT,
    GUI_BUTTON,
    GUI_INPUTFIELD,
    GUI_CHECKBOX
};


void opendzlgui_free();
void opendzlgui_render(struct SDL_T* sdl);
void* opendzlgui_getelem(guielem_handle_t handle, enum guielem_type_e type);

// Create input event. Needed to update selected input field elements.
void opendzlgui_event_textinput(struct SDL_T* sdl, char* input);
void opendzlgui_event_keyinput(struct SDL_T* sdl, int key);

// Reset all input fields focus.
void opendzlgui_reset_guielem_focus();

// Update label, font size and color for text.
// NOTE: This it not very efficient function because it recreates the text texture.
void opendzlgui_update_text(struct SDL_T* sdl, guielem_handle_t handle, char* label, int font_size, SDL_Color new_color);


guielem_handle_t  opendzlgui_create_text
(
    struct SDL_T* sdl,
    char* label,
    struct vec2i pos,
    SDL_Color color,
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

guielem_handle_t  opendzlgui_create_inputfield
(
    struct SDL_T* sdl,
    char* place_holder,
    struct vec2i pos,
    int width,
    int flags
);

guielem_handle_t  opendzlgui_create_checkbox
(
    struct SDL_T* sdl,
    char* label,
    struct vec2i pos,
    bool* value_ptr,
    int flags
);





#endif
