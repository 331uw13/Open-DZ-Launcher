#include <ncurses.h>

#include "opendz_launcher.h"




void opendzl_handle_input(struct opendzl_ctx* ctx) {

    char ch = getch();

    mvprintw(1, 0, "Input: %c / %i", ch, ch);

   
    switch(ch) {

        case 'q':
            ctx->running = false;
            break;

        case KEY_LEFT:
            break;
        
        case KEY_RIGHT:
            break;

        case KEY_UP:
            break;

        case KEY_DOWN:
            break;

        case KEY_BACKSPACE:
            break;

        case KEY_ENTER:
            break;

    }

}



