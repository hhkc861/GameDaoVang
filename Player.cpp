#include "Player.h"
#include "constants.h"

Player::Player(SDL_Texture* tex, int x, int y, int w, int h) : GameObject(tex, x, y, w, h),
speed(3), //tốc độ tối đa
currentSpeed(0.0f),  //tốc độ hiện tại đứng yên
acceleration(0.15f),  //gia tốc 0.15f
deceleration(0.2f),  //giảm tốc
movingRight(true),
isMoving(true) {}

    void Player::update() {
        if (isMoving) {
            if (movingRight) {
                currentSpeed += acceleration;  //tăng tốc theo gia tốc
                if (currentSpeed > speed) {
                    currentSpeed = speed;
                }
            } else {
                currentSpeed -= acceleration;
                if (currentSpeed < -speed) {
                    currentSpeed = -speed;
                }
            }
        } else {
            if (currentSpeed > 0) {
                currentSpeed -= deceleration;
                if (currentSpeed < 0) {
                    currentSpeed = 0;
                }
            } else if (currentSpeed < 0) {
                currentSpeed += deceleration;
                if (currentSpeed > 0) {
                    currentSpeed = 0;
                }
            }
        }

        rect.x += static_cast<int>(currentSpeed); //update vị trí x của người chơi trên tốc độ hiện tại

        if (rect.x + rect.w > SCREEN_WIDTH) { //nếu vượt quá chiều rộng màn hình cạnh phải
            rect.x = SCREEN_WIDTH - rect.w;
            movingRight = false;
            currentSpeed = 0;
            isMoving = true;
        } else if (rect.x < 0) { //cạnh trái
            rect.x = 0;
            movingRight = true;
            currentSpeed = 0;
            isMoving = true;
        }
    }

    void Player::moveLeft() {
        movingRight = false;
        isMoving = true;
        currentSpeed = 0.0f;
    }

    void Player::moveRight() {
        movingRight = true;
        isMoving = true;
        currentSpeed = 0.0f;
    }

    void Player::stopMoving() {
        isMoving = false;
    }

    void Player::render(SDL_Renderer* renderer) const {
    GameObject::render(renderer);
    }
