#if defined(_MSC_VER) || defined(__ANDROID__)

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#elif defined(TARGET_OS_IPHONE)

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>

#include "../../../platforms/IOS/screen_dimensions.h"

#else

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <unistd.h>

#endif

#include "../../../non_core/menu.h"
#include "../../../non_core/filebrowser.h"
#include "../../../non_core/logger.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifdef __SWITCH__
#include <switch.h>

#define JOY_A     0
#define JOY_B     1
#define JOY_X     2
#define JOY_Y     3
#define JOY_PLUS  10
#define JOY_MINUS 11
#define JOY_LEFT  12
#define JOY_UP    13
#define JOY_RIGHT 14
#define JOY_DOWN  15

#endif

#ifdef PSVITA
#include <psp2/ctrl.h>

#define SDLK_VITA_TRIANGLE 0
#define SDLK_VITA_CIRCLE 1 
#define SDLK_VITA_CROSS 2
#define SDLK_VITA_SQUARE 3

#define SDLK_VITA_LTRIGGER 4
#define SDLK_VITA_RTRIGGER 5

#define SDLK_VITA_DOWN 6
#define SDLK_VITA_LEFT 7
#define SDLK_VITA_UP 8
#define SDLK_VITA_RIGHT 9

#define SDLK_VITA_SELECT 10
#define SDLK_VITA_START 11
#endif

#if defined(__ANDROID__) || defined(TARGET_OS_IPHONE)
#include "../../../non_core/joypad.h"
#endif

static SDL_Texture *overlay_t;

typedef enum {NAV_DOWN, NAV_UP, NAV_LEFT, NAV_RIGHT, NAV_SELECT, NAV_BACK} navigate_option_t;

static int button_config[] = {SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT, SDLK_a, SDLK_s, SDLK_RETURN, SDLK_SPACE};

// TODO move
// lots of different platforms Switch, Windows, iOS, Android, etc.
// store specifics to render for that platform
typedef struct {
    int screen_width;
    int screen_height;
    int banner_width_percentage;
    int banner_height_percentage;
    
    int text_x_start;
    int text_y_start;
    int text_y_spacing;
    int screen_text_area_width_ratio_top;
    int screen_text_area_width_ratio_bottom;

//    char *boxart_path;
} platform_config;


typedef struct {
    SDL_Texture **textures;
    uint32_t count;
} dir_fname_textures_t;

typedef struct {
    platform_config *config;
    dir_fname_textures_t *fname_textures;
    dir_t *dir;
    SDL_Renderer *renderer;
    SDL_Texture *screen_texture;
    uint32_t current_highlighted;
    uint32_t scroll_offset; // How many filenames are above the top displayed 
    char *selected_rom_path; 
} context_t;

typedef struct {
    int x;
    int y;
    int w;
    int h;
} b_rect;

typedef struct {
    int key_code; // Key code on the keyboard which
                       // maps to the given gameboy button
    int state; // 1 pressed, 0 unpressed
    b_rect rect;
} button_state;

void navigation(context_t *context, navigate_option_t navigate_option);

static button_state buttons[8];

#define TOTAL_BUTTONS (sizeof(buttons)/sizeof(buttons[0]))


enum {UP = 0, DOWN, LEFT, RIGHT, A, B, START, SELECT};

#define BG_COLOR_RED 77
#define BG_COLOR_GREEN 77
#define BG_COLOR_BLUE 77

#define DIR_COLOR_RED 43
#define DIR_COLOR_GREEN 217
#define DIR_COLOR_BLUE 255

SDL_Window *screen;
static SDL_DisplayMode current;


// Given relative screen x and y positions and an on/off state
// sets any buttons those co-ordinates are in to the given state.
static void check_keys_pressed(context_t *context, float x, float y, int state) {
    float p_x = x * current.w;
    float p_y = y * current.h;
            
     log_message(LOG_INFO, "x: %f,  y: %f\n", p_x, p_y);
   
     for (size_t i = 0; i < TOTAL_BUTTONS; i++) {
        if (p_x >= buttons[i].rect.x && 
                p_x <= buttons[i].rect.x + buttons[i].rect.w &&
                p_y >= buttons[i].rect.y &&
                p_y <= buttons[i].rect.y + buttons[i].rect.h) {
            

            // If activating button, send rumble feedback
            if (/*rumble_on*/ 0 && !buttons[i].state && state) {
               // SDL_HapticRumblePlay(haptic, 0.5, 100);
               #if defined(__ANDROID__)
                  //vibrate();
               #endif
            }
            buttons[i].state = state;
            if (state)
            {
                log_message(LOG_INFO, "button_pressed: %d\n", buttons[i].key_code);
                   if (buttons[i].key_code == buttons[UP].key_code)
                   {
                        log_message(LOG_INFO, "up pressed\n");
                         navigation(context, NAV_UP);
                   }
                   if (buttons[i].key_code == buttons[DOWN].key_code)
                   {
                        log_message(LOG_INFO, "down pressed\n");
                        navigation(context, NAV_DOWN);
                   }
                   if (buttons[i].key_code == buttons[LEFT].key_code)
                   {
                        log_message(LOG_INFO, "left pressed\n");
                   }
                   if (buttons[i].key_code == buttons[RIGHT].key_code)
                   {
                        log_message(LOG_INFO, "right pressed\n");
                   }
                   if (buttons[i].key_code == buttons[A].key_code)
                   {
                        log_message(LOG_INFO, "a pressed\n");
                        navigation(context, NAV_SELECT);
                   }
                   if (buttons[i].key_code == buttons[B].key_code)
                   {
                        log_message(LOG_INFO, "b pressed\n");
                   }
                   if (buttons[i].key_code == buttons[START].key_code)
                   {
                        log_message(LOG_INFO, "start pressed\n");
                   }
                   if (buttons[i].key_code == buttons[SELECT].key_code)
                   {
                        log_message(LOG_INFO, "select pressed\n");
                   }
             }
        }
    }
}

// Given new x/y relative screen positions and x/y movement 
// determines which keys are now pressed
static void check_keys_moved(float x, float y, float mv_x, float mv_y) {
    // Pixel positions after movement
    float p_x = x * current.w;
    float p_y = y * current.h;

    // Pixel positions before movement
    float old_p_x = (x - mv_x) * current.w;
    float old_p_y = (y - mv_y) * current.h;
   
    int activated = 0;

     for (size_t i = 0; i < TOTAL_BUTTONS; i++) {
        int was_on = (old_p_x >= buttons[i].rect.x) &&
                     (old_p_x <= buttons[i].rect.x + buttons[i].rect.w) &&
                     (old_p_y >= buttons[i].rect.y) &&
                     (old_p_y <= buttons[i].rect.y + buttons[i].rect.h);

        int is_on = (p_x >= buttons[i].rect.x) && 
                    (p_x <= buttons[i].rect.x + buttons[i].rect.w) &&
                    (p_y >= buttons[i].rect.y) &&
                    (p_y <= buttons[i].rect.y + buttons[i].rect.h);
            
        if (!was_on && is_on) {
            activated = 1;
            buttons[i].state = 1;           
        }

        if (was_on && !is_on) {
            buttons[i].state = 0;
        }    
    } 

         
    // If activating button, send rumble feedback
    if (/*rumble_on*/0 && activated) {
        // SDL_HapticRumblePlay(haptic, 0.5, 100);
        #if defined(__ANDROID__)
        //vibrate();
        #endif
    }
}


#if defined(__ANDROID__) || defined(TARGET_OS_IPHONE)
static void init_vbutton_texture(SDL_Renderer *renderer) {
    // Setup buttons 
    SDL_GetCurrentDisplayMode(0, &current);    
     
    buttons[UP].state = 0;
    buttons[UP].key_code = button_config[UP];
    b_rect rect_u = {DPAD_UP_X, DPAD_UP_Y(current.h), DPAD_UP_W, DPAD_UP_H};
    buttons[UP].rect = rect_u;

    buttons[DOWN].state = 0;
    buttons[DOWN].key_code = button_config[DOWN];
    b_rect rect_d = {DPAD_DOWN_X, DPAD_DOWN_Y(current.h), DPAD_DOWN_W, DPAD_DOWN_H};
    buttons[DOWN].rect = rect_d; 

    buttons[LEFT].state = 0;
    buttons[LEFT].key_code = button_config[LEFT];
    b_rect rect_l = {DPAD_LEFT_X, DPAD_LEFT_Y(current.h), DPAD_LEFT_W, DPAD_LEFT_H};
    buttons[LEFT].rect = rect_l; 

    buttons[RIGHT].state = 0;
    buttons[RIGHT].key_code = button_config[RIGHT];
    b_rect rect_r = {DPAD_RIGHT_X, DPAD_RIGHT_Y(current.h), DPAD_RIGHT_W, DPAD_RIGHT_H};
    buttons[RIGHT].rect = rect_r; 

    buttons[A].state = 0;
    buttons[A].key_code = button_config[A];
    b_rect rect_a = {A_X(current.w), A_Y(current.h), A_W, A_H};
    buttons[A].rect = rect_a; 

    buttons[B].state = 0;
    buttons[B].key_code = button_config[B];
    b_rect rect_b = {B_X(current.w), B_Y(current.h), B_W, B_H};
    buttons[B].rect = rect_b; 

    buttons[START].state = 0;
    buttons[START].key_code = button_config[START];
    b_rect rect_st = {START_X, START_Y(current.h), START_W, START_H};
    buttons[START].rect = rect_st; 
   
    buttons[SELECT].state = 0;
    buttons[SELECT].key_code = button_config[SELECT];




    SDL_Surface *overlay = SDL_CreateRGBSurface(0, current.w, current.h, 32,
                                        0x00FF0000,
                                        0x0000FF00,
                                        0x000000FF,
                                        0xFF000000);
   
    // Draw DPad
    SDL_Rect left = {DPAD_LEFT_X, DPAD_LEFT_Y(current.h), DPAD_LEFT_W, DPAD_LEFT_H};
    SDL_Rect right = {DPAD_RIGHT_X, DPAD_RIGHT_Y(current.h), DPAD_RIGHT_W, DPAD_RIGHT_H};
    SDL_Rect up = {DPAD_UP_X, DPAD_UP_Y(current.h), DPAD_UP_W, DPAD_UP_H};
    SDL_Rect down = {DPAD_DOWN_X, DPAD_DOWN_Y(current.h), DPAD_DOWN_W, DPAD_DOWN_H};

    // Draw Start/Select
    SDL_Rect start = {START_X, START_Y(current.h), START_W, START_H};
    SDL_Rect select = {SELECT_X, SELECT_Y(current.h), SELECT_W, SELECT_H};

    // Draw A/B
    SDL_Rect a = {A_X(current.w), A_Y(current.h), A_W, A_H};
    SDL_Rect b = {B_X(current.w), B_Y(current.h), B_W, B_H};
    
    Uint32 b_color = SDL_MapRGB(overlay->format, 0x95, 0x8E, 0x8E);
 
    SDL_FillRect(overlay, &left, b_color);
    SDL_FillRect(overlay, &right, b_color);
    SDL_FillRect(overlay, &down, b_color);
    SDL_FillRect(overlay, &up, b_color);
    SDL_FillRect(overlay, &start, b_color);
    SDL_FillRect(overlay, &select, b_color);
    SDL_FillRect(overlay, &a, b_color);
    SDL_FillRect(overlay, &b, b_color);

    SDL_SetSurfaceBlendMode(overlay, SDL_BLENDMODE_BLEND);

    // Make overlay partially transparent
    SDL_SetSurfaceAlphaMod(overlay, 0x7F);
    overlay_t = SDL_CreateTextureFromSurface(renderer, overlay);

    #if defined(__ANDROID__) || (defined(__APPLE__) && TARGET_OS_IPHONE)  
        SDL_RenderCopy(renderer, overlay_t, NULL, NULL);
        log_message(LOG_INFO, "copied overlay\n");
    #endif
}
#endif

// Main Menu
/* --------------------- */

// Creates top Banner, bottom Row options
// and background
void draw_select_game_bg(SDL_Renderer *renderer, platform_config *config) {
    
   // 15% top for banner
   // 10% bottom for controls
   SDL_Rect bg_rect = {0, 0, config->screen_width, config->screen_height};
    
   SDL_SetRenderDrawColor(renderer, BG_COLOR_RED, BG_COLOR_GREEN, BG_COLOR_BLUE, 0xFF);
   SDL_RenderFillRect(renderer, &bg_rect); 
}


SDL_Texture* draw_box_art(SDL_Renderer *renderer, platform_config *config, char *path) {
    
    SDL_Surface *box_art_surface = IMG_Load(path);

    // Failed to load the image, just place a placeholder image instead.
    // Currently just uses a black square.
    if (box_art_surface == NULL) {
        box_art_surface = SDL_CreateRGBSurface(0, 
            config->screen_width, 
            config->screen_height, 
            32,
            0x00FF0000,
            0x0000FF00,
            0x000000FF,
            0xFF000000
        );
   
        Uint32 p_color = SDL_MapRGB(box_art_surface->format, 0, 0, 0);   
        SDL_Rect placeholder_rect = {0, 0, config->screen_width, config->screen_height};

        SDL_FillRect(box_art_surface, &placeholder_rect, p_color);
        
   }
    
    uint32_t x0 = (config->screen_width *  5)  / 8;

    uint32_t height = box_art_surface->h;
    uint32_t width = box_art_surface->w;
    uint32_t max_width = config->screen_width - x0 - (config->screen_width / 20);
    // config contains location of box art
    
    if (width > max_width)
    {
        height = height * (width / max_width);
        width = max_width;
    }

    uint32_t banner_height = config->screen_height / config->banner_height_percentage;
    uint32_t max_height = config->screen_height - (banner_height * 3);
    if (height > max_height)
    {
        height = max_height;
    }

    uint32_t y0 = (config->screen_height - height) / 2;

    SDL_Rect dest_rect = {x0, y0, width, height};

    SDL_Texture *out_texture = SDL_CreateTextureFromSurface(renderer, box_art_surface);

    SDL_RenderCopy(renderer, out_texture, NULL, &dest_rect);

    SDL_FreeSurface(box_art_surface);
    return out_texture;
}


void  draw_top_banner(SDL_Renderer *renderer, platform_config *config, char* title) {
    
   Uint32 banner_height = config->screen_height / config->banner_height_percentage;
    
   SDL_Rect bg_rect = {0, 0, config->screen_width, banner_height};

   SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
   SDL_RenderFillRect(renderer, &bg_rect); 

}


void draw_bottom_banner(SDL_Renderer *renderer, platform_config *config) {
   
    Uint32 banner_width = config->screen_width / config->banner_width_percentage;
    Uint32 banner_height = config->screen_height / config->banner_height_percentage;
 
    SDL_Rect bg_rect = {0, config->screen_height - banner_height, config->screen_width, banner_height};
    
   SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
   SDL_RenderFillRect(renderer, &bg_rect); 
}



void free_dir_fname_textures(dir_fname_textures_t *textures)
{
    if (textures != NULL)
    {
        for (uint32_t i = 0; i < textures->count; i++)
        {
            SDL_DestroyTexture(textures->textures[i]);
        }

        free(textures->textures);
    }

    free(textures);
}


dir_fname_textures_t *get_directory_file_text_textures(SDL_Renderer *renderer, platform_config *config, dir_t *dir)
{
    if (dir == NULL)
    {
        log_message(LOG_ERROR, "NULL dir\n");
        return NULL;
    }

    uint32_t font_size = 24; //TODO dynamically work out via config
    #if defined(PSVITA)
        TTF_Font* font = TTF_OpenFont("app0:8_Bit_Madness.ttf", font_size);
    #elif defined(__SWITCH__)
        TTF_Font* font = TTF_OpenFont("romfs:/8_Bit_Madness.ttf", font_size);
    #elif defined(TARGET_OS_IPHONE)
        TTF_Font* font = TTF_OpenFont("8_Bit_Madness.ttf", font_size);
    #elif defined(__ANDROID__)
        // In Android the font is stored in the APK Assets which can't be read
        // from an absolute path. Instead we need to read it into memory and
        // initialize the font that way.
        int font_data_size = 0;
        log_message(LOG_INFO, "loading asset\n");
        uint8_t *font_data = load_asset("8_Bit_Madness.ttf", &font_data_size);
        log_message(LOG_INFO, "font_data %p, size %d\n", font_data, font_data_size);
        TTF_Font *font = NULL;
        if (font_data)
        {
            SDL_RWops *font_rwops = SDL_RWFromMem(font_data, font_data_size);
            font = TTF_OpenFontRW(font_rwops, 1, font_size);
            log_message(LOG_INFO, "font %p\n", font);
            ///free(font_data);
        }
    #else
        TTF_Font* font = TTF_OpenFont("8_Bit_Madness.ttf", font_size);
    #endif
    if (font == NULL)
    {
        log_message(LOG_ERROR, "Failed to load font\n");
        return NULL;
    }

    dir_fname_textures_t *fname_textures = calloc(1, sizeof(dir_fname_textures_t));
    if (fname_textures == NULL)
    {
        log_message(LOG_ERROR, "Failed to allocate fname textures\n");
        return NULL;
    }

    fname_textures->textures = calloc(dir->entry_count, sizeof(SDL_Texture*));
    if (fname_textures->textures == NULL) 
    {
        log_message(LOG_ERROR, "Failed to allocate texture storage\n");
        goto err;
    }

    SDL_Color text_color = {.r=0xFF, .g=0xFF, .b=0xFF, .a=0xff};
    SDL_Color dir_text_color  = {.r=DIR_COLOR_RED, .g=DIR_COLOR_BLUE, .b=DIR_COLOR_GREEN, .a=0xFF};;

    for (uint32_t i = 0; i < dir->entry_count; i++)
    {
       SDL_Color color = dir->entries[i].is_dir ? dir_text_color : text_color;
       SDL_Surface *text_surface = TTF_RenderText_Solid(font, dir->entries[i].name, color); 
       log_message(LOG_INFO, "text surface %p, %s\n", text_surface, dir->entries[i].name);
       fname_textures->textures[i] = SDL_CreateTextureFromSurface(renderer, text_surface);
       SDL_FreeSurface(text_surface);
       if (fname_textures->textures[i] == NULL)
       {
           log_message(LOG_ERROR, "Failed to get file name texture from surface\n");
           goto err;
       }
       fname_textures->count++;
    }
	
	TTF_CloseFont(font);
	
    return fname_textures;

    err:
		TTF_CloseFont(font);
        free_dir_fname_textures(fname_textures);
        return NULL;
}


void redraw_filenames(context_t *context)
{
    platform_config *config = context->config;
    SDL_Renderer *renderer = context->renderer;

    // Fill over Text Area 
    uint32_t width = (config->screen_width * 
                config->screen_text_area_width_ratio_top) /
                config->screen_text_area_width_ratio_bottom;
    
    uint32_t banner_height = config->screen_height / config->banner_height_percentage;

    uint32_t x0 = 0;
    uint32_t y0 = banner_height;

    uint32_t height = config->screen_height - (banner_height * 2); 

    // Draw the text to be displayed
    log_message(LOG_INFO, "fnames: %d\n", context->fname_textures->count);
    for (uint32_t i = context->scroll_offset; i < context->fname_textures->count; i++)
    {
        int width = 0;
        int height = 0;
        
        SDL_QueryTexture(context->fname_textures->textures[i], NULL, NULL, &width, &height);
        uint32_t start_tx = config->text_x_start;
        uint32_t start_ty = config->text_y_start + (config->text_y_spacing * (i - context->scroll_offset));
        
        if (start_ty + height >= config->screen_height - banner_height)
        {
            break;
        }

        SDL_Rect rect = {start_tx, start_ty, width, height};
        SDL_RenderCopy(renderer, context->fname_textures->textures[i], NULL, &rect);
    }
}

void redraw_screen(context_t *context)
{
    SDL_Renderer *renderer = context->renderer;
    platform_config *config = context->config;
     
    draw_select_game_bg(renderer, config);
    draw_top_banner(renderer, config, "Select Game");
    draw_bottom_banner(renderer, config);
       
    // Hightlight new 
    uint32_t text_width = (config->screen_width * 
                config->screen_text_area_width_ratio_top) /
                config->screen_text_area_width_ratio_bottom;
    
    SDL_Rect highlight = {config->text_x_start, config->text_y_start + ((context->current_highlighted - context->scroll_offset) * config->text_y_spacing), text_width - config->text_x_start, config->text_y_spacing};
    SDL_SetRenderDrawColor(renderer, 55, 55, 55, 255);
    SDL_RenderFillRect(renderer, &highlight); 
      
    redraw_filenames(context);
        

    // Get Boxart of new selected file
    dir_t *dir = context->dir;
    char *file_path;
    char *name = dir->entries[context->current_highlighted].name; 
    char *path = dir->path;

    char *result = calloc(1, strlen(path) + 1 + strlen(name) + 1);
    SDL_Texture *box_art_texture = NULL;
    if (result != NULL)
    {
        strcpy(result, path); 
        strcpy(result + strlen(path), "/");
        strcpy(result + strlen(path)  + 1, name);
        
        box_art_texture = draw_box_art(renderer, config, result);
    }

#if defined(__ANDROID__) || (defined(__APPLE__) && TARGET_OS_IPHONE)  
    SDL_RenderCopy(renderer, overlay_t, NULL, NULL);
#endif
    
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(box_art_texture);
}


void navigation(context_t *context, navigate_option_t navigate_option)
{
    int redraw = 0;
    platform_config *config = context->config;

    if (navigate_option == NAV_SELECT)
    {
        // Check what was selected
        dir_t *dir = context->dir;
        dir_entry_t selected_entry = dir->entries[context->current_highlighted];
           
        // File selected, set the field and return to return the path
        if (!selected_entry.is_dir)
        {
            log_message(LOG_INFO, "is file\n"); 
            char *path = calloc(1, strlen(dir->path) + 1 + strlen(selected_entry.name) + 1);
            strcpy(path, dir->path);
            path[strlen(dir->path)] = '/';
            strcpy(path + strlen(dir->path) + 1, selected_entry.name);
            context->selected_rom_path = path;
            return;
        }
        else
        {
            log_message(LOG_INFO, "is dir\n");
            char *new_path = calloc(1, strlen(dir->path) + 1 + strlen(selected_entry.name) + 1);
            if (new_path == NULL)
            {   
                log_message(LOG_INFO, "Failed to allocate new dir name\n");
                goto navigate_fail;
            }

				
			#if !defined(__ANDROID__)
            strcpy(new_path, dir->path); 
            new_path[strlen(dir->path)] = '/';
            strcpy(new_path + strlen(dir->path)  + 1, selected_entry.name);
		    #else
            strcpy(new_path, selected_entry.name);
			#endif       	
            
			log_message(LOG_INFO, "changing dir:\n");
            log_message(LOG_INFO, "%s\n", new_path);

            dir_t *new_dir = get_dir(new_path);
            if (new_dir == NULL)
            {
                goto navigate_fail;
            }

            dir_fname_textures_t *fname_textures = context->fname_textures;

            dir_fname_textures_t *new_fname_textures = get_directory_file_text_textures(context->renderer, context->config, new_dir);

            if (new_fname_textures == NULL)
            {
                free_dir(new_dir);
                goto navigate_fail;
            }
            context->fname_textures = new_fname_textures;
            context->dir = new_dir;
            context->current_highlighted = 0;
            context->scroll_offset = 0;

            redraw_screen(context);

            // Don't need the old dir anymore
            free_dir_fname_textures(fname_textures);
            free_dir(dir);
        
            //SDL_RenderPresent(renderer);
            return;
        }

        navigate_fail:
            return;
    }

    if (navigate_option == NAV_DOWN || navigate_option == NAV_UP)
    {
            
        uint32_t previous_highlighted = context->current_highlighted;

        if (navigate_option == NAV_DOWN)
        {
            log_message(LOG_INFO, "NAV_DOWN");
            // Already at last entry
            if (context->current_highlighted + 1 >= context->dir->entry_count)
            {
                return;
            }

            context->current_highlighted += 1;
            redraw = 1;
        
            Uint32 banner_height = config->screen_height / config->banner_height_percentage;
            if (config->text_y_start + ((context->current_highlighted - context->scroll_offset) * (config->text_y_spacing + 1))  > config->screen_height - banner_height)
             {
                 context->scroll_offset += 1;
             }
        }

        if (navigate_option == NAV_UP)
        {
            log_message(LOG_INFO, "NAV_UP");
            // Already at first entry
            if (context->current_highlighted == 0)
            {
                return;
            }
            
            context->current_highlighted -= 1;
            redraw = 1;   
            // Check if we need to scroll the text
            if (context->current_highlighted < context->scroll_offset)
            {
                context->scroll_offset -= 1;
            }
        }
 

        //SDL_SetRenderTarget(context->renderer, context->screen_texture);
        SDL_Renderer *renderer = context->renderer;

        if (redraw)
        {
            log_message(LOG_INFO, "redraw_screen\n");
            redraw_screen(context);

        }
    }
}

// If down and at bottom, then move list +1
// If up and at top, then move list -1

/* --------------------- */

int ui_menu(char **selected_path, int *ret_val) {
    
    log_message(LOG_INFO, "Starting menu\n");
 
    #if defined(PSVITA)
    platform_config config = {	.screen_width = 960, 
								.screen_height = 544, 
								.banner_width_percentage = 10, 
								.banner_height_percentage = 10,
                                .text_x_start = 75,
                                .text_y_start = 100,
                                .text_y_spacing=20,
                                .screen_text_area_width_ratio_top = 5,
                                .screen_text_area_width_ratio_bottom = 8
                             };
    #elif !defined(__ANDROID__) && !defined(TARGET_OS_IPHONE)
    platform_config config = {	//.screen_width = 1280, 
								//.screen_height = 720,
                                .screen_width = 1280,
                                .screen_height = 720, 
								.banner_width_percentage = 10, 
								.banner_height_percentage = 10,
                                .text_x_start = 75,
                                .text_y_start = 100,
                                .text_y_spacing=20,
                                .screen_text_area_width_ratio_top = 5,
                                .screen_text_area_width_ratio_bottom = 8
                             };
    #endif
    
    if((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)==-1)) {
        log_message(LOG_ERROR, "Failed to init SDL\n");
        return -1;
    }
    
    if (TTF_Init() == -1)
    {
        log_message(LOG_ERROR, "Could not initialize SDL2_ttf: %s\n", TTF_GetError());
        return -1;
    }

    log_message(LOG_INFO, "Initialized SDL\n");
    SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
    SDL_GetCurrentDisplayMode(0, &current);
    if (current.h > current.w)
    {
        uint32_t tmp = current.h;
        current.h = current.w;
        current.w = tmp;
    }
    //current.w *= 3;
    //current.h *= 3;
    
    #if defined(TARGET_OS_IPHONE)
        uint32_t x0, y0, w, h;
        get_dimensions(&x0, &y0, &w, &h);

        current.w = w;
        current.h = h;
     
    #endif

    log_message(LOG_INFO, "width: %d height: %d\n", current.w, current.h);

    #if defined(__ANDROID__)
    platform_config config = {	//.screen_width = 1280, 
								//.screen_height = 720,
                                .screen_width = current.w,
                                .screen_height = current.h, 
								.banner_width_percentage = 10, 
								.banner_height_percentage = 10,
                                .text_x_start = 75,
                                .text_y_start = 100,
                                .text_y_spacing=20,
                                .screen_text_area_width_ratio_top = 5,
                                .screen_text_area_width_ratio_bottom = 8
                             };
    #elif defined(TARGET_OS_IPHONE)
    platform_config config = {    //.screen_width = 1280,
                                  //.screen_height = 720,
                                .screen_width = current.w,
                                .screen_height = current.h,
                                .banner_width_percentage = 10,
                                .banner_height_percentage = 10,
                                .text_x_start = 75,
                                .text_y_start = 100,
                                .text_y_spacing=20,
                                .screen_text_area_width_ratio_top = 5,
                                .screen_text_area_width_ratio_bottom = 8
    };
    #endif
    
    //#if defined(TARGET_OS_IPHONE)
    screen = SDL_CreateWindow("Plutoboy",
                     // SDL_WINDOWPOS_UNDEFINED,
                     // SDL_WINDOWPOS_UNDEFINED,
                    300,
                              300,
                      config.screen_width, config.screen_height,
                      SDL_WINDOW_ALLOW_HIGHDPI);
    
    //SDL_SetWindowSize(screen, config.screen_width - 500, config.screen_height - 500);
    
    // A bit annoying but the safeAreaLayoutGuide only has its values set
    // after SDL_CreateWindow is called, which is kind of useless to us.
    //
    // To hack around this we take the dimensions from the already iinitialized window
    // close it, and then open another one adjusted for the safeAreaLayout
    //
    // This ensures part of the screen isn't obscured by the notch, curved borders of
    // the iPhone X and above series of devices.
    /*#if defined(TARGET_OS_IPHONE)
        get_dimensions(&x0, &y0, &w, &h);
        config.screen_width = w;
        config.screen_height = h;
        SDL_DestroyWindow(screen);
        screen = SDL_CreateWindow("Plutoboy",
                   x0,
                   y0 ,
                   config.screen_width, config.screen_height,
                SDL_WINDOW_ALLOW_HIGHDPI);
    #endif*/
    

    SDL_Renderer *renderer = SDL_CreateRenderer(screen, -1, 0);

    SDL_Texture *screen_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, config.screen_width, config.screen_height); 


    /* Select the color for drawing. It is set to red here. */
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    /* Clear the entire screen to our selected color. */
    SDL_RenderClear(renderer);

    log_message(LOG_INFO, "created screen texture\n");

    log_message(LOG_INFO, "set renderer\n");

    draw_select_game_bg(renderer, &config);
    
    log_message(LOG_INFO, "bg_texture copied\n");
    
    draw_top_banner(renderer, &config, "Select Game");
    draw_bottom_banner(renderer, &config);

    log_message(LOG_INFO, "about to draw boxart\n");
#if defined(PSVITA)
    dir_t *dir = get_dir("ux0:data/Plutoboy");
#elif defined(__SWITCH__)
    dir_t *dir = get_dir("/switch/Plutoboy");
    if (dir == NULL) {
        char cwd_buf[PATH_MAX+1];
        if (getcwd(cwd_buf, PATH_MAX) != 0) {
            log_message(LOG_INFO, "buf: %s\n", cwd_buf);
            dir = get_dir(cwd_buf);
        }
        else {
            log_message(LOG_INFO, "Failed to get cwd\n");
        }
    }
#elif defined(__ANDROID__)
    dir_t *dir = get_dir("");
#elif defined(TARGET_OS_IPHONE)
    dir_t *dir = get_dir(".");
#else    
    dir_t *dir = get_dir(".");
#endif
    if (dir == NULL) {
        dir = get_dir(".");
    }

    if (dir == NULL)
    {
        log_message(LOG_ERROR, "Failed to get current dir\n");
        return -1;
    }

    dir_fname_textures_t *fname_textures =  get_directory_file_text_textures(renderer, &config, dir); 

    if (fname_textures == NULL)
    {
		log_message(LOG_ERROR, "Failed to get directiory file textures\n");
        return -1;
    }
	
    Uint32 banner_height = config.screen_height / config.banner_height_percentage;
    
    log_message(LOG_INFO, "fnames: %d\n", fname_textures->count);
    for (uint32_t i = 0; i < fname_textures->count; i++)
    {
        int width = 0;
        int height = 0;
        
        SDL_QueryTexture(fname_textures->textures[i], NULL, NULL, &width, &height);
        uint32_t start_tx = config.text_x_start;
        uint32_t start_ty = config.text_y_start + (config.text_y_spacing * i);
        
        if (start_ty + height >= config.screen_height - banner_height)
        {
            break;
        }

        SDL_Rect rect = {start_tx, start_ty, width, height};
        SDL_RenderCopy(renderer, fname_textures->textures[i], NULL, &rect);
    }
    
    SDL_Rect render_rect = {0, 0, config.screen_width, config.screen_height};

    context_t context;
    context.config = &config;
    context.fname_textures = fname_textures;
    context.dir = dir;
    context.renderer = renderer;
    context.current_highlighted = 0;
    context.screen_texture = screen_texture;
    context.scroll_offset = 0;
    context.selected_rom_path = 0;

#if defined(__SWITCH__)    
	for (int i = 0; i < 2; i++) {
        if (SDL_JoystickOpen(i) == NULL) {
            SDL_Log("SDL_JoystickOpen: %s\n", SDL_GetError());
            SDL_Quit();
            return -1;
        }
    }
#endif

#if defined(PSVITA)
    if (SDL_NumJoysticks() > 0) {
        SDL_JoystickOpen(0);
    }
#endif

    log_message(LOG_INFO, "Got this far\n");

#if defined(__ANDROID__) || defined(TARGET_OS_IPHONE)
    init_vbutton_texture(renderer);
#endif

#if defined(__ANDROID__) || defined(TARGET_OS_IPHONE)
    SDL_RenderCopy(renderer, overlay_t, NULL, NULL);
#endif
    
    SDL_RenderPresent(renderer);

    int quit = 0;
    SDL_Event e;
    while (!quit && context.selected_rom_path == NULL)
    {
        while(SDL_PollEvent(&e)) {
            switch (e.type)
            {
                case SDL_QUIT:
                    quit = 1;
                    break;
   			#if !defined(__SWITCH__) && !defined(PSVITA) 
	             case SDL_KEYDOWN:
                    switch (e.key.keysym.sym)
                    {
                        case SDLK_UP:
                            navigation(&context, NAV_UP);
                            break;
                        case SDLK_DOWN:
                            navigation(&context, NAV_DOWN);
                            break;
                        case SDLK_RETURN:
                            navigation(&context, NAV_SELECT);
                            break;
                    }
			#endif
			#if defined(__SWITCH__)
                case SDL_JOYBUTTONDOWN:
					switch (e.jbutton.button)
					{
						case JOY_A:
							navigation(&context, NAV_SELECT);
							break;
						case JOY_UP:
							navigation(&context, NAV_UP);
							break;
						case JOY_DOWN:
							navigation(&context, NAV_DOWN);
							break;
     				}

			#endif
            #if defined(PSVITA)
                case SDL_JOYBUTTONDOWN:
                    log_message(LOG_INFO, "vita pressed\n");
                    switch(e.jbutton.button)
                    {
                        case SDLK_VITA_CROSS:
                            navigation(&context, NAV_SELECT);
                            break;
                        case SDLK_VITA_UP:
                            log_message(LOG_INFO, "up");
                            navigation(&context, NAV_UP);
                            break;
                        case SDLK_VITA_DOWN:
                            log_message(LOG_INFO, "down");
                            navigation(&context, NAV_DOWN);
                            break;
                    }
            #endif
            #ifndef PSVITA          
                case SDL_FINGERDOWN:
                    log_message(LOG_INFO, "Finger down\n");
                    check_keys_pressed(&context, e.tfinger.x, e.tfinger.y, 1);
                    break;

                case SDL_FINGERUP:
                    log_message(LOG_INFO, "Finger up\n");
                    check_keys_pressed(&context, e.tfinger.x, e.tfinger.y, 0);
                    break;

                // Assume only one finger can be on a button at a time considering
                // the size of the buttons. It would be more accurate in the future
                // to keep track of finger ids for each button 
                case SDL_FINGERMOTION:
                    log_message(LOG_INFO, "Finger motion\n");
                    check_keys_moved(e.tfinger.x, e.tfinger.y,
                    e.tfinger.dx, e.tfinger.dy);
                    break;
#endif
            }
        }
    }

    int ret_code = 0;
    if (context.selected_rom_path != NULL) {
        *selected_path = context.selected_rom_path;
        *ret_val = 0;
    }
    else
    {
        *ret_val = -1;
    }


    free_dir_fname_textures(context.fname_textures);
    free_dir(context.dir);
    SDL_DestroyTexture(context.screen_texture);
    SDL_DestroyRenderer(context.renderer);
    #ifndef PSVITA
    SDL_DestroyWindow(screen);
    #endif
    IMG_Quit();    
    TTF_Quit();

    // Switch REALLY does not like quitting SDL2 and then re-opening :/
    #if !defined(__SWITCH__)
    //SDL_Quit();
    #endif

    return *ret_val;
}

