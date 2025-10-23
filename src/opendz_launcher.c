#include <stdio.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_ttf.h>

#include "string.h"
#include "server_query.h"
#include "a2s_parser.h"
#include "a2s_decoder.h"
#include "opendz_launcher.h"



static struct SDL_T {
    SDL_Window* window;
    SDL_GLContext* context;
    SDL_Renderer* renderer;
    TTF_Font* font;
    int win_width;
    int win_height;
}
SDL = (struct SDL_T) {
    .window = NULL,
    .context = NULL
};


void opendzl_create_rtext(struct rtext* text, char* buf, int font_size, int color) {
    TTF_SetFontSize(SDL.font, font_size);
    
    SDL_Color text_color = (SDL_Color){ 
        (color & 0xFF0000) >> 16,
        (color & 0x00FF00) >> 8,
        (color & 0x0000FF),
        255
    };

    SDL_Surface* surface = TTF_RenderText_Blended(SDL.font, buf, text_color);
    text->texture = SDL_CreateTextureFromSurface(SDL.renderer, surface);


    text->rect = (SDL_Rect) { 0, 0, surface->w, surface->h };
    SDL_FreeSurface(surface);
}

void opendzl_render_rtext(struct rtext* text, int x, int y) {
    text->rect.x = x;
    text->rect.y = y;
    SDL_RenderCopy(SDL.renderer, text->texture, NULL, &text->rect);
}

void opendzl_free_rtext(struct rtext* text) {
    SDL_DestroyTexture(text->texture);
}


bool opendzl_get_server_info(char* addr, uint16_t port, struct dayz_server* server) {
    bool result = false;

    struct string_t server_info = create_string();
    struct string_t server_mod_info = create_string();

    if(!get_server_a2s_responses(addr, port, &server_info, &server_mod_info)) {
        fprintf(stderr, "Failed to get correct server A2S responses.\n");
        goto out;
    }


    if(server_info.size < 4) {
        fprintf(stderr, "Failed to get good server info response. Got %i bytes.\n",
                server_info.size);
        goto out;
    }

    if(server_mod_info.size < 4) {
        fprintf(stderr, "Failed to get good server mod info response. Got %i bytes.\n",
                server_mod_info.size);
        goto out;
    }


    uint8_t decoded[4096] = { 0 };
    if(!decode_a2s_escapes((uint8_t*)server_mod_info.bytes, server_mod_info.size,
            decoded, sizeof(decoded))) {
        fprintf(stderr, "Failed to decode A2S escape sequences.\n");
        goto out;
    }

    if(!parse_a2s_rules(decoded, sizeof(decoded), server->mods, &server->num_mods)) {
        server->num_mods = 0;
        //goto out;
    }


    memset(server->name, 0, OPENDZL_MAX_SERVER_NAME);
    memset(server->map_name, 0, OPENDZL_MAX_SERVER_MAP_NAME);

    server->name_length = 0;
    server->map_name_length = 0;

    //
    //   Steam header |???| Server name | Game map name | 
    //                |   |             |               |
    //     FF FF FF FF I11 EXAMPLE NAME0 chernarusplus0 ...
    // 
    // First 6 bytes can be ignored.
    

    // Read server name.
    for(uint32_t i = 6; i < server_info.size; i++) {
        char ch = server_info.bytes[i];
        if(ch == 0) {
            break;
        }
        server->name[server->name_length++] = ch;
    }

    // Read server map name.
    for(uint32_t i = 7+server->name_length; i < server_info.size; i++) {
        char ch = server_info.bytes[i];
        if(ch == 0) {
            break;
        }
        server->map_name[server->map_name_length++] = ch;
    }

    /*
    for(size_t i = 0; i < server_info.size; i++) {
        char byte = server_info.bytes[i];
        if(byte >= 0x20 && byte <= 0x7E) {
            printf("%c", byte);
        }
        else {
            printf("\033[34m%02X\033[0m ", (uint8_t)byte);
        }
    }

    printf("\n");
    */

    result = true;

out:
    string_free(&server_info);
    string_free(&server_mod_info);

    return result;
}


bool opendzl_init_sdl3() {
    SDL.window = NULL;
    SDL.context = NULL;
    SDL.renderer = NULL;


    SDL.window = SDL_CreateWindow("Open-DZ-Launcher", 
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, 700, 400, SDL_WINDOW_OPENGL);
    if(!SDL.window) {
        fprintf(stderr, "SDL failed to create window | %s\n", SDL_GetError());
        return false;
    }

    SDL.context = SDL_GL_CreateContext(SDL.window);
    if(!SDL.context) {
        fprintf(stderr, "SDL failed to create context | %s\n", SDL_GetError());
        return false;
    }

    const int renderer_index = -1;
    const int renderer_flags = 0;
    SDL.renderer = SDL_CreateRenderer(SDL.window, renderer_index, renderer_flags);
    if(!SDL.renderer) {
        fprintf(stderr, "SDL failed to create renderer | %s\n", SDL_GetError());
        return false;
    }

    SDL_GetWindowSize(SDL.window, &SDL.win_width, &SDL.win_height);

    return true;
}

bool opendzl_load_font(const char* path) {
    TTF_Init();
    SDL.font = NULL;
    SDL.font = TTF_OpenFont(path, 64);
    if(!SDL.font) {
        fprintf(stderr, "SDL failed to load font \"%s\" | %s\n", path, SDL_GetError());
        return false;
    }

    return true;
}

void opendzl_run() {


    SDL_Event event;
    bool running = true;


    struct rtext name_text;
    opendzl_create_rtext(&name_text, "Open-DZ-Launcher", 23, 0xCFAEECA);


    while(running) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    running = false;
                    break;


            }
        }

        SDL_SetRenderDrawColor(SDL.renderer, 35, 33, 30, 255);
        SDL_RenderClear(SDL.renderer);

        opendzl_render_rtext(&name_text, 10, SDL.win_height-30);


        SDL_RenderPresent(SDL.renderer);
        SDL_GL_SwapWindow(SDL.window);
    }

    opendzl_free_rtext(&name_text);
}

void opendzl_free() {
    if(SDL.renderer) {
        SDL_DestroyRenderer(SDL.renderer);
        SDL.renderer = NULL;
    }
    if(SDL.context) {
        SDL_GL_DeleteContext(SDL.context);
        SDL.context = NULL;
    }
    if(SDL.window) {
        SDL_DestroyWindow(SDL.window);
        SDL.window = NULL;
    }

    TTF_Quit();
}









