#include <assert.h>
#include <iostream>

#include <SDL.h>

// SDL defines a main function itself, so it has to be undefined such that the
// following main is picked up correctly
#undef main

int main(int, char**) {
    assert(SDL_Init(SDL_INIT_VIDEO) >= 0);

    SDL_Window* window = SDL_CreateWindow("SDL + CMake Template",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600,
        0);
    SDL_Renderer* renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

    SDL_Event event;
    bool quit = false;
    int time = SDL_GetTicks();
    while (!quit) {
        int dt = SDL_GetTicks() - time;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) quit = true;
        }

        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawLine(renderer, (time) % 800, 0, 800 - (time % 800), 600);
        SDL_RenderPresent(renderer);

        time += dt;
    }
    return 0;
}
