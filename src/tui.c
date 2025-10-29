#include <stdlib.h>
#include <string.h>

#include "tui.h"



void opendzltui_create(struct opendzltui* tui, uint16_t max_cols, uint16_t max_rows) {
    
    tui->element_map = malloc(sizeof **tui->element_map * max_rows);
    for(uint16_t r = 0; r < max_rows; r++) {
        tui->element_map[r] = malloc(sizeof **tui->element_map * max_cols);
    }


    for(uint16_t x = 0; x < max_cols; x++) {
        for(uint16_t y = 0; y < max_rows; y++) {
            tui->element_map[x][y].type = TUI_ELEM_T_UNKNOWN;
        }
    }
    
    tui->cursor_x = 0;
    tui->cursor_y = 0;
    tui->max_rows = max_rows;
    tui->max_cols = max_cols;
}

void opendzltui_free(struct opendzltui* tui) {
    for(uint16_t r = 0; r < tui->max_rows; r++) {
        free(tui->element_map[r]);
    }
    free(tui->element_map);
}


void opendzltui_draw(struct opendzltui* tui) {
    for(uint16_t y = 0; y < tui->max_rows; y++) {
        for(uint16_t x = 0; x < tui->max_rows; x++) {
            struct tui_element* telem = &tui->element_map[x][y];


            // Continue from here...

        }
    }
}

void opendzltui_create_button
(
    struct opendzltui* tui,
    uint16_t x,
    uint16_t y,
    const char* label,
    void(*use_callback)(void*)
){
    if((x >= tui->max_cols) || (y >= tui->max_rows)) {
        return;
    }

    size_t label_len = strlen(label);
    struct tui_element* telem = &tui->element_map[x][y];
    telem->type = TUI_ELEM_T_BUTTON;

    if(label_len >= TUI_LABEL_MAX_LENGTH) {
        label_len = TUI_LABEL_MAX_LENGTH - 1;
    }

    telem->x = x;
    telem->y = y;
    telem->width = label_len;
    telem->height = 1;

    memcpy(telem->label, label, label_len);
}


