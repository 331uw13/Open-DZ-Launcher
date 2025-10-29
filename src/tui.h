#ifndef OPENDZL_TUI_H
#define OPENDZL_TUI_H


#include <stdint.h>

#define TUI_LABEL_MAX_LENGTH 32


// Tui element types.
#define TUI_ELEM_T_UNKNOWN -1
#define TUI_ELEM_T_BUTTON 0
#define TUI_ELEM_T_INPUTFIELD 1


struct telem_inputfield {
    char*  buffer;
    size_t buffer_memsize;
};

struct telem_button {
    void (*use_callback)(void*);
};





struct tui_element {
    int type;
    char label[TUI_LABEL_MAX_LENGTH];

    union {
        struct telem_inputfield inputfield;
        struct telem_button     button;
    };

    int x, y, width, height;
};


struct opendzltui {
    int cursor_x;
    int cursor_y;
    uint16_t max_rows;
    uint16_t max_cols;
    struct tui_element** element_map;
};

void opendzltui_create(struct opendzltui* tui, uint16_t max_cols, uint16_t max_rows);
void opendzltui_free(struct opendzltui* tui);
void opendzltui_draw(struct opendzltui* tui);

void opendzltui_create_button
(
    struct opendzltui* tui,
    uint16_t x,
    uint16_t y,
    const char* label,
    void(*use_callback)(void*)
);



#endif
