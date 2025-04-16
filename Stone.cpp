#include "Stone.h"
#include "constants.h"
#include <cstdlib>
#include <vector>
#include <iostream>

using namespace std;

Stone::Stone(SDL_Texture* texture, int x, int y, int w, int h) : GameObject(texture, x, y, w, h), value(10) {}

Stone Stone::createRandomStone(SDL_Renderer* renderer) {
    Stone stone(nullptr, 0, 0, 0, 0);
    int size = 30;
    stone.rect = {rand() % (SCREEN_WIDTH - size - 20) + 10, rand() % (SCREEN_HEIGHT - size - 100) + 100, size, size};
    return stone;
}

void Stone::render(SDL_Renderer* renderer) const {
    if (texture != nullptr) {
        GameObject::render(renderer);
    } else {
        SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
}
