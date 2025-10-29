#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "string.h"
#include "server_query.h"
#include "a2s_parser.h"
#include "a2s_decoder.h"
#include "opendz_launcher.h"

#include "leaf/leaf.h"
#include "leaf/gui/gui.h"



char* mods[] = {
    "3031784065",
    "3417067137",
    "1559212036",
    "2572331007",
    "2394804821",
    "2545327648",
    "1582756848",
    //"3475246850", // Impala.
    "1644467354", // Summer chernarus.
};

int num_mods = 8;


void cmd_add_steam_applaunch(struct string_t* cmd) {
    char* applaunch = "steam -applaunch 221100 -nolauncher ";
    string_move(cmd, applaunch, strlen(applaunch));
}


void link_mods() {

    char* mods_dir = "~/.local/share/Steam/steamapps/workshop/content/221100/";
    char* dayz_path = "~/.local/share/Steam/steamapps/common/DayZ/";

    struct string_t cmd = create_string();

    for(size_t i = 0; i < num_mods; i++) {
       
        string_clear(&cmd);
        string_append(&cmd, "ln -sr ", 7);
        string_append(&cmd, mods_dir, strlen(mods_dir));
        string_append(&cmd, mods[i], strlen(mods[i]));
        string_pushbyte(&cmd, ' ');

        string_append(&cmd, dayz_path, strlen(dayz_path));
        string_pushbyte(&cmd, '@');
        string_append(&cmd, mods[i], strlen(mods[i]));

        system(cmd.bytes);

        printf("%s\n", cmd.bytes);

        //printf("%s, %s\n", dayz_path, mods[i]);
    }

    string_free(&cmd);
}

void cmd_add_mods(struct string_t* cmd) {
    string_append(cmd, "-mod='", 6);

    for(size_t i = 0; i < num_mods; i++) {
    
        string_pushbyte(cmd, '@');
        string_append(cmd, mods[i], strlen(mods[i]));
        if(i+1 < num_mods) {
            string_pushbyte(cmd, ';');
        }
    }

    string_pushbyte(cmd, '\'');
    string_pushbyte(cmd, ' ');
}

void cmd_add_destination(struct string_t* cmd, char* host, char* port) {
    string_append(cmd, "-connect=", 9);
    string_append(cmd, host, strlen(host));
    string_pushbyte(cmd, ' ');
    string_append(cmd, "-port=", 6);
    string_append(cmd, port, strlen(port));

}

void start_dayz() {
  
    //link_mods();  // TODO: Create a check for symlink if they exists. create them.
    // And should maybe not use system() for that.

    struct string_t cmd = create_string();

    cmd_add_steam_applaunch(&cmd);
    cmd_add_mods(&cmd);
    cmd_add_destination(&cmd, "192.168.1.141", "2304");


    printf("%s\n", cmd.bytes);
   
    system(cmd.bytes);
    string_free(&cmd);
     


}
/*
void print_server_info(char* addr, uint16_t port) {

    struct dayz_server server_info;


    printf("–––––––––––––––––––––––––––––––––––––––––––––––––––\n");
    printf("\033[90mA2S Response from: (%s : %i)\033[0m\n", addr, port);
    printf("\n");
    if(!opendzl_get_server_info(addr, port, &server_info)) {
        fprintf(stderr, "opendzl_get_server_info() Failed\n");
        return;
    }

    printf("'%s'\n", server_info.name);
    printf("'%s'\n", server_info.map_name);
    printf(" %i Mods\n", server_info.num_mods);

    for(uint8_t i = 0; i < server_info.num_mods; i++) {
        printf("%s \033[32m%u\033[0m\n", 
                server_info.mods[i].name,
                server_info.mods[i].workshop_id);
    }

    printf("-----------------------------------------------------\n");
}
*/


static bool AUTO_SETUP_MODS = true;
static int COUNTER = 0;
static char SERVER_ADDR[16] = { 0 };
static char SERVER_PORT[6] = { '2','3','0','2' };
static char SERVER_QUERY_PORT[6] = { '2', '7', '0', '1', '6' };
static struct dayz_server server_info;

enum guiview_e {
    GUI_VIEW_MAIN_PAGE,
    GUI_VIEW_JOIN_SERVER_PAGE
};


bool setup_mods_for_server() {

    server_info = (struct dayz_server){ 0 };

    const int query_port = atoi(SERVER_QUERY_PORT);
    if(!opendzl_get_server_info(SERVER_ADDR, query_port, &server_info)) {
        fprintf(stderr, "Failed to get server info.\n");
        return false;
    }

    for(uint8_t i = 0; i < server_info.num_mods; i++) {
        printf("%s  -  %u\n", server_info.mods[i].name, server_info.mods[i].workshop_id);
    }

    return true;
}

void handle_connect_button() {
    
    if(AUTO_SETUP_MODS) {
        if(!setup_mods_for_server()) {
            return;
        }
    }

}


void update_guiview_main_page(struct font_t* font, struct leaf_ctx_t* ctx) { 
    leaf_set_font_scale(font, 1.0f);
    leaf_set_font_spacing(font, 2.0f);
    
    leafgui_enable(LEAFGUI_SAME_LINE);

    leafgui_input(0/*ID*/, font, "server address", 
            LEAFGUI_AUTOPOS, 150,
            SERVER_ADDR, sizeof(SERVER_ADDR));


    leafgui_input(1/*ID*/, font, "port",
            LEAFGUI_AUTOPOS, 100,
            SERVER_PORT, sizeof(SERVER_PORT));


    leafgui_input(2/*ID*/, font, "query port",
            LEAFGUI_AUTOPOS, 100, 
            SERVER_QUERY_PORT, sizeof(SERVER_QUERY_PORT));


    if(leafgui_button(font, "connect", LEAFGUI_AUTOPOS)) {
        handle_connect_button();
    }
    
    leafgui_checkbox(font, "auto setup mods", LEAFGUI_AUTOPOS, &AUTO_SETUP_MODS);

    leafgui_disable(LEAFGUI_SAME_LINE);

    /*
    static int region_scroll = 0.0f;
    leafgui_region_begin(NULL, LEAFGUI_AUTOPOS, ctx->win_width-100, 320, &region_scroll);
    
    leafgui_region_end(); 
    */
}


void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        leafgui_event_mouse_down();
    }
}

void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    leafgui_event_mouse_scroll(yoffset);
}

void glfw_character_callback(GLFWwindow* window, unsigned int codepoint) {
    leafgui_event_char_input(codepoint);
}

void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if((action == GLFW_PRESS) || (action == GLFW_REPEAT)) {
        leafgui_event_key_input(key);
    }
}

int main() {
    //start_dayz(); return 0;


    const char* font_file = "OpenSans-Bold.ttf";
    const size_t renderer_max_vertices = 1024 * sizeof(float);
  

    struct leaf_ctx_t* leaf_ctx = leaf_open("Open-DZ-Launcher", 800, 400);
    if(!leaf_ctx) {
        return 1;
    }

    struct font_t font;
    if(!leaf_load_font(&font, font_file)) {
        return 1;
    }


    glfwSetMouseButtonCallback (leaf_ctx->glfw_win, glfw_mouse_button_callback);
    glfwSetScrollCallback      (leaf_ctx->glfw_win, glfw_scroll_callback);
    glfwSetCharCallback        (leaf_ctx->glfw_win, glfw_character_callback);
    glfwSetKeyCallback         (leaf_ctx->glfw_win, glfw_key_callback);

    leaf_init_renderer(leaf_ctx, renderer_max_vertices);
    leafgui_init_context(leaf_ctx);

    while(!glfwWindowShouldClose(leaf_ctx->glfw_win)) {
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //update_gui(&font, leaf_ctx);
        update_guiview_main_page(&font, leaf_ctx); 

        glfwSwapBuffers(leaf_ctx->glfw_win);
        glfwWaitEvents();
    }

    leaf_unload_font(&font);
    leaf_free_renderer(leaf_ctx);
    leaf_quit(leaf_ctx);

    return 0;
}


    /*
    if(!opendzl_init_sdl3()) {
        return 1;
    }

    if(!opendzl_load_font("DejaVuSansCondensed.ttf")) {
        return 1;
    }

    opendzl_run();
    opendzl_free();
    */
    //link_mods();
    //start_dayz();


    /*
    print_server_info("192.168.1.141", 27016);
    print_server_info("168.100.163.22", 27016);
    print_server_info("103.152.197.191", 2303);
    print_server_info("189.127.165.207", 27016);
    */
     

    /*
    struct string_t server_info = create_string();
    struct string_t server_mod_info = create_string();

    get_server_a2s_responses("185.207.214.54", 2305, &server_info, &server_mod_info);



    printf("\033[90m\n––––––––––––––––––––––––––––––\033[0m\n");
    for(size_t i = 0; i < server_info.size; i++) {
        char byte = server_info.bytes[i];
        if(byte >= 0x20 && byte <= 0x7E) {
            printf("%c", byte);
        }
        else {
            printf("\033[34m%02X\033[0m ", (uint8_t)byte);
        }
    }
   

    uint8_t decoded[4096] = { 0 };

    decode_a2s_escapes((uint8_t*)server_mod_info.bytes, server_mod_info.size,
            decoded, sizeof(decoded));


    struct dayz_mod  mods[OPENDZL_MAX_SERVER_MODS];
    uint8_t          num_mods = 0;

    
    parse_a2s_rules(decoded, sizeof(decoded), 
            mods, &num_mods);

    for(uint16_t i = 0; i < num_mods; i++) {
        printf("'%s' %u\n", mods[i].name, mods[i].workshop_id);
    }

    printf("Server has %i mods\n", num_mods);




    //parse_a2s_rules((uint8_t*)server_mod_info.bytes, server_mod_info.size);
    */




