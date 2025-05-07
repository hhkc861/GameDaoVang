#include "Stone.h"
#include "constants.h"
#include <cstdlib>
#include <vector>
#include <iostream>

Stone::Stone(SDL_Texture* texture, int x, int y, int w, int h) : GameObject(texture, x, y, w, h), value(5) {}

Stone Stone::createRandomStone(SDL_Renderer* renderer) {
    Stone stone(nullptr, 0, 0, 0, 0);
    int size = rand() % 20 + 25;
    int spawnAreaTop = 200;
    int spawnAreaBottom = SCREEN_HEIGHT - size - 20;
    int spawnAreaLeft = 50;
    int spawnAreaRight = SCREEN_WIDTH - size - 50;

    stone.rect = {
        rand() % (spawnAreaRight - spawnAreaLeft) + spawnAreaLeft,
        rand() % (spawnAreaBottom - spawnAreaTop) + spawnAreaTop,
        size,
        size
    };

    stone.value = 5;

    return stone;
}

void Stone::render(SDL_Renderer* renderer) const {
    if (texture != nullptr) {
        GameObject::render(renderer);
    } else {
        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
}
