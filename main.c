#include <stdio.h>
#include <SDL.h>
#include <math.h>

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 700
#define RAYS_NUMBER 150

struct Circle
{
    double x;
    double y;
    double r;
};

struct Ray
{
    double x_start, y_start;
    double angle;
    double x_end, y_end;
};

int RayCircleHit(double ox, double oy, double dx, double dy, struct Circle circle, double* t_hit)
{
    double cx = circle.x, cy = circle.y, r = circle.r;
    double ocx = ox - cx;
    double ocy = oy - cy;

    double a = dx * dx + dy * dy; // normally 1
    double b = 2 * (dx * ocx + dy * ocy);
    double cc = ocx * ocx + ocy * ocy - r * r;

    double disc = b * b - 4 * a * cc;
    if (disc < 0) return 0; // no hit

    double t1 = (-b - sqrt(disc)) / (2 * a);
    double t2 = (-b + sqrt(disc)) / (2 * a);

    if (t1 > 0)
    {
        *t_hit = t1;
        return 1;
    }
    if (t2 > 0)
    {
        *t_hit = t2;
        return 1;
    }
    return 0;
}

static void generate_rays(const struct Circle circle, struct Ray* rays)
{
    double max_len = sqrt(WINDOW_WIDTH * WINDOW_WIDTH +
        WINDOW_HEIGHT * WINDOW_HEIGHT);

    for (int i = 0; i < RAYS_NUMBER; i++)
    {
        double angle = i * (2.0 * M_PI / RAYS_NUMBER);
        double dx = cos(angle);
        double dy = sin(angle);
        double x1 = circle.x + dx * max_len;
        double y1 = circle.y + dy * max_len;
        rays[i] = (struct Ray){circle.x, circle.y, angle, x1, y1};
    }
}

static void update_rays_with_shadows(struct Ray* rays, const struct Circle circle, int ray_count)
{
    for (int i = 0; i < ray_count; i++)
    {
        double dx = cos(rays[i].angle);
        double dy = sin(rays[i].angle);
        double t_hit;

        if (RayCircleHit(rays[i].x_start, rays[i].y_start, dx, dy, circle, &t_hit))
        {
            rays[i].x_end = rays[i].x_start + dx * t_hit;
            rays[i].y_end = rays[i].y_start + dy * t_hit;
        }
    }
}

void DrawCircle(SDL_Renderer* renderer, struct Circle circle)
{
    int minX = (int)(circle.x - circle.r);
    int maxX = (int)(circle.x + circle.r);
    int minY = (int)(circle.y - circle.r);
    int maxY = (int)(circle.y + circle.r);

    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {
            double dx = x - circle.x;
            double dy = y - circle.y;
            double distance = sqrt(dx * dx + dy * dy);
            if (distance <= circle.r)
            {
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
    }
}

void DrawRays(SDL_Renderer* renderer, struct Ray* rays)
{
    for (int i = 0; i < RAYS_NUMBER; i++)
    {
        SDL_RenderDrawLine(renderer,
                           (int)rays[i].x_start, (int)rays[i].y_start,
                           (int)rays[i].x_end, (int)rays[i].y_end);
    }
}

int main(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Raytracing (Renderer mode)",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window)
    {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // ⚡ Hardware-accelerated renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer)
    {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    struct Ray rays[RAYS_NUMBER];
    struct Circle circle = {200, 200, 80};
    struct Circle shadow_circle = {650, 300, 120};

    generate_rays(circle, rays);
    update_rays_with_shadows(rays, shadow_circle, RAYS_NUMBER);

    int running = 1;
    SDL_Event event;
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
            }
            if (event.type == SDL_MOUSEMOTION && event.motion.state != 0)
            {
                circle.x = event.motion.x;
                circle.y = event.motion.y;
                generate_rays(circle, rays);
                update_rays_with_shadows(rays, shadow_circle, RAYS_NUMBER);
            }
        }

        // clear screen black
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // draw rays (grayish)
        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
        DrawRays(renderer, rays);

        // draw light circle (white)
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        DrawCircle(renderer, circle);

        // draw shadow circle (white)
        DrawCircle(renderer, shadow_circle);

        // present frame
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
