#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "physics.h"
#include <iostream>

const int screenWidth = 640;
const int screenHeight = 480;

int WinMain(int argc, char* args[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create a window
    SDL_Window* window = SDL_CreateWindow(
        "Moving Red Circle",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        screenWidth,
        screenHeight,
        SDL_WINDOW_SHOWN
    );

    if (window == nullptr) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == nullptr) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Circle properties
    int circleX = screenWidth / 2; // Initial X position (centered)
    int circleY = screenHeight / 2; // Initial Y position (centered)
    int circleRadius = 20; // Circle radius
    Uint8 circleRed = 255; // Red color value

    // Create a Physics object
    Physics physics;

    // Main loop flag
    bool quit = false;

    // Event handler
    SDL_Event e;

    // Main loop
    while (!quit) {
        // Print debug information
        std::cout << "circleX: " << circleX << " velocity: " << physics.getVelocity() << std::endl;

        // Handle events on the queue
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = true;
            }

            // Handle key presses
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                        // Accelerate to the left
                        physics.applyAcceleration(-0.1f);
                        break;
                    case SDLK_RIGHT:
                        // Accelerate to the right
                        physics.applyAcceleration(0.1f);
                        break;
                }
            }

            // Handle key releases
            if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                    case SDLK_RIGHT:
                        // Decelerate (apply friction)
                        physics.applyFriction(0.02f);
                        break;
                }
            }
        }

        // Update physics
        physics.updatePosition(circleX, circleRadius, screenWidth - circleRadius);

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw a filled red circle
        filledCircleRGBA(renderer, circleX, circleY, circleRadius, circleRed, 0, 0, 255);

        // Update the screen
        SDL_RenderPresent(renderer);

        // Introduce a delay to control the speed of movement
        SDL_Delay(10);
    }

    // Destroy window and renderer
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // Quit SDL subsystems
    SDL_Quit();

    return 0;
}
