#include <SDL3/SDL.h>

int main()
{
    SDL_Window *window;
    SDL_Renderer *renderer;

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Failed to initialise video: %s", SDL_GetError());
        return -1;
    }

    if (!SDL_CreateWindowAndRenderer("VL Display", 640, 560, SDL_WINDOW_FULLSCREEN, &window, &renderer))
    {
        SDL_Log("Failed to create window and renderer: %s", SDL_GetError());
        return -1;
    }

    SDL_SetRenderLogicalPresentation(renderer, 640, 560, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();

    return 0;
}