// Rope.cpp
#include "Rope.h"
#include <cmath>
#include <iostream>
#include "constants.h"

using namespace std;

Rope::Rope(SDL_Texture* tex, int x, int y, int w, int h) : GameObject(tex, x, y, w, h),
length(0), maxLength(SCREEN_HEIGHT - y), speed(10), // maxLength based on screen height
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
            retract(); // Start retracting automatically when max length is reached
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
        // Render grabbed gold at the end of the rope during extending/retracting
        if (attachedGold != nullptr) {
            SDL_Rect goldRenderRect = attachedGold->rect;
            goldRenderRect.x = rect.x + rect.w / 2 - goldRenderRect.w / 2; // Center gold horizontally with rope
            goldRenderRect.y = rect.y + length - goldRenderRect.h; // Attach gold to the end of the rope
            attachedGold->render(renderer); // Use gold's render function
        }
        if (attachedStone != nullptr) {
            SDL_Rect stoneRenderRect = attachedStone->rect;
            stoneRenderRect.x = rect.x + rect.w / 2 - stoneRenderRect.w / 2; // Center gold horizontally with rope
            stoneRenderRect.y = rect.y + length - stoneRenderRect.h; // Attach gold to the end of the rope
            attachedStone->render(renderer); // Use gold's render function
        }

    }

}

Gold* Rope::checkCollision(vector<Gold>& golds) { // <---- Thay đổi kiểu trả về thành const Gold*
    if (!isExtending) return nullptr; // Only check collision when extending

    SDL_Rect ropeEndRect;
    ropeEndRect.x = rect.x - 5; // Adjust for width of rope and collision detection
    ropeEndRect.y = rect.y + length - 5; // End of rope
    ropeEndRect.w = 10; // Width for collision check
    ropeEndRect.h = 10; // Height for collision check

    for (Gold& gold : golds) {
        SDL_Rect intersection;
        if (SDL_IntersectRect(&ropeEndRect, &gold.rect, &intersection)) {
            return &gold; // Return the collided gold object (now returning const Gold*)
        }
    }
    return nullptr; // No collision
}


Stone* Rope::checkCollision(vector<Stone>& stones) { // <---- Thay đổi kiểu trả về thành const Stone*
    if (!isExtending) return nullptr; // Only check collision when extending

    SDL_Rect ropeEndRect;
    ropeEndRect.x = rect.x - 5; // Adjust for width of rope and collision detection
    ropeEndRect.y = rect.y + length - 5; // End of rope
    ropeEndRect.w = 10; // Width for collision check
    ropeEndRect.h = 10; // Height for collision check

    for (Stone& stone : stones) {
        SDL_Rect intersection;
        if (SDL_IntersectRect(&ropeEndRect, &stone.rect, &intersection)) {
            return &stone; // Return the collided stone object (now returning const Stone*)
        }
    }
    return nullptr; // No collision
}
