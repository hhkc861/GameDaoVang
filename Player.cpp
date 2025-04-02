#include "Player.h"
#include <iostream> // for debugging

Player::Player(SDL_Texture* tex, int x, int y, int w, int h, SDL_Texture* ropeTexture) : GameObject(tex, x, y, w, h) {
    // Tạo Rope (dây) và gán vào con trỏ rope
    rope = new Rope(ropeTexture, x + w / 2 - 2, y + h, 4, 0); // Đặt vị trí ban đầu của dây
}

Player::~Player() {
    delete rope; // Giải phóng Rope khi Player bị hủy
}


void Player::handleInput(const Uint8* keyboardState) {
    if (keyboardState[SDL_SCANCODE_LEFT]) {
        rect.x -= 5;
    }
    if (keyboardState[SDL_SCANCODE_RIGHT]) {
        rect.x += 5;
    }

    if (keyboardState[SDL_SCANCODE_SPACE]) {
        dropRope();
    }
}

void Player::update() {
    // Giữ player trong màn hình (chỉ chiều ngang)
    if (rect.x < 0) rect.x = 0;
    if (rect.x > 800 - rect.w) rect.x = 800 - rect.w;

    // Cập nhật dây
    rope->rect.x = rect.x + rect.w / 2 - 2; // Cập nhật vị trí x của dây theo player
    rope->rect.y = rect.y + rect.h;         // Cập nhật vị trí y của dây theo player
    rope->update(); // Cập nhật chiều dài của dây

}

void Player::render(SDL_Renderer* renderer) { // REMOVE override keyword here
    GameObject::render(renderer); // Vẽ player
    rope->render(renderer);         // Vẽ dây
}

void Player::dropRope() {
    if (!rope->isExtending && !rope->isRetracting) {
        rope->extend(); // Thả dây nếu không đang kéo dài hoặc thu lại
    }
}
