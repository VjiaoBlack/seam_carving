#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string.h>

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gPNGSurface = NULL;

int find_energy( int r, int c, int rows, int cols, SDL_Surface* surface) {
    Uint8 rl = 0, gl = 0, bl = 0,
          rr = 0, gr = 0, br = 0,
          rt = 0, gt = 0, bt = 0,
          rb = 0, gb = 0, bb = 0;


    Uint8 ro = 0, go = 0, bo = 0;
    Uint32* pixels = (Uint32*) surface->pixels;

    SDL_GetRGB( * (pixels + c  + r * cols ),
            gScreenSurface->format, &ro, &go, &bo);

    int num_sides = 0;

    int asdf = 20;
    int basdf = 21;
    int energy = 0;


    if (c > 0) {
        SDL_GetRGB( pixels[c - 1 + r * cols] ,
                gScreenSurface->format, &rl, &gl, &bl);
        num_sides++;

        // printf("before: %d energy\n", energy);
        // printf("%d %d: %d %d %d to %d %d: %d %d %d\n", r, c, ro, go, bo, r, c-1, rl, gl, bl);
        energy += abs((int)rl - (int)ro) + abs((int)gl - (int)go) + abs((int)bl - (int)bo);
        // printf("after: %d energy\n", energy);

    }

    if (c < cols - 1) {
        SDL_GetRGB( pixels[c + 1 + r * cols ],
                gScreenSurface->format, &rr, &gr, &br);
        num_sides++;

        energy += abs((int)rr - (int)ro) + abs((int)gr - (int)go) + abs((int)br - (int)bo);

    }

    if (r > 0) {
        SDL_GetRGB( pixels[c + (r - 1) * cols],
                gScreenSurface->format, &rt, &gt, &bt);
        num_sides++;

        energy += abs((int)rt - (int)ro) + abs((int)gt - (int)go) + abs((int)bt - (int)bo);

    }
    if (r < rows - 1) {
        SDL_GetRGB( pixels[c + (r + 1) * cols],
                gScreenSurface->format, &rb, &gb, &bb);
        num_sides++;

        energy += abs((int)rb - (int)ro) + abs((int)gb - (int)go) + abs((int)bb - (int)bo);

    }


    energy /= 3;
    energy /= num_sides;

    double e = (double) energy;
    e = log(e);

    if (e < 0)
        e = 0;

    // 256 / ln(256)
    e *= 46;

    return (int) e;
}

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


    // avoid editing pixels and then re-putting them in same loop. Else, it breaks.
    for (i = 0; i < rows; i++) {
        energy[i] = malloc(sizeof(char) * cols);
        for (j = 0; j < cols; j++) {
            energy[i][j] = (char) find_energy(i, j, rows, cols, gPNGSurface);
        }
    }



    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            *((Uint32*)gPNGSurface->pixels + j + i * cols) = SDL_MapRGB(
                    gScreenSurface->format, energy[i][j], energy[i][j], energy[i][j]);
        }
    }




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
