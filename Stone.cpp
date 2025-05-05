#include "Stone.h"
#include "constants.h"
#include <cstdlib>
#include <vector>
#include <iostream>

// Constructor initializes base GameObject and Stone's value
Stone::Stone(SDL_Texture* texture, int x, int y, int w, int h) : GameObject(texture, x, y, w, h), value(5) {} // Example stone value

// Static function to create stone with random properties
Stone Stone::createRandomStone(SDL_Renderer* renderer) {
    // Similar to Gold::createRandomGold, texture assigned later in main.cpp
    Stone stone(nullptr, 0, 0, 0, 0); // Start with null texture

    // Random size and position (stones usually smaller than gold)
    int size = rand() % 20 + 25; // Size between 25 and 44
    // Use same spawn area logic as gold
    int spawnAreaTop = 200;
    int spawnAreaBottom = SCREEN_HEIGHT - size - 20;
    int spawnAreaLeft = 50;
    int spawnAreaRight = SCREEN_WIDTH - size - 50;

    if (spawnAreaBottom <= spawnAreaTop) spawnAreaBottom = spawnAreaTop + 1;
    if (spawnAreaRight <= spawnAreaLeft) spawnAreaRight = spawnAreaLeft + 1;

    stone.rect = {
        rand() % (spawnAreaRight - spawnAreaLeft) + spawnAreaLeft,
        rand() % (spawnAreaBottom - spawnAreaTop) + spawnAreaTop,
        size,
        size
    };

    // Value could also be based on size, or fixed
    stone.value = 5; // Example fixed value

    return stone;
}

// Render function: uses GameObject::render if texture exists, otherwise draws grey rect
void Stone::render(SDL_Renderer* renderer) const {
    if (texture != nullptr) {
        GameObject::render(renderer); // Call base class render
    } else {
        // Fallback rendering if no texture is assigned
        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // Grey color
        SDL_RenderFillRect(renderer, &rect);
    }
}
