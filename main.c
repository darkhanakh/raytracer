#include <stdio.h>
#include <SDL.h>
#include <math.h>

#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 600
#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0x00000000

struct Circle
{
    double x;
    double y;
    double r;
};

static void PutPixelAlpha(const SDL_Surface *surface, const int x, const int y, Uint32 color, Uint8 alpha)
{
    if (x < 0 || x >= surface->w || y < 0 || y >= surface->h) return;

    Uint8 r, g, b, a;
    SDL_GetRGBA(color, surface->format, &r, &g, &b, &a);

    const auto pixels = (Uint32 *)surface->pixels;
    Uint32 *dst = pixels + y * surface->w + x;

    Uint8 dr, dg, db, da;
    SDL_GetRGBA(*dst, surface->format, &dr, &dg, &db, &da);

    const float af = alpha / 255.0f;
    const Uint8 nr = (Uint8)(r * af + dr * (1 - af));
    const Uint8 ng = (Uint8)(g * af + dg * (1 - af));
    const Uint8 nb = (Uint8)(b * af + db * (1 - af));

    *dst = SDL_MapRGBA(surface->format, nr, ng, nb, 255);
}

void FillCircle(SDL_Surface *surface, struct Circle const circle, Uint32 color)
{
    const int minX = (int)(circle.x - circle.r);
    const int maxX = (int)(circle.x + circle.r);
    const int minY = (int)(circle.y - circle.r);
    const int maxY = (int)(circle.y + circle.r);

    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {
            const double dx = x - circle.x;
            const double dy = y - circle.y;
            const double distance = sqrt(dx * dx + dy * dy);

            if (distance < circle.r - 0.5)
            {
                PutPixelAlpha(surface, x, y, color, 255);
            } else if (distance < circle.r + 0.5)
            {
                Uint8 alpha = (Uint8)((circle.r + 0.5 - distance) * 255);
                PutPixelAlpha(surface, x, y, color, alpha);
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

    SDL_Surface *surface = SDL_GetWindowSurface(window);
    struct Circle circle = {200, 200, 80};
    struct Circle shadow_circle = {650, 300, 120};
    SDL_Rect erase_surface = (SDL_Rect){0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};


    // Main loop (runs until user closes the window)
    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }

            if (event.type == SDL_MOUSEMOTION && event.motion.state != 0)
            {
                circle.x = event.motion.x;
                circle.y = event.motion.y;
            }
        }
        SDL_FillRect(surface, &erase_surface, COLOR_BLACK);
        FillCircle(surface, circle, COLOR_WHITE);
        FillCircle(surface, shadow_circle, COLOR_WHITE);

        SDL_UpdateWindowSurface(window);

        SDL_Delay(8); // ~120 FPS idle
    }

    // Cleanup
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}