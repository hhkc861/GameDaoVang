#ifndef PLAYER_H
#define PLAYER_H
#include "GameObject.h"
#include "Rope.h" // Thêm include cho Rope

class Player : public GameObject {
public:
    Rope* rope; // Con trỏ đến Rope

    Player(SDL_Texture* tex, int x, int y, int w, int h, SDL_Texture* ropeTexture);
    ~Player();

    void handleInput(const Uint8* keyboardState);
    void update();
    void render(SDL_Renderer* renderer) override; // Override render để vẽ cả dây

    void dropRope(); // Hàm thả dây
};

#endif
