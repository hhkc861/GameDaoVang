#include "Player.h"
#include "constants.h"

Player::Player(SDL_Texture* tex, int x, int y, int w, int h) : GameObject(tex, x, y, w, h),
speed(3),
currentSpeed(0.0f),
acceleration(0.15f),
deceleration(0.2f),
movingRight(true),
isMoving(true) {}

    void Player::update() {
        if (isMoving) {
            if (movingRight) {
                currentSpeed += acceleration;
                if (currentSpeed > speed) {
                    currentSpeed = speed;
                }
            } else {
                currentSpeed -= acceleration; // Subtract acceleration for left movement
                if (currentSpeed < -speed) { // Check against negative speed
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

        rect.x += static_cast<int>(currentSpeed);

        if (rect.x + rect.w > SCREEN_WIDTH) {
            rect.x = SCREEN_WIDTH - rect.w;
            movingRight = false; // Change direction to left
            currentSpeed = 0;   // Stop at boundary briefly (optional, but might look better)
            isMoving = true;     // Continue moving automatically
        } else if (rect.x < 0) {
            rect.x = 0;
            movingRight = true;  // Change direction to right
            currentSpeed = 0;    // Stop at boundary briefly (optional)
            isMoving = true;      // Continue moving automatically
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

    void Player::stopMoving() { // Thêm Player::
        isMoving = false;
    }

    void Player::render(SDL_Renderer* renderer) const {
    GameObject::render(renderer);
    }
