#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include <signal.h> // ** INSTANT RELOAD INTEGRATION ** - Ability to check for SIGUSR1 (console closed)


#include "funkey/sdl-menu.h"

// Global Variable
int should_quick_save = 0;

// ** INSTANT RELOAD INTEGRATION **
void handle_sigusr1(int sig)
{
	// Stop the menu loop if running (this is a global variable from sdl-menu.h)
    // Otherwise we'll never process the bool below, which is in the application main loop
	stop_menu_loop = 1;

	/* Signal to quick save and poweroff after next loop */
	should_quick_save = 1;
}

int main(int argc, char *argv[])
{  
    //
    //      launch_resume_menu_loop()
    //          Added to emulator startup in emu_run() [emu.c] before we enter the main loop
    //          Called if quicksave file is detected, waits for result from menu
    //          Said quicksave file is created from quick_save_and_poweroff() [emu.c]

    // emu.c
    //      quick_save_and_poweroff()
    //      NOTE - Not part of the funkey 'library' but probably should be!
    //      Added to main loop, triggered from SIGUSR1 handler, callback registered in main() 


    int quit_main_loop = 0;
    SDL_Event event;

	/* Init USR1 Signal (for quick save and poweroff) */
	signal(SIGUSR1, handle_sigusr1);

    // Init SDL Video
    SDL_Init(SDL_INIT_VIDEO);

    // Open HW screen and set video mode 240x240, with double buffering 
    SDL_Surface* hw_surface = SDL_SetVideoMode(240, 240, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);

    // Hide the cursor, FunKey doesn't come with a mouse
    SDL_ShowCursor(0);

    // ** QUICK MENU INTEGRATION ** - Initialise the menu, loading ttf/image assets
    // Also pre-renders all non-dynamic elements of each menu page, trying to reduce dynamic rendering
    // Should be placed after SDL_Init, and also requires the main SDL_Surface to be accessible
    // TTF_Init() should probably move within init_menu_SDL(), wrapped in a TTF_WasInit() guard?
    TTF_Init();
    init_menu_SDL(hw_surface);

    //Main loop
    while(!quit_main_loop)
    {
        // Process event queue
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    quit_main_loop = 1;
                    exit(0);
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_q:
                        case SDLK_ESCAPE:

                            // ** QUICK MENU INTEGRATION ** - Start the menu update, effectively pausing the main loop
                            // Handles input events, scrolling anim, and rendering of dynamic elements if needed
                            // Hook this up to a press of the Q or ESC key in however the app processes inputs
                            run_menu_loop();
                            break;

                        default:
                            break; 
                    }
                default:
                    break; 
            }
        }

        // Draw a green square
        SDL_Rect draw_rect = {.x=70, .y=70, .w=100, .h=100};
        Uint32 color = SDL_MapRGB(hw_surface->format, 0, 255, 0);
        SDL_FillRect(hw_surface, &draw_rect, color);

        // Flip the screen buffer
        SDL_Flip(hw_surface);


        // ** INSTANT RELOAD INTEGRATION **
        if (should_quick_save)
        {
            //quick_save_and_poweroff();
        }
    }

    // ** QUICK MENU INTEGRATION ** - Standard shutdown, deallocating ttf/image assets
    // If we do move TTF_Init() into the menu init, cache off and shutdown that in here as well
    deinit_menu_SDL();

    SDL_Quit();
    return 0;
}
