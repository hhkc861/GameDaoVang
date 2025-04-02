#include "Gold.h"
#include <SDL_image.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>

Gold::Gold(SDL_Texture* tex, int x, int y, int w, int h, int val) : GameObject(tex, x, y, w, h), value(val) {}

Gold Gold::createRandomGold(SDL_Renderer* renderer, const std::vector<SDL_Rect>& existingObjectRects) {
// ... (createRandomGold implementation - no changes needed here for const correctness) ...
SDL_Surface* tmpSurface = IMG_Load("gold.png");
SDL_Texture* goldTexture = SDL_CreateTextureFromSurface(renderer, tmpSurface);
SDL_FreeSurface(tmpSurface);

int width, height, x, y;
SDL_Rect newGoldRect;
bool overlap = false;
int retryCount = 0;
const int maxRetry = 100;

do {
overlap = false;
width = 32 + rand() % 32;
height = width;

x = rand() % (800 - width);
y = 200 + rand() % (600 - 300 - height);

newGoldRect = {x, y, width, height};

for (const auto& existingRect : existingObjectRects) {
    SDL_Rect intersection;
    if (SDL_IntersectRect(&newGoldRect, &existingRect, &intersection)) {
        overlap = true;
        break;
    }
}
retryCount++;
if (retryCount > maxRetry) {
    std::cerr << "Warning: Max retries reached for gold placement, might overlap." << std::endl;
    break;
}
} while (overlap);

int value = 50 + rand() % 100;

return Gold(goldTexture, x, y, width, height, value);

}

void Gold::render(SDL_Renderer* renderer) const { // <----- IMPORTANT: 'const' keyword here
GameObject::render(renderer);
}
