


#include "opendz_launcher.h"
#include "opendz_launcher_gui.h"


#define OPENDZLGUI_BUTTONS_MAX 16
#define OPENDZLGUI_CHECKBOXES_MAX 16
#define OPENDZLGUI_TEXTS_MAX 16
#define OPENDZLGUI_INPUTFIELDS_MAX 16

#define GUICOLOR_TEXT (SDL_Color){ 230, 200, 160, 255 }
#define GUICOLOR_BUTTON_FG (SDL_Color){ 230, 200, 160, 255 }
#define GUICOLOR_BUTTON_BG (SDL_Color){ 50, 50, 50, 255 }
#define GUICOLOR_BUTTON_SHADOW (SDL_Color){ 20, 20, 20, 255 }
#define GUICOLOR_INPUTF_BG (SDL_Color) { 30, 30, 30, 255 }
#define GUICOLOR_INPUTF_SHADOW (SDL_Color){ 20, 20, 20, 255 }
#define GUICOLOR_INPUTF_OUTLINE (SDL_Color){ 50, 50, 50, 255 }
#define GUICOLOR_INPUTF_OUTLINE_ACTIVE (SDL_Color){ 50, 80, 60, 255 }
#define GUICOLOR_INPUTF_PLACEHOLDER (SDL_Color){ 70, 70, 70, 255 }
#define GUICOLOR_INPUTF_CURSOR (SDL_Color){ 100, 200, 100, 255 }
#define GUICOLOR_CHECKBOX_OFF (SDL_Color){ 50, 50, 50, 255 }
#define GUICOLOR_CHECKBOX_ON (SDL_Color){ 50, 160, 50, 255 }
#define GUICOLOR_CHECKBOX_BG (SDL_Color){ 30, 30, 30, 255 }

#define GUIBUTTON_FONT_SIZE 18
#define GUIINPUTF_FONT_SIZE 18
#define GUIBUTTON_TEXT_XPADDN 5
#define GUIBUTTON_TEXT_YPADDN 2
#define GUICHECKBOX_FONT_SIZE 18
#define GUICHECKBOX_WIDTH 25

static struct GUI_T {

    struct opendzlgui_text texts[OPENDZLGUI_TEXTS_MAX];
    uint8_t num_texts;

    struct opendzlgui_button buttons[OPENDZLGUI_BUTTONS_MAX];
    uint8_t num_buttons;
    
    struct opendzlgui_checkbox checkboxes[OPENDZLGUI_CHECKBOXES_MAX];
    uint8_t num_checkboxes;

    struct opendzlgui_inputfield inputfields[OPENDZLGUI_INPUTFIELDS_MAX];
    uint8_t num_inputfields;

    bool frame_had_input_event;
    float inputfield_cursor_timer;
    guielem_handle_t active_inputfield_handle;
}
GUI = (struct GUI_T) {
    .num_buttons = 0,
    .num_texts = 0,
    .num_inputfields = 0,
    .active_inputfield_handle = -1,
    .inputfield_cursor_timer = 0.0f
};



void* opendzlgui_getelem(guielem_handle_t handle, enum guielem_type_e type) {
    void* ptr = NULL;

    switch(type) {

        // TODO: Handle out of bounds index.

        case GUI_TEXT:
            ptr = (void*)&GUI.texts[handle];
            break;

        case GUI_BUTTON:
            ptr = (void*)&GUI.buttons[handle];
            break;

        case GUI_INPUTFIELD:
                ptr = (void*)&GUI.inputfields[handle];
            break;

        default:
            fprintf(stderr, "%s: Failed to find gui element for type %i\n",
                    __func__, type);
            break;
    }


    return ptr;
}

static void opendzlgui_free_text(struct opendzlgui_text* text) {
     SDL_DestroyTexture(text->texture);
}

void opendzlgui_free() {
    for(uint8_t i = 0; i < GUI.num_texts; i++) {
        opendzlgui_free_text(&GUI.texts[i]);
    }
    for(uint8_t i = 0; i < GUI.num_inputfields; i++) {
        string_free(&GUI.inputfields[i].input);
    }
}


void opendzlgui_reset_guielem_focus() {
    GUI.active_inputfield_handle = -1;
}

void opendzlgui_event_textinput(struct SDL_T* sdl, char* input) {
    if(GUI.active_inputfield_handle < 0) {
        return; // No input field has been selected.
    }

    struct opendzlgui_inputfield* inputfield = opendzlgui_getelem(GUI.active_inputfield_handle, GUI_INPUTFIELD);
    if(!inputfield) {
        return;
    }

    string_append(&inputfield->input, input, strlen(input));

    opendzlgui_update_text(sdl, inputfield->input_text_handle, inputfield->input.bytes, 
            GUIINPUTF_FONT_SIZE, GUICOLOR_TEXT);

    GUI.inputfield_cursor_timer = 0.0f;
}

void opendzlgui_event_keyinput(struct SDL_T* sdl, int key) {
    if(GUI.active_inputfield_handle < 0) {
        return; // No input field has been selected.
    }

    struct opendzlgui_inputfield* inputfield = opendzlgui_getelem(GUI.active_inputfield_handle, GUI_INPUTFIELD);
    if(!inputfield) {
        return;
    }


    if(key == SDLK_BACKSPACE) {
        string_popback(&inputfield->input);
        if(inputfield->input.size == 0) {
            struct opendzlgui_text* text = opendzlgui_getelem(inputfield->input_text_handle, GUI_TEXT);
            if(!text) {
                return;
            }

            text->visible = false;
        }
        else {
            opendzlgui_update_text(sdl, inputfield->input_text_handle, inputfield->input.bytes, 
                GUIINPUTF_FONT_SIZE, GUICOLOR_TEXT);
        }
    }
}

void opendzlgui_update_text(struct SDL_T* sdl, guielem_handle_t handle, char* label, int font_size, SDL_Color color) {
    struct opendzlgui_text* text = opendzlgui_getelem(handle, GUI_TEXT);
    if(!text) { 
        return;
    }

    if(text->texture) {
        SDL_DestroyTexture(text->texture);
    }

    text->color = color;
    text->font_size = font_size;
    TTF_SetFontSize(sdl->font, font_size);
    SDL_Surface* surface = TTF_RenderText_Blended(sdl->font, label, text->color);
    text->texture = SDL_CreateTextureFromSurface(sdl->renderer, surface);

    text->rect.w = surface->w;
    text->rect.h = surface->h;
    SDL_FreeSurface(surface);
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
    if(sdl->mouse_down && mouse_on_rect(sdl, &button->rect)) {
        button->callback(sdl);
    }
}

static void opendzlgui_render_checkbox(
        struct SDL_T* sdl,
        struct opendzlgui_checkbox* checkbox
){
    update_guielem_timers(sdl, &checkbox->rect, &checkbox->timers);

    SDL_Rect shadow_rect = checkbox->box_rect;
    shadow_rect.x += 3;
    shadow_rect.y += 3;
    draw_rect(sdl, &shadow_rect, GUICOLOR_BUTTON_SHADOW);


    draw_rect(sdl, &checkbox->box_rect, GUICOLOR_CHECKBOX_BG);
    draw_rect_ln(sdl, &checkbox->box_rect, 
            color_mult(GUICOLOR_CHECKBOX_BG, 1.85f + checkbox->timers.focus_enter * 0.85f));

    //if(*checkbox->value_ptr) {
        SDL_Rect indicator_rect = checkbox->box_rect;
        indicator_rect.x += 3;
        indicator_rect.y += 3;
        indicator_rect.w -= 6;
        indicator_rect.h -= 6;
        draw_rect(sdl, &indicator_rect, (*checkbox->value_ptr) ? GUICOLOR_CHECKBOX_ON : GUICOLOR_CHECKBOX_OFF);
    //}


    /*
    SDL_Rect shadow_rect = button->rect;
    shadow_rect.x += 3;
    shadow_rect.y += 3;
    draw_rect(sdl, &shadow_rect, GUICOLOR_BUTTON_SHADOW);
    */
    /*
    draw_rect(sdl, &button->rect, 
            color_mult(button->bg_color, 1.0f + button->timers.elem_active * 0.85f));

    draw_rect_ln(sdl, &button->rect, 
            color_mult(button->bg_color, 1.2f + button->timers.focus_enter * 0.65f));
    */
}

static void opendzlgui_update_checkbox(
        struct SDL_T* sdl,
        struct opendzlgui_checkbox* checkbox
){
    if(sdl->mouse_down && mouse_on_rect(sdl, &checkbox->rect)) {
        *checkbox->value_ptr = !*checkbox->value_ptr;
    }
}

static void opendzlgui_render_inputfield(
        struct SDL_T* sdl,
        struct opendzlgui_inputfield* inputfield
){
    update_guielem_timers(sdl, &inputfield->rect, &inputfield->timers);


    SDL_Rect outline_rect = inputfield->rect;
    outline_rect.x -= 2;
    outline_rect.y -= 2;
    outline_rect.w += 4;
    outline_rect.h += 4;

    SDL_Rect bg_rect = inputfield->rect;
    bg_rect.x += 3;
    bg_rect.y += 3;
    bg_rect.w -= 3;
    bg_rect.h -= 3;

    // TODO: Use linear interpolation for outline color.
    //       T should be the timers->elem_active value. 
    //       elem_active value will be decreased when the element is no longer active
    //SDL_Color outline_color = (GUI.active_inputfield_handle == inputfield->handle)
    //    ? GUICOLOR_INPUTF_OUTLINE_ACTIVE : GUICOLOR_INPUTF_OUTLINE;
    
    SDL_Color outline_color = GUICOLOR_INPUTF_OUTLINE;

    draw_rect(sdl, &outline_rect, color_mult(outline_color, 1.0f + inputfield->timers.focus_enter*0.8f));
    draw_rect(sdl, &inputfield->rect, GUICOLOR_INPUTF_SHADOW);
    draw_rect(sdl, &bg_rect, GUICOLOR_INPUTF_BG);


    // Draw cursor.


    if(inputfield->handle == GUI.active_inputfield_handle) {
    
        GUI.inputfield_cursor_timer += sdl->frame_time;
        if(GUI.inputfield_cursor_timer >= 1.0f) {
            GUI.inputfield_cursor_timer = 0.0f;
        }
        if(GUI.inputfield_cursor_timer > 0.5f) {
            return;
        }
        /*
        if(GUI.inputfield_cursor_timer < 0.35f) {
            return;
        }
        if(GUI.inputfield_cursor_timer > 0.7f) {
            GUI.inputfield_cursor_timer = 0.0f;
        }
        */


        int width = 0;
        int height = 0;
        TTF_SizeText(sdl->font, inputfield->input.bytes, &width, &height);

        int cursor_x = inputfield->rect.x + width + GUIBUTTON_TEXT_XPADDN;
        int cursor_y = inputfield->rect.y;

        SDL_Rect cursor_rect = (SDL_Rect) {
            cursor_x+1, cursor_y+5, 3, height-5
        };
        draw_rect(sdl, &cursor_rect, GUICOLOR_INPUTF_CURSOR);
    }
}

static void opendzlgui_update_inputfield(
        struct SDL_T* sdl,
        struct opendzlgui_inputfield* inputfield
){
    if(sdl->mouse_down && mouse_on_rect(sdl, &inputfield->rect)) {
        GUI.active_inputfield_handle = inputfield->handle;
    }

    struct opendzlgui_text* text = opendzlgui_getelem(inputfield->input_text_handle, GUI_TEXT);
    if(!text) {
        return;
    }

    struct opendzlgui_text* place_holder = opendzlgui_getelem(inputfield->place_holder_handle, GUI_TEXT);
    if(!place_holder) {
        return;
    }

    if(inputfield->input.size == 0) {
        text->visible = false;
        place_holder->visible = true;
    }
    else {
        text->visible = true;
        place_holder->visible = false;
    }

}

guielem_handle_t  opendzlgui_create_text
(
    struct SDL_T* sdl,
    char* label,
    struct vec2i pos,
    SDL_Color color,
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

    text->color = color;
    text->font_size = font_size;
   
    size_t label_len = 0;

    if(label) {
        label_len = strlen(label);
    }

    if(label_len > 0) {
        TTF_SetFontSize(sdl->font, font_size);
        SDL_Surface* surface = TTF_RenderText_Blended(sdl->font, label, text->color);
        text->texture = SDL_CreateTextureFromSurface(sdl->renderer, surface);

        text->rect = (SDL_Rect){ pos.x, pos.y, surface->w, surface->h };
        SDL_FreeSurface(surface);
        text->visible = true;
    }
    else {
        // Text may be wanted to be created for future use as empty.
        text->texture = NULL;
        text->visible = false;
        text->rect = (SDL_Rect){ pos.x, pos.y, 0, 0 };
    }
    

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

    button->visible = true;
    button->bg_color = color;
    button->callback = callback;
    button->text_handle = opendzlgui_create_text(sdl, label, text_pos, GUICOLOR_TEXT, GUIBUTTON_FONT_SIZE, 0);
    // TODO: Handle return value of "handle"

    button->rect = (SDL_Rect){ pos.x, pos.y, 0, 0 };

    TTF_SizeText(sdl->font, label, &button->rect.w, &button->rect.h);
    button->rect.w += GUIBUTTON_TEXT_XPADDN*2;
    button->rect.h += GUIBUTTON_TEXT_YPADDN*2;

    init_guielem_timers(&button->timers);

    return handle;
}

guielem_handle_t  opendzlgui_create_checkbox
(
    struct SDL_T* sdl,
    char* label,
    struct vec2i pos,
    bool* value_ptr,
    int flags
){

    guielem_handle_t handle = -1;
    if(GUI.num_checkboxes+1 >= OPENDZLGUI_CHECKBOXES_MAX) {
        fprintf(stderr, "%s: Too many checkbox elements.\n", __func__);
        return handle;
    }

    handle = GUI.num_checkboxes;
    struct opendzlgui_checkbox* checkbox = &GUI.checkboxes[handle];
    GUI.num_checkboxes++;


    checkbox->box_rect = (SDL_Rect) {
        pos.x, pos.y, GUICHECKBOX_WIDTH, 0
    };
    
    struct vec2i text_pos = pos;
    text_pos.x += checkbox->box_rect.w + GUIBUTTON_TEXT_XPADDN;
    text_pos.y += GUIBUTTON_TEXT_YPADDN;


    checkbox->visible = true;
    checkbox->value_ptr = value_ptr;
    checkbox->text_handle = opendzlgui_create_text(sdl, label, text_pos, GUICOLOR_TEXT, GUICHECKBOX_FONT_SIZE, 0);
    // TODO: Handle return value of "handle"

    checkbox->rect = (SDL_Rect){ pos.x, pos.y, 0, 0 };

    TTF_SizeText(sdl->font, label, &checkbox->rect.w, &checkbox->rect.h);
    checkbox->rect.w += GUIBUTTON_TEXT_XPADDN*2 + GUICHECKBOX_WIDTH;
    checkbox->rect.h += GUIBUTTON_TEXT_YPADDN*2;

    checkbox->box_rect.h = checkbox->rect.h; // Make the actual "checkbox" match button height.
    init_guielem_timers(&checkbox->timers);

    return handle;


}


guielem_handle_t  opendzlgui_create_inputfield
(
    struct SDL_T* sdl,
    char* place_holder,
    struct vec2i pos,
    int width,
    int flags
){
    guielem_handle_t handle = -1;
    if(GUI.num_inputfields+1 >= OPENDZLGUI_INPUTFIELDS_MAX) {
        fprintf(stderr, "%s: Too many inputfield elements.\n", __func__);
        return handle;
    }


    handle = GUI.num_inputfields;
    struct opendzlgui_inputfield* inputfield = &GUI.inputfields[handle];
    GUI.num_inputfields++;

    inputfield->input = create_string();

    struct vec2i text_pos = pos;
    text_pos.x += GUIBUTTON_TEXT_XPADDN;
    text_pos.y += GUIBUTTON_TEXT_YPADDN;

    inputfield->input_text_handle = 
        opendzlgui_create_text(sdl, NULL, text_pos, GUICOLOR_TEXT, GUIINPUTF_FONT_SIZE, 0);
    // TODO: Handle return value of "handle"

    text_pos.x += 4;
    inputfield->place_holder_handle = 
        opendzlgui_create_text(sdl, place_holder, text_pos, GUICOLOR_INPUTF_PLACEHOLDER, GUIINPUTF_FONT_SIZE, 0);
    // TODO: Handle return value of "handle"


    inputfield->rect = (SDL_Rect){ pos.x, pos.y, 0, 0 };
    TTF_SizeText(sdl->font, place_holder, &inputfield->rect.w, &inputfield->rect.h);
    
    if(inputfield->rect.w < width) {
        inputfield->rect.w = width;
    }

    inputfield->rect.w += GUIBUTTON_TEXT_XPADDN * 2;
    inputfield->rect.h += GUIBUTTON_TEXT_YPADDN * 2;

    inputfield->visible = true;
    inputfield->handle = handle;
    
    init_guielem_timers(&inputfield->timers);
    return handle;
}



void opendzlgui_render(struct SDL_T* sdl) {
    struct opendzlgui_button* button = NULL;
    for(uint8_t i = 0; i < GUI.num_buttons; i++) {
        button = &GUI.buttons[i];
        if(!button->visible) { continue; }
        opendzlgui_update_button(sdl, button);
        opendzlgui_render_button(sdl, button);
    }

    struct opendzlgui_checkbox* checkbox = NULL;
    for(uint8_t i = 0; i < GUI.num_checkboxes; i++) {
        checkbox = &GUI.checkboxes[i];
        if(!checkbox->visible) { continue; }
        opendzlgui_update_checkbox(sdl, checkbox);
        opendzlgui_render_checkbox(sdl, checkbox);
    }
    struct opendzlgui_inputfield* inputf = NULL;
    for(uint8_t i = 0; i < GUI.num_inputfields; i++) {
        inputf = &GUI.inputfields[i];
        if(!inputf->visible) { continue; }
        opendzlgui_update_inputfield(sdl, inputf);
        opendzlgui_render_inputfield(sdl, inputf);
    }

    struct opendzlgui_text* text = NULL;
    for(uint8_t i = 0; i < GUI.num_texts; i++) {
        text = &GUI.texts[i];
        if(!text->visible) { continue; }
        opendzlgui_render_text(sdl, text);
    }
}



