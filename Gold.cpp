#include "Gold.h"
#include "constants.h"
#include <cstdlib>
#include <vector>
#include <iostream>

Gold::Gold(SDL_Texture* texture, int x, int y, int w, int h) : GameObject(texture, x, y, w, h), value(0) {}

Gold Gold::createRandomGold(SDL_Renderer* renderer) {

    Gold gold(nullptr, 0, 0, 0, 0);
    int size = rand() % 60 + 40; // Size between 40 and 99
    //khu vuc vang, da
    int spawnAreaTop = 200;
    int spawnAreaBottom = SCREEN_HEIGHT - size - 20;
    int spawnAreaLeft = 50;
    int spawnAreaRight = SCREEN_WIDTH - size - 50;

    gold.rect = {
        rand() % (spawnAreaRight - spawnAreaLeft) + spawnAreaLeft,
        rand() % (spawnAreaBottom - spawnAreaTop) + spawnAreaTop,
        size,
        size
    };

    gold.value = gold.rect.w / 1.5;
    if (gold.value < 10) gold.value = 10;

    return gold;
}


void Gold::render(SDL_Renderer* renderer) const {
    if (texture != nullptr) {
        GameObject::render(renderer); // Call base class render
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255); // Gold color
        SDL_RenderFillRect(renderer, &rect);
    }
}
