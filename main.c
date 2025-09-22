#include <stdio.h>
#include <SDL.h>
#include <math.h>

#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 600
#define COLOR_WHITE 0xffffffff

struct Circle
{
    double x;
    double y;
    double r;
};

void FillCircle(SDL_Surface *surface, struct Circle const circle, Uint32 color)
{
    const int minX = (int)(circle.x - circle.r);
    const int maxX = (int)(circle.x + circle.r);
    const int minY = (int)(circle.y - circle.r);
    const int maxY = (int)(circle.y + circle.r);

    const double radius_squared = pow(circle.r, 2);

    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {
            const double distance_squared = pow(x- circle.x, 2) + pow(y - circle.y, 2);

            if (distance_squared < radius_squared)
            {
                SDL_Rect pixel = { x, y, 1, 1 };
                SDL_FillRect(surface, &pixel, color);
            }
        }
    }
}

int main(void) {
    // Initialize SDL video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create a window
    SDL_Window *window = SDL_CreateWindow(
        "Raytracing",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN // important: show the window
    );

    if (!window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Main loop (runs until user closes the window)
    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }
        SDL_Surface *surface = SDL_GetWindowSurface(window);
        auto rect = (SDL_Rect){200,200,200,200};
        SDL_FillRect(surface, &rect, COLOR_WHITE);
        SDL_UpdateWindowSurface(window);

        SDL_Delay(16); // ~60 FPS idle
    }

    // Cleanup
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}