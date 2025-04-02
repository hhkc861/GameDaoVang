#include "Stone.h"
#include <SDL_image.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>

Stone::Stone(SDL_Texture* tex, int x, int y, int w, int h, int val) : GameObject(tex, x, y, w, h), value(val) {}

Stone Stone::createRandomStone(SDL_Renderer* renderer, const std::vector<SDL_Rect>& existingObjectRects) {
    // ... (createRandomStone implementation - no changes needed here for const correctness) ...
    SDL_Surface* tmpSurface = IMG_Load("player.png"); // Sử dụng lại "player.png" làm ảnh cho đá
    SDL_Texture* stoneTexture = SDL_CreateTextureFromSurface(renderer, tmpSurface);
    SDL_FreeSurface(tmpSurface);

    int width, height, x, y;
    SDL_Rect newStoneRect;
    bool overlap = false;
    int retryCount = 0;
    const int maxRetry = 100;

    do {
        overlap = false;
        width = 48 + rand() % 32; // Kích thước đá, có thể điều chỉnh
        height = width;

        x = rand() % (800 - width);
        y = 200 + rand() % (600 - 300 - height);

        newStoneRect = {x, y, width, height};

        for (const auto& existingRect : existingObjectRects) { // Kiểm tra với tất cả object hiện có (vàng và đá)
            SDL_Rect intersection;
            if (SDL_IntersectRect(&newStoneRect, &existingRect, &intersection)) {
                overlap = true;
                break;
            }
        }
        retryCount++;
        if (retryCount > maxRetry) {
            std::cerr << "Warning: Max retries reached for stone placement, might overlap." << std::endl;
            break;
        }
    } while (overlap);

    int value = 0; // Giá trị của đá, có thể là 0 hoặc giá trị khác nếu cần

    return Stone(stoneTexture, x, y, width, height, value);
}

void Stone::render(SDL_Renderer* renderer) const { // <----- IMPORTANT: 'const' keyword here
    GameObject::render(renderer);
}
