#include "Gold.h"
#include "constants.h"
#include <cstdlib>
#include <vector>
#include <iostream>

// Constructor initializes base GameObject and Gold's value
Gold::Gold(SDL_Texture* texture, int x, int y, int w, int h) : GameObject(texture, x, y, w, h), value(0) {}

// Static function to create gold with random properties
Gold Gold::createRandomGold(SDL_Renderer* renderer) {
    // Note: This function currently doesn't use the renderer argument.
    // It creates gold without a texture initially. The texture is assigned in main.cpp.
    Gold gold(nullptr, 0, 0, 0, 0); // Start with null texture

    // Random size and position
    int size = rand() % 60 + 40; // Size between 40 and 99
    // Ensure gold spawns below the player/rope area and within bounds
    int spawnAreaTop = 200; // Minimum Y coordinate for spawning
    int spawnAreaBottom = SCREEN_HEIGHT - size - 20; // Max Y
    int spawnAreaLeft = 50; // Min X
    int spawnAreaRight = SCREEN_WIDTH - size - 50; // Max X

    // Ensure valid ranges if screen dimensions are small
    if (spawnAreaBottom <= spawnAreaTop) spawnAreaBottom = spawnAreaTop + 1;
    if (spawnAreaRight <= spawnAreaLeft) spawnAreaRight = spawnAreaLeft + 1;

    gold.rect = {
        rand() % (spawnAreaRight - spawnAreaLeft) + spawnAreaLeft,
        rand() % (spawnAreaBottom - spawnAreaTop) + spawnAreaTop,
        size,
        size
    };

    // Value based on size
    gold.value = gold.rect.w / 2; // Example value calculation
    if (gold.value < 10) gold.value = 10; // Minimum value

    return gold;
}

// Render function: uses GameObject::render if texture exists, otherwise draws yellow rect
void Gold::render(SDL_Renderer* renderer) const {
    if (texture != nullptr) {
        GameObject::render(renderer); // Call base class render
    } else {
        // Fallback rendering if no texture is assigned
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255); // Gold color
        SDL_RenderFillRect(renderer, &rect);
    }
}
