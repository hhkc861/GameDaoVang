// Rope.h
#ifndef ROPE_H
#define ROPE_H

#include "GameObject.h"
#include "Gold.h" // Make sure to include Gold.h here
#include <SDL.h> // Include SDL.h for SDL_Renderer

class Rope : public GameObject {
public:
    int length; // Chiều dài hiện tại của dây
    int maxLength; // Chiều dài tối đa của dây
    int speed; // Tốc độ kéo dài/thu lại của dây
    bool isExtending; // Dây đang kéo dài
    bool isRetracting; // Dây đang thu lại
    bool isAttached; // Dây đã gắn vào vàng/đá
    Gold* attachedGold; // Con trỏ đến cục vàng mà dây đang giữ, nullptr nếu không giữ gì

    float swingAngle;      // Góc xoay hiện tại của dây (độ)
    float swingSpeed;      // Tốc độ xoay (độ/frame)
    float maxSwingAngle;   // Góc xoay tối đa (độ)
    bool swingDirectionRight; // Hướng xoay (true = phải, false = trái)

    Rope(SDL_Texture* tex, int x, int y, int w, int h);
    void extend();
    void retract();
    void update();
    void render(SDL_Renderer* renderer) const override;
    void handleSwing(); // Hàm xử lý xoay dây

    Gold* checkCollision(const std::vector<Gold>& golds);
};

#endif
