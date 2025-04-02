#ifndef STONE_H
#define STONE_H

#include "GameObject.h"
#include <SDL.h>
#include <vector>

class Stone : public GameObject {
public:
    int value; // Có thể giữ lại value, hoặc bỏ nếu đá không có giá trị trong game hiện tại

    Stone(SDL_Texture* tex, int x, int y, int w, int h, int val);

    static Stone createRandomStone(SDL_Renderer* renderer, const std::vector<SDL_Rect>& existingObjectRects); // Thay existingGoldRects thành existingObjectRects để dùng chung cho cả vàng và đá

    void render(SDL_Renderer* renderer) const override; // <----- IMPORTANT: 'const override' here
};

#endif
