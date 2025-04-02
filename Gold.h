#ifndef GOLD_H
#define GOLD_H

#include "GameObject.h"
#include <SDL.h> // Required for SDL_Rect
#include <vector> // Required for std::vector

class Gold : public GameObject {
public:
    int value;

    Gold(SDL_Texture* tex, int x, int y, int w, int h, int val);

    // Hàm tạo vàng ngẫu nhiên, nhận danh sách các hình chữ nhật vàng hiện có
    static Gold createRandomGold(SDL_Renderer* renderer, const std::vector<SDL_Rect>& existingObjectRects);

    void render(SDL_Renderer* renderer) const override; // <----- IMPORTANT: 'const override' here
};

#endif
