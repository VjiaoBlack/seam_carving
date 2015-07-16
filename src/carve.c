#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string.h>

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gPNGSurface = NULL;

int main( int argc, char* args[] ) {

    if (argc != 4) {
        printf("Format: ./carve <img> <rows_cut> <cols_cut>\n");
        exit(1);
    }

    int rows_cut, cols_cut, rows, cols;

    sscanf(args[2], "%d", &rows_cut);
    sscanf(args[3], "%d", &cols_cut);

    printf("rows to cut: %d, cols to cut: %d\n", rows_cut, cols_cut);

    SDL_Init( SDL_INIT_VIDEO );
    IMG_Init( IMG_INIT_PNG );

    SDL_Surface* optimizedSurface = NULL;
    SDL_Surface* loadedSurface = IMG_Load( args[1] );

    rows = loadedSurface->h;
    cols = loadedSurface->w;
    gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, cols, rows, SDL_WINDOW_SHOWN );
    gScreenSurface = SDL_GetWindowSurface( gWindow );

    optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, NULL );
    SDL_FreeSurface( loadedSurface );
    gPNGSurface = optimizedSurface;


    // set up energy matrix
    // use as much RAM as you want - assume that you have enough
    char** energy = malloc(sizeof(char*) * rows);
    int i, j;

    Uint8 ro, go, bo, r, g, b;


    for (i = 0; i < rows; i++) {
        energy[i] = malloc(sizeof(char) * rows);
        for (j = 0; j < cols; j++) {
            SDL_GetRGB( * (Uint32*)(gPNGSurface->pixels + j * sizeof(Uint32) + i * cols * sizeof(Uint32)),
                gScreenSurface->format, &ro, &go, &bo);
                *((Uint32*)gPNGSurface->pixels + j + i * cols) = SDL_MapRGB(
                    gScreenSurface->format, ro* ((double)i / rows), go* ((double)i / rows), bo * ((double)i / rows));
        }
    }







    // analyze energy matrix





    int quit = 0;
    SDL_Event e;

    while( !quit ) {
        while( SDL_PollEvent( &e ) != 0 ) {
            if( e.type == SDL_QUIT ) {
                quit = 1;
            }
        }

        SDL_BlitSurface( gPNGSurface, NULL, gScreenSurface, NULL );

        SDL_UpdateWindowSurface( gWindow );
        SDL_Delay(100);
    }

    SDL_FreeSurface( gPNGSurface );
    gPNGSurface = NULL;

    SDL_DestroyWindow( gWindow );
    gWindow = NULL;

    IMG_Quit();
    SDL_Quit();

    return 0;
}
