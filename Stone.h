#ifndef STONE_H
#define STONE_H

#include "GameObject.h"
#include <SDL.h>
#include <vector>

using namespace std;

class Stone : public GameObject {
public:
    int value; // Có thể giữ lại value, hoặc bỏ nếu đá không có giá trị trong game hiện tại

    Stone(SDL_Texture* tex, int x, int y, int w, int h);

    static Stone createRandomStone(SDL_Renderer* renderer);
    // Thay existingGoldRects thành existingObjectRects để dùng chung cho cả vàng và đá
    void render(SDL_Renderer* renderer) const override;
};

#endif
