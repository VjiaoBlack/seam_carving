#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string.h>
#include "carve.hxx"

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gPNGSurface = NULL;

#define MIN(a,b) ((a > b) ? b : a)


int calculate_pixel_difference(int rows, int cols, int row1, int col1, int row2, int col2, SDL_Surface* surface) {
    Uint8 red1, green1, blue1;
    Uint8 red2, green2, blue2;

    Uint32* pixels = (Uint32*) surface->pixels;

    SDL_GetRGB( pixels[col1 + row1 * cols] ,
                gScreenSurface->format, &red1, &green1, &blue1);

    SDL_GetRGB( pixels[col2 + row2 * cols] ,
                gScreenSurface->format, &red2, &green2, &blue2);


    return abs(red1 - red2) + abs(green1 - green2) + abs(blue1 - blue2);

}


int calculate_pixel_energy(int rows, int cols, int row, int col, SDL_Surface* surface) {
    Uint32* pixels = (Uint32*) surface->pixels;

    int num_neighbors = 0;
    int difference = 0;

    if (row > 0) {
        if (col > 0) {
            num_neighbors++;
            difference += calculate_pixel_difference(rows, cols, row, col, row-1, col-1, surface);
            // r-1, c-1
        }
        if (col < cols - 1) {
            num_neighbors++;
            difference += calculate_pixel_difference(rows, cols, row, col, row-1, col+1, surface);
            // r-1, c+1
        }
        num_neighbors++;
        difference += calculate_pixel_difference(rows, cols, row, col, row-1, col, surface);
        // r-1, c
    }
    if (row < rows - 1) {
        if (col > 0) {
            num_neighbors++;
            difference += calculate_pixel_difference(rows, cols, row, col, row+1, col-1, surface);
            // r+1, c-1

        }
        if (col < cols -1 ) {
            num_neighbors++;
            difference += calculate_pixel_difference(rows, cols, row, col, row+1, col+1, surface);
            // r+1, c+1
        }
        num_neighbors++;
        difference += calculate_pixel_difference(rows, cols, row, col, row+1, col, surface);
        // r+1
    }
    if (col > 0) {
        num_neighbors++;
        difference += calculate_pixel_difference(rows, cols, row, col, row, col-1, surface);
        // r, c-1
    }
    if (col < cols - 1) {
        num_neighbors++;
        difference += calculate_pixel_difference(rows, cols, row, col, row, col+1, surface);
        // r, c+1
    }

    return difference / num_neighbors;
}


void calculate_vertical_energy_gradient(int rows, int cols, SDL_Surface* surface, int** energy) {
    // for (int r = 0; r < rows; r++) {
    //     for(int c = 0; c < cols; c++) {

    //         Uint8 rl = 0, gl = 0, bl = 0,
    //               rr = 0, gr = 0, br = 0,
    //               rt = 0, gt = 0, bt = 0,
    //               rb = 0, gb = 0, bb = 0;


    //         Uint8 ro = 0, go = 0, bo = 0;
    //         Uint32* pixels = (Uint32*) surface->pixels;

    //         SDL_GetRGB( * (pixels + c  + r * cols ),
    //                 gScreenSurface->format, &ro, &go, &bo);

    //         int num_sides = 0;

    //         int this_energy = 0;


    //         if (c > 0) {
    //             SDL_GetRGB( pixels[c - 1 + r * cols] ,
    //                     gScreenSurface->format, &rl, &gl, &bl);
    //             num_sides++;

    //             this_energy += abs((int)rl - (int)ro) + abs((int)gl - (int)go) + abs((int)bl - (int)bo);
    //         }

    //         if (c < cols - 1) {
    //             SDL_GetRGB( pixels[c + 1 + r * cols ],
    //                     gScreenSurface->format, &rr, &gr, &br);
    //             num_sides++;

    //             this_energy += abs((int)rr - (int)ro) + abs((int)gr - (int)go) + abs((int)br - (int)bo);

    //         }

    //         this_energy /= 3;
    //         this_energy /= num_sides;

    //         this_energy *= 8;
    //         if (this_energy > 255) {
    //             this_energy = 255;
    //         }
    //         energy[r][c] = this_energy;
    //     }
    // }
    // return 200;

    // now, loop thru r and c urself.


    // use that online algorithm you saw.

    int next_energy[3] = {0}; // energy of left, right, and center.
    Uint8 rc, bc, gc;
    Uint32* pixels = (Uint32*) surface->pixels;

    int cur_energy = 0;


    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            energy[r][c] = 0;
            if (r != 0) {
                next_energy[1] = energy[r-1][c];
                next_energy[0] = 100000;
                next_energy[2] = 100000;

                cur_energy = calculate_pixel_energy(rows, cols, r, c, surface);

                if (c != cols - 1) {
                    next_energy[2] = energy[r-1][c+1];
                }
                if (c != 0) {
                    next_energy[0] = energy[r-1][c-1];
                }


                if (next_energy[0] < next_energy[1]) {
                    if (next_energy[0] < next_energy[2]) {
                        energy[r][c] = cur_energy + energy[r-1][c-1];
                    } else {
                        energy[r][c] = cur_energy + energy[r-1][c+1];

                    }
                } else {
                    if (next_energy[1] <= next_energy[2]) {
                        energy[r][c] = cur_energy + energy[r-1][c];
                    } else {
                        energy[r][c] = cur_energy + energy[r-1][c+1];
                    }
                }


                if (c == 0) {
                    // printf("%d | %d, %d, %d | %d\n", r, next_energy[0], next_energy[1], next_energy[2], energy[r][c]);
                }


            }

            // SDL_GetRGB( pixels[c + r * cols ],
            //             gScreenSurface->format, &rc, &gc, &bc);

            // energy[r][c] = (rc + gc + bc) / 3;
            // energy[r][c] = next_energy[1] / 3;

            // energy[r][c] = cur_energy * 30;
        }
    }

    int max_energy = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (energy[r][c] > max_energy) {
                max_energy = energy[r][c];
            }
        }
    }

    int ratio = (max_energy / 256) + 1;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            energy[r][c] /= ratio;
        }
    }



}

int find_seam(int** vseams, int rows, int cols, int re, int g, int b) {
    // linearly sort for lowest seam energy;
    int i, j;
    int min_energy = 0x7FFFFFFF;
    int min_energy_col = -1;
    for (j = 0; j < cols; j++) {
        if (vseams[rows-1][j] < min_energy && vseams[rows-1][j] > 0) {
            min_energy_col = j;
        }
    }

    j = min_energy_col;

    for (i = rows-1; i >= 1; i--) {
        // paint lowest seam red

        vseams[i][j] = 0 - vseams[i][j] - 1;
        if (re >= 0 && g >= 0 && b >= 0) {
            *((Uint32*)gPNGSurface->pixels + j + i * cols) = SDL_MapRGB(
                        gScreenSurface->format, re, g, b);
        }

        int m, l, r;

        m = vseams[i-1][j];
        l = vseams[i-1][j-1];
        r = vseams[i-1][j+1];

        if (i == 0) {
            break;
        }

        if (j == 0) {
            if (m > r) {
                j++;
            }
        } else if (j == cols - 1) {
            if (m > l) {
                j--;
            }
        } else {
            if (m > r) {
                if (r > l) {
                    j--;
                } else {
                    j++;
                }
            } else if (m > l) {
                j--;
            }
        }


    }

    if (j < 0) {
        j = 0;
    } else if (j > cols - 1) {
        j = cols - 1;
    }
    return 1;
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

    optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, 0 );
    SDL_FreeSurface( loadedSurface );
    gPNGSurface = optimizedSurface;

    int** energy = (int**) malloc(sizeof(int*) * rows);
    int i, j;

    for (int i = 0; i < rows; i++) {
        energy[i] = (int*) malloc(sizeof(int*) * cols);
    }

    calculate_vertical_energy_gradient(rows, cols, gPNGSurface, energy);


    // int** vseams = (int**) malloc(sizeof(int*) * rows);




    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            *((Uint32*)gPNGSurface->pixels + j + i * cols) = SDL_MapRGB(gScreenSurface->format, (char)energy[i][j], (char)energy[i][j], (char)energy[i][j]);
        }
    }

    // vseams[0] = (int*) malloc(sizeof(int) * cols);
    // for (j = 0; j < cols; j++) {
    //     vseams[0][j] = energy[0][j];
    // }

    // for (i = 1; i < rows; i++) {
    //     vseams[i] = (int*) malloc(sizeof(int) * cols);

    //     vseams[i][0] = energy[i][0] + MIN(energy[i-1][0], energy[i-1][1]);
    //     for (j = 1; j < cols - 1; j++) {
    //         vseams[i][j] = energy[i][j] + MIN(MIN(energy[i-1][j-1], energy[i-1][j]), energy[i-1][j+1]);
    //     }

    //     vseams[i][cols-1] = energy[i][cols-1] + MIN(energy[i-1][cols-1], energy[i-1][cols-2]);
    // }

    for (i = 0; i < 255; i++) {

        // find_seam(vseams, rows, cols, 1, i, 10);
    }

    int quit = 0;
    SDL_Event e;

    while( !quit ) {
        while( SDL_PollEvent( &e ) != 0 ) {
            if( e.type == SDL_QUIT ) {
                quit = 1;
            }

            if (e.type ==  SDL_KEYDOWN && e.key.keysym.sym == SDLK_q)  {
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
