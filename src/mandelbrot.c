#include "mandelbrot.h"


static inline double my_creal(complex double z) {
    return creal(z);
}

static inline double my_cimag(complex double z) {
    return cimag(z);
}

void draw_mandelbrot(SDL_Window *window, SDL_Surface *surface, complex double center, double zoom)
{
    int x, y;
    int maxiter = (WIDTH/2) * 0.049715909 * log10(zoom);
    float C;

    static SDL_Rect rects[HEIGHT/FLIPS];

    #pragma omp parallel for private(x, C)
    for (y = 0; y < HEIGHT; y++)
    {
        complex double c_im = (my_cimag(center) + ((y - (HEIGHT/2))/zoom))*_Complex_I;
        for (x = 0; x < WIDTH; x++)
        {
            complex double c_re = creal(center) + ((x - (WIDTH/2))/zoom);
            complex double c = c_re + c_im;
            complex double z = 0;
            int n;

            for (n = 0; n < maxiter; n++)
            {
                z = z*z + c;
                if (my_creal(z)*my_creal(z) + my_cimag(z)*my_cimag(z) > BAIL_OUT)
                    break;
            }

            C = n - log2f(logf(cabs(z)) / M_LN2 );

            ((Uint32*)surface->pixels)[(y * surface->w) + x] = (n >= maxiter) ? 0 :
                SDL_MapRGB(surface->format,
                    (1+sin(C*0.27 + 5)) * 127.0f,
                    (1+cos(C*0.85)) * 127.0f,
                    (1+sin(C*0.15)) * 127.0f
                );
        }
        rects[y/FLIPS].x = 0;
        rects[y/FLIPS].y = y;
        rects[y/FLIPS].w = WIDTH;
        rects[y/FLIPS].h = 1;
    }

    SDL_UpdateWindowSurface(window);
    SDL_Delay(1);
    SDL_UpdateWindowSurfaceRects(window, rects, HEIGHT/FLIPS);
}

void draw_julia(SDL_Window *window, SDL_Surface *surface, complex double c, double zoom)
{
    int x, y;
    int maxiter = (WIDTH/2) * 0.049715909 * log10(zoom);
    float C;

    static SDL_Rect rects[HEIGHT/FLIPS];

    #pragma omp parallel for private(x, C)
    for (y = 0; y < HEIGHT; y++)
    {
        for (x = 0; x < WIDTH; x++)
        {
            complex double z = (x - (WIDTH/2)) / zoom + ((y - (HEIGHT/2)) / zoom) * _Complex_I;
            int n;

            for (n = 0; n < maxiter; n++)
            {
                z = z * z + c;
                if (my_creal(z)*my_creal(z) + my_cimag(z)*my_cimag(z) > BAIL_OUT)
                    break;
            }

            C = n - log2f(logf(cabs(z)) / M_LN2 );

            ((Uint32*)surface->pixels)[(y * surface->w) + x] = (n >= maxiter) ? 0 :
                SDL_MapRGB(surface->format,
                    (1+sin(C*0.27 + 5)) * 127.0f,
                    (1+cos(C*0.85)) * 127.0f,
                    (1+sin(C*0.15)) * 127.0f
                );
        }
        rects[y/FLIPS].x = 0;
        rects[y/FLIPS].y = y;
        rects[y/FLIPS].w = WIDTH;
        rects[y/FLIPS].h = 1;
    }

    SDL_UpdateWindowSurface(window);
    SDL_Delay(1);
    SDL_UpdateWindowSurfaceRects(window, rects, HEIGHT/FLIPS);
}

void handle_keydown(SDL_Keycode key, complex double *center, double *zoom, SDL_Window *window, SDL_Surface *surface) {
    switch (key) {
        case SDLK_SPACE:
            *center = START_POS;
            *zoom = START_ZOOM;
            draw_mandelbrot(window, surface, *center, *zoom);
            break;

        case SDLK_ESCAPE:
            exit(0);
        break;
    }
}

void handle_mousebutton(SDL_MouseButtonEvent button, complex double *center, double *zoom, SDL_Window *window, SDL_Surface *surface) 
{
    *center = creal(*center) + ((button.x - (WIDTH/2))/(*zoom)) + ((cimag(*center) + ((button.y - (HEIGHT/2))/(*zoom)))*_Complex_I);
    
    if (button.button == SDL_BUTTON_LEFT)
        *zoom *= ZOOM_FACTOR;
    else if (button.button == SDL_BUTTON_RIGHT)
        *zoom /= ZOOM_FACTOR;

    draw_mandelbrot(window, surface, *center, *zoom);
}