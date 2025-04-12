// Gold.cpp
#include "Gold.h"
#include "constants.h" // Assuming SCREEN_WIDTH, SCREEN_HEIGHT are here
#include <cstdlib>
#include <vector>
#include <iostream>

using namespace std;

Gold::Gold(SDL_Texture* texture, int x, int y, int w, int h) : GameObject(texture, x, y, w, h), value(0) {} // Initialize GameObject part and Gold's value

Gold Gold::createRandomGold(SDL_Renderer* renderer) {
    // Random size for gold
    Gold gold(nullptr, 0, 0, 0, 0);
    int size = rand() % 60 + 40; // Size between 40 and 100 (larger range for gold)
    gold.rect = {rand() % (SCREEN_WIDTH - size - 100) + 50, rand() % (SCREEN_HEIGHT - size - 200) + 200, size, size}; // Adjusted position range

    gold.value = gold.rect.w / 2; // Example: Value is half of the width. Adjust scaling as needed.
    if (gold.value < 10) gold.value = 10; // Minimum value if gold is very small

    return gold;
}

void Gold::render(SDL_Renderer* renderer) const {
    if (texture != nullptr) {
        GameObject::render(renderer);
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow color if texture is missing
        SDL_RenderFillRect(renderer, &rect);
    }
}
