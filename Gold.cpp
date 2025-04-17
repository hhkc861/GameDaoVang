// Gold.cpp
#include "Gold.h"
#include "constants.h" // Assuming SCREEN_WIDTH, SCREEN_HEIGHT are here
#include <cstdlib>
#include <vector>
#include <iostream>

using namespace std;

Gold::Gold(SDL_Texture* texture, int x, int y, int w, int h) : GameObject(texture, x, y, w, h), value(0) {} // Initialize GameObject part and Gold's value

Gold Gold::createRandomGold(SDL_Renderer* renderer) {

    Gold gold(nullptr, 0, 0, 0, 0);
    int size = rand() % 60 + 40;
    gold.rect = {rand() % (SCREEN_WIDTH - size - 100) + 50, rand() % (SCREEN_HEIGHT - size - 200) + 200, size, size}; // Adjusted position range

    gold.value = gold.rect.w / 2;
    if (gold.value < 10) gold.value = 10;

    return gold;
}

void Gold::render(SDL_Renderer* renderer) const {
    if (texture != nullptr) {
        GameObject::render(renderer);
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); //màu vàng
        SDL_RenderFillRect(renderer, &rect);
    }
}
