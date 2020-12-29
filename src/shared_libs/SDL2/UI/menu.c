#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "../../../non_core/menu.h"
#include "../../../non_core/filebrowser.h"
#include "../../../non_core/logger.h"

#include <unistd.h>

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

// Another moan at how garbage VITASDK's SDL2 library is.
// Loading Textures through SDL2_Image's IMG_LoadTexture and attempting
// to RenderCopy them crashes the whole console!! Sometimes even
// requiring going through the recovery menu, wtf?

// A hack around this bs is we just use vita2d to load our images
// and then read them directly into a SDL2 texture. 

// Seriously every other implementation works fine 
// why is the vita one so shit!?

#endif

typedef enum {NAV_DOWN, NAV_UP, NAV_LEFT, NAV_RIGHT, NAV_SELECT, NAV_BACK} navigate_option_t;

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


#define BG_COLOR_RED 77
#define BG_COLOR_GREEN 77
#define BG_COLOR_BLUE 77

#define DIR_COLOR_RED 43
#define DIR_COLOR_GREEN 217
#define DIR_COLOR_BLUE 255

SDL_Window *screen;


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


void draw_box_art(SDL_Renderer *renderer, platform_config *config, char *path) {
    
    //SDL_Surface* img = IMG_Load(path);
  
    SDL_Surface *box_art_surface = IMG_Load(path);

    // IMG_LoadTexture on the Vita doesn't play nice, crashes the GPU.
    // We need to load the image to a surface and then convert to SDL_PIXELFORMAT_ARGB8888
    // and finally directly copy the pixels onto the texture.
    #ifdef PSVITA
    if (box_art_surface)
    {
       SDL_Surface *tmp = SDL_ConvertSurfaceFormat(box_art_surface, SDL_PIXELFORMAT_ARGB8888, 0);
       SDL_FreeSurface(box_art_surface);
       box_art_surface = tmp; 
    }
    #endif

    log_message(LOG_INFO, "%s\n", path);
    log_message(LOG_INFO, "%p\n", box_art_surface); 
    // Failed to load the image, just place a placeholder image instead.
    // Currently just uses a black square.
    if (box_art_surface == NULL) {
        log_message(LOG_INFO, "Failed to load image");
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


    #ifdef PSVITA
    SDL_Texture *out_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                    SDL_TEXTUREACCESS_STREAMING, box_art_surface->w, box_art_surface->h);
    SDL_UpdateTexture(out_texture, NULL, box_art_surface->pixels, box_art_surface->pitch);
    #else
    SDL_Texture *out_texture = SDL_CreateTextureFromSurface(renderer, box_art_surface);
    #endif

    SDL_RenderCopy(renderer, out_texture, NULL, &dest_rect);

    SDL_FreeSurface(box_art_surface);
    SDL_DestroyTexture(out_texture);

    log_message(LOG_INFO, "Done");
}


void  draw_top_banner(SDL_Renderer *renderer, platform_config *config, char* title) {
     
   /*   
   SDL_Surface *banner = SDL_CreateRGBSurface(0, 
        config->screen_width, 
        config->screen_height, 
        32,
        0x00FF0000,
        0x0000FF00,
        0x000000FF,
        0xFF000000
    );
 */
    Uint32 banner_height = config->screen_height / config->banner_height_percentage;
    
  //  Uint32 b_color = SDL_MapRGB(banner->format, 0, 0, 0);
    SDL_Rect bg_rect = {0, 0, config->screen_width, banner_height};

   SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
   SDL_RenderFillRect(renderer, &bg_rect); 

    /*
    SDL_FillRect(banner, &bg_rect, b_color);
   
    SDL_SetSurfaceBlendMode(banner, SDL_BLENDMODE_BLEND);
    SDL_Texture *banner_texture = SDL_CreateTextureFromSurface(renderer, banner);
	SDL_FreeSurface(banner);
    */
    //return banner_texture;
}


void draw_bottom_banner(SDL_Renderer *renderer, platform_config *config) {
   
    Uint32 banner_width = config->screen_width / config->banner_width_percentage;
    Uint32 banner_height = config->screen_height / config->banner_height_percentage;
 
    //Uint32 b_color = SDL_MapRGB(banner->format, 0, 0, 0);
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
        log_message(LOG_ERROR, "Accessing NULL directory\n"); 
        return NULL;
    }

    uint32_t font_size = 24; //TODO dynamically work out via config
    #if defined(PSVITA)
        TTF_Font* font = TTF_OpenFont("app0:8_Bit_Madness.ttf", font_size);
    #elif defined(__SWITCH__)
        TTF_Font* font = TTF_OpenFont("romfs:/8_Bit_Madness.ttf", font_size);
    #else
        TTF_Font* font = TTF_OpenFont("8_Bit_Madness.ttf", font_size);
    #endif
    if (font == NULL)
    {
        printf("Failed to load font\n");
        return NULL;
    }

    dir_fname_textures_t *fname_textures = calloc(1, sizeof(dir_fname_textures_t));
    if (fname_textures == NULL)
    {
        return NULL;
    }

    fname_textures->textures = calloc(dir->entry_count, sizeof(SDL_Texture*));
    if (fname_textures->textures == NULL) 
    {
        goto err;
    }

    SDL_Color text_color = {.r=0xFF, .g=0xFF, .b=0xFF, .a=0xff};
    SDL_Color dir_text_color  = {.r=DIR_COLOR_RED, .g=DIR_COLOR_BLUE, .b=DIR_COLOR_GREEN, .a=0xFF};;

    for (uint32_t i = 0; i < dir->entry_count; i++)
    {
       SDL_Color color = dir->entries[i].is_dir ? dir_text_color : text_color;
       SDL_Surface *text_surface = TTF_RenderText_Solid(font, dir->entries[i].name, color); 
       fname_textures->textures[i] = SDL_CreateTextureFromSurface(renderer, text_surface);
       SDL_FreeSurface(text_surface);
       if (fname_textures->textures[i] == NULL)
       {
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
    char *path = dir->path; //"/Users/ross/Dropbox/Projects/Plutoboy_UI/boxart\0";

    char *result = calloc(1, strlen(path) + 1 + strlen(name) + 1);
    if (result != NULL)
    {
        strcpy(result, path); 
        strcpy(result + strlen(path), "/");
        strcpy(result + strlen(path)  + 1, name);
        printf("%s\n", result);
        
        draw_box_art(renderer, config, result);
    }
    
    SDL_RenderPresent(renderer);
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
            char *path = calloc(1, strlen(dir->path) + 1 + strlen(selected_entry.name) + 1);
            strcpy(path, dir->path);
            path[strlen(dir->path)] = '/';
            strcpy(path + strlen(dir->path) + 1, selected_entry.name);
            context->selected_rom_path = path;
            return;
        }
        else
        {
            char *new_path = calloc(1, strlen(dir->path) + 1 + strlen(selected_entry.name) + 1);
            if (new_path == NULL)
            {
                goto navigate_fail;
            }

            strcpy(new_path, dir->path); 
            new_path[strlen(dir->path)] = '/';
            strcpy(new_path + strlen(dir->path)  + 1, selected_entry.name);
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
        /*
        else
        {
            platform_config *config = context->config;
            SDL_Renderer *renderer = context->renderer;
            dir_fname_textures_t *fname_textures = context->fname_textures;
            int width = 0;
            int height = 0;
            uint32_t start_x = 0;
            uint32_t start_y = 0;
            uint32_t text_width = (config->screen_width * 
                config->screen_text_area_width_ratio_top) /
                config->screen_text_area_width_ratio_bottom;

            // Unhighlight old, and highlight new
            SDL_Rect old_highlight = {config->text_x_start, config->text_y_start + (config->text_y_spacing * (previous_highlighted - context->scroll_offset)) , text_width - config->text_x_start, config->text_y_spacing};
        
            SDL_SetRenderDrawColor(renderer, BG_COLOR_RED, BG_COLOR_GREEN, BG_COLOR_BLUE, 255);
            SDL_RenderFillRect(renderer, &old_highlight); 
      
            // Redraw old text
            SDL_QueryTexture(fname_textures->textures[previous_highlighted], NULL, NULL, &width, &height);
            start_x = config->text_x_start;
            start_y = config->text_y_start + (config->text_y_spacing * (previous_highlighted -context->scroll_offset));
            SDL_Rect rect = {start_x, start_y, width, height};
            SDL_RenderCopy(renderer, fname_textures->textures[previous_highlighted], NULL, &rect);


            // Hightlight new 
            SDL_Rect highlight = {config->text_x_start, config->text_y_start + ((context->current_highlighted - context->scroll_offset) * config->text_y_spacing), text_width - config->text_x_start, config->text_y_spacing};
            SDL_SetRenderDrawColor(renderer, 55, 55, 55, 255);
            SDL_RenderFillRect(renderer, &highlight); 

   
            // Redraw next text
            SDL_QueryTexture(fname_textures->textures[context->current_highlighted], NULL, NULL, &width, &height);
            start_x = config->text_x_start;
            start_y = config->text_y_start + (config->text_y_spacing * (context->current_highlighted - context->scroll_offset));
            SDL_Rect new_text_rect = {start_x, start_y, width, height};
            SDL_RenderCopy(renderer, fname_textures->textures[context->current_highlighted], NULL, &new_text_rect);
    
            //SDL_RenderPresent(renderer);
        }

        //SDL_SetRenderTarget(renderer, NULL);
        SDL_Rect render_rect = {0, 0, config->screen_width, config->screen_height};
        SDL_RenderCopy(renderer, context->screen_texture, NULL, &render_rect);
    
        // Get Boxart of new selected file
        dir_t *dir = context->dir;
        char *file_path;
        char *name = dir->entries[context->current_highlighted].name; 
        char *path = dir->path; //"/Users/ross/Dropbox/Projects/Plutoboy_UI/boxart\0";
        //path = dir->path;

         char *result = calloc(1, strlen(path) + 1 + strlen(name) + 1);
         if (result != NULL)
         {
                strcpy(result, path); 
                strcpy(result + strlen(path), "/");
                strcpy(result + strlen(path)  + 1, name);
                printf("%s\n", result);
        
                draw_box_art(renderer, config, result);
        }

        free(result);
        //SDL_RenderPresent(renderer);
    */
    }
}

// If down and at bottom, then move list +1
// If up and at top, then move list -1

/* --------------------- */

int ui_menu(char **selected_path, int *ret_val) {
    
    log_message(LOG_INFO, "Starting menu\n");
 
    #if defined(PSVITA)
    // Lol crash if dimensions are Vita ones?:S
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
    #else
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
    
    //if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) & IMG_INIT_PNG & IMG_INIT_JPG)) {
    //    log_message(LOG_ERROR, "Could not initialize SDL2_image: %s\n", IMG_GetError());
    //    return -1;
    // }
    
    if (TTF_Init() == -1)
    {
        log_message(LOG_ERROR, "Could not initialize SDL2_ttf: %s\n", TTF_GetError());
        return -1;
    }

    log_message(LOG_INFO, "Initialized SDL\n");

    screen = SDL_CreateWindow("Plutoboy", 
                   SDL_WINDOWPOS_UNDEFINED, 
                   SDL_WINDOWPOS_UNDEFINED,
                   config.screen_width, config.screen_height,
                   0);

    SDL_Renderer *renderer = SDL_CreateRenderer(screen, -1, 0);

    SDL_Texture *screen_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, config.screen_width, config.screen_height); 


    /* Select the color for drawing. It is set to red here. */
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    /* Clear the entire screen to our selected color. */
    SDL_RenderClear(renderer);

    log_message(LOG_INFO, "created screen texture\n");

    //SDL_SetRenderTarget(renderer, screen_texture);

    log_message(LOG_INFO, "set renderer\n");

    draw_select_game_bg(renderer, &config);
    
    log_message(LOG_INFO, "bg_texture copied\n");
    /*
    SDL_Texture *banner_texture = draw_top_banner(renderer, &config, "Select Game");
    SDL_RenderCopy(renderer, banner_texture, NULL, NULL);
    SDL_DestroyTexture(banner_texture);
    
    log_message(LOG_INFO, "top banned texture copied\n");

    SDL_Texture *bottom_banner_texture = draw_bottom_banner(renderer, &config);
    SDL_RenderCopy(renderer, bottom_banner_texture, NULL, NULL);
    SDL_DestroyTexture(bottom_banner_texture);
    */
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
#else    
    dir_t *dir = get_dir(".");
#endif
    if (dir == NULL) {
        dir = get_dir(".");
    }

    dir_fname_textures_t *fname_textures =  get_directory_file_text_textures(renderer, &config, dir); 

    if (fname_textures == NULL)
    {
		log_message(LOG_ERROR, "Failed to get directiory file textures\n");
        return -1;
    }
	
    Uint32 banner_height = config.screen_height / config.banner_height_percentage;
    
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
    
    // Works on Vita
    //SDL_Rect highlight = {config.text_x_start, config.text_y_start, 200, config.text_y_spacing};
    //SDL_SetRenderDrawColor(renderer, 55, 55, 55, 255);
    //SDL_RenderFillRect(renderer, &highlight); 

    //SDL_RenderPresent(renderer);

    //SDL_SetRenderTarget(renderer, NULL);
    SDL_Rect render_rect = {0, 0, config.screen_width, config.screen_height};
    //SDL_RenderCopy(renderer, screen_texture, NULL, &render_rect);
    
    SDL_RenderPresent(renderer);

    context_t context;
    context.config = &config;
    context.fname_textures = fname_textures;
    context.dir = dir;
    context.renderer = renderer;
    context.current_highlighted = 0;
    context.screen_texture = screen_texture;
    context.scroll_offset = 0;
    context.selected_rom_path = 0;

	    // open CONTROLLER_PLAYER_1 and CONTROLLER_PLAYER_2
    // when railed, both joycons are mapped to joystick #0,
    // else joycons are individually mapped to joystick #0, joystick #1, ...
    // https://github.com/devkitPro/SDL/blob/switch-sdl2/src/joystick/switch/SDL_sysjoystick.c#L4
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

