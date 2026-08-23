#include <SDL3/SDL.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;



/* This function runs once at startup. */
bool SDL_AppInit()
{
    SDL_SetAppMetadata("Example Renderer Lines", "1.0", "com.example.renderer-lines");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return false;
    }

    if (!SDL_CreateWindowAndRenderer("examples/renderer/lines", 640, 480, SDL_WINDOW_RESIZABLE, &window, &renderer))
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return false;
    }
    SDL_SetRenderLogicalPresentation(renderer, 640, 480, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    return true; /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
bool SDL_AppEvent()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            exit(0);
            break;

        default:
            break;
        }
    }

    return true;
}

bool SDL_AppIterate()
{
    int i;

    static const SDL_FPoint line_points[] = {
        {100, 354}, {220, 230}, {140, 230}, {320, 100}, {500, 230}, {420, 230}, {540, 354}, {400, 354}, {100, 354}};

    /* as you can see from this, rendering draws over whatever was drawn before it. */
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE); /* grey, full alpha */
    SDL_RenderClear(renderer);                                         /* start with a blank canvas. */

    /* You can draw lines, one at a time, like these brown ones... */
    SDL_SetRenderDrawColor(renderer, 127, 49, 32, SDL_ALPHA_OPAQUE);
    SDL_RenderLine(renderer, 240, 450, 400, 450);
    SDL_RenderLine(renderer, 240, 356, 400, 356);
    SDL_RenderLine(renderer, 240, 356, 240, 450);
    SDL_RenderLine(renderer, 400, 356, 400, 450);

    /* You can also draw a series of connected lines in a single batch... */
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderLines(renderer, line_points, SDL_arraysize(line_points));

    /* here's a bunch of lines drawn out from a center point in a circle. */
    /* we randomize the color of each line, so it functions as animation. */
    for (i = 0; i < 360; i++)
    {
        const float size = 30.0f;
        const float x = 320.0f;
        const float y = 95.0f - (size / 2.0f);
        const float r = (float)i * (SDL_PI_F / 180.0f);
        SDL_SetRenderDrawColor(renderer, SDL_rand(256), SDL_rand(256), SDL_rand(256), SDL_ALPHA_OPAQUE);
        SDL_RenderLine(renderer, x, y, x + SDL_cosf(r) * size, y + SDL_sinf(r) * size);
    }

    SDL_RenderPresent(renderer); /* put it all on the screen! */

    return true; /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit()
{
    /* SDL will clean up the window/renderer for us. */
}

// cl /c /I "C:\Personal\Libraries\SDL\include" sdl_test.c && link /machine:x64 /libpath:"C:\Personal\Libraries\SDL\lib\x64" C:\Personal\Libraries\SDL\lib\x64\SDL3.lib sdl_test.obj /out:sdltest.exe

int main()
{
    bool status = SDL_AppInit();

    while (status)
    {
        status = SDL_AppEvent() && SDL_AppIterate();
    }

    SDL_AppQuit();
}