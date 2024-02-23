#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "physics.h"
#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>
#include "enemy/Enemy.h"
#include <vector>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int CIRCLE_RADIUS = 20;
const float GRAVITY = 0.5f;
const float JUMP_VELOCITY = -10.0f;  // Negative velocity for jumping
const float MOVEMENT_SPEED = 5.0f;    // Adjust this value based on your preference
const float SHOOTING_SPEED = 10.0f;   // Adjust the shooting speed as needed

bool shooting = false;

struct Projectile {
    float x;
    float y;
    float velocityY;
    bool active;

    Projectile() : x(0.0f), y(0.0f), velocityY(0.0f), active(false) {}
};

void renderFillCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, SDL_Color color) {
    filledCircleColor(renderer, centerX, centerY, radius, SDL_MapRGB(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), color.r, color.g, color.b));
}

int WinMain() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Event e;
    bool quit = false;
    bool isJumping = false;

    float circleX = SCREEN_WIDTH / 2.0f;
    float circleY = SCREEN_HEIGHT - CIRCLE_RADIUS;  // Set the initial position to the bottom

    Physics physics;

    Projectile projectile;

    std::vector<Enemy> enemies;
    enemies.push_back(Enemy(100, 100)); // Example enemy creation

    // Frame rate variables
    int frameCount = 0;
    auto startTime = std::chrono::high_resolution_clock::now();

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                        // Move left
                        physics.setVelocityX(-MOVEMENT_SPEED);
                        break;
                    case SDLK_RIGHT:
                        // Move right
                        physics.setVelocityX(MOVEMENT_SPEED);
                        break;
                    case SDLK_SPACE:
                        std::cout << "Jumping! Y position: " << circleY << std::endl;
                        // Start jumping only when on the bottom
                        if (!isJumping && circleY == 560 ) {
                            physics.setVelocityY(JUMP_VELOCITY);
                            isJumping = true;
                        }
                        break;
                    case SDLK_LCTRL:
                        // Start shooting
                        if (!shooting) {
                            shooting = true;
                            projectile.x = circleX;
                            projectile.y = circleY;
                            projectile.velocityY = -SHOOTING_SPEED; // Shoot upward
                            projectile.active = true; // Activate the projectile
                        }
                        break;
                }
            }
            else if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                    case SDLK_RIGHT:
                        // Stop horizontal movement when left or right key is released
                        physics.setVelocityX(0.0f);
                        break;
                    case SDLK_SPACE:
                        // Stop jumping when space key is released
                        isJumping = false;
                        break;
                      /*  
                    case SDLK_LCTRL:
                        // Stop shooting when Ctrl key is released
                        shooting = false;
                        // Deactivate the projectile
                        projectile.active = false;
                        break;
                        */
                }
            }
        }

        // Update physics and position
        physics.applyAcceleration(0.0f, GRAVITY);
        physics.updatePosition(circleX, circleY, CIRCLE_RADIUS, SCREEN_WIDTH - CIRCLE_RADIUS, CIRCLE_RADIUS, SCREEN_HEIGHT - CIRCLE_RADIUS, CIRCLE_RADIUS);

        // Update projectile position if shooting
        if (shooting && projectile.active) {
            projectile.y += projectile.velocityY;

            // Check if projectile is out of bounds
            if (projectile.y < 0) {
                shooting = false;
                projectile.active = false;
            } else {
                // Check for collision with each enemy
                for (auto& enemy : enemies) {
                    float dx = projectile.x - enemy.getX();
                    float dy = projectile.y - enemy.getY();
                    float distance = std::sqrt(dx * dx + dy * dy);
                    if (distance < CIRCLE_RADIUS + enemy.getRadius()) {
                        // Handle collision
                        enemy.hit();
                        // Deactivate the projectile
                        std::cout << "Ouch!";
                        shooting = false;
                        projectile.active = false;
                        break; // Exit the loop since the projectile can only hit one enemy at a time
                    }
                }
            }
        }


        // Clear the renderer
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (auto& enemy : enemies) {
            enemy.update();
            enemy.render(renderer);
        }

        // Render the filled circle for the player
        renderFillCircle(renderer, static_cast<int>(circleX), static_cast<int>(circleY), CIRCLE_RADIUS, {255, 255, 255}); // White color

        // Render the projectile if shooting
        if (shooting && projectile.active) {
            renderFillCircle(renderer, static_cast<int>(projectile.x), static_cast<int>(projectile.y), 3, {255, 255, 88}); // Red color
        }

        // Present the renderer
        SDL_RenderPresent(renderer);

        // Frame rate control
        frameCount++;

        auto currentTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();

        if (duration >= 1000) {
            double fps = frameCount / (duration / 1000.0);
            std::cout << "FPS: " << fps << std::endl;

            // Reset frame count and start time for the next second
            frameCount = 0;
            startTime = std::chrono::high_resolution_clock::now();
        }

        // Cap the frame rate
        std::this_thread::sleep_for(std::chrono::milliseconds(8));  // Cap the frame rate to approximately 120 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
