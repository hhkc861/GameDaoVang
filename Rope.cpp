// Rope.cpp
#include "Rope.h"
#include <cmath>
#include <iostream>
#include "constants.h"

using namespace std;

Rope::Rope(SDL_Texture* tex, int x, int y, int w, int h) : GameObject(tex, x, y, w, h),
length(0), maxLength(SCREEN_HEIGHT - y), speed(10),
isExtending(false), isRetracting(false), attachedGold(nullptr), attachedStone(nullptr){}

void Rope::extend() {
    isExtending = true;
    isRetracting = false;
    isAttached = false;
    length = 0;
}

void Rope::retract() {
    isRetracting = true;
    isExtending = false;
}

void Rope::update() {
    if (isExtending) {
        length += speed;
        if (length >= maxLength) {
            isExtending = false;
            retract();
        }
    } else if (isRetracting) {
        length -= speed;

        if (attachedGold != nullptr){
            attachedGold->rect.y = rect.y + length - attachedGold->rect.h;
        }
        if (attachedStone != nullptr){
            attachedStone->rect.y = rect.y + length - attachedStone->rect.h;
        }

        if (length <= 0) {
            isRetracting = false;
            length = 0;
            attachedGold = nullptr;
            attachedStone = nullptr;
        }
    }
}

void Rope::render(SDL_Renderer* renderer) const {
    if (texture == nullptr) return;

    SDL_Rect renderRect = {rect.x, rect.y, rect.w, length};
    SDL_RenderCopy(renderer, texture, nullptr, &renderRect);

    if (isExtending || isRetracting) {
        if (attachedGold != nullptr) {
            SDL_Rect goldRenderRect = attachedGold->rect;
            goldRenderRect.x = rect.x + rect.w / 2 - goldRenderRect.w / 2;
            goldRenderRect.y = rect.y + length - goldRenderRect.h;
            attachedGold->render(renderer);
        }
        if (attachedStone != nullptr) {
            SDL_Rect stoneRenderRect = attachedStone->rect;
            stoneRenderRect.x = rect.x + rect.w / 2 - stoneRenderRect.w / 2;
            stoneRenderRect.y = rect.y + length - stoneRenderRect.h;
            attachedStone->render(renderer);
        }

    }

}

Gold* Rope::checkCollision(vector<Gold>& golds) {
    if (!isExtending) return nullptr;

    SDL_Rect ropeEndRect;
    ropeEndRect.x = rect.x - 5;
    ropeEndRect.y = rect.y + length - 5;
    ropeEndRect.w = 10;
    ropeEndRect.h = 10;

    for (Gold& gold : golds) {
        SDL_Rect intersection;
        if (SDL_IntersectRect(&ropeEndRect, &gold.rect, &intersection)) {
            return &gold;
        }
    }
    return nullptr;
}


Stone* Rope::checkCollision(vector<Stone>& stones) {
    if (!isExtending) return nullptr;

    SDL_Rect ropeEndRect;
    ropeEndRect.x = rect.x - 5;
    ropeEndRect.y = rect.y + length - 5;
    ropeEndRect.w = 10;
    ropeEndRect.h = 10;

    for (Stone& stone : stones) {
        SDL_Rect intersection;
        if (SDL_IntersectRect(&ropeEndRect, &stone.rect, &intersection)) {
            return &stone;
        }
    }
    return nullptr;
}
