#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include <SDL.h>

class Player : public GameObject {
public:
    float speed;
    float currentSpeed;
    float acceleration;
    float deceleration;
    bool movingRight;
    bool isMoving;

    Player(SDL_Texture* tex, int x, int y, int w, int h);
    void update();
    void moveLeft();
    void moveRight();
    void stopMoving();
    void render(SDL_Renderer* renderer) const override;
};

#endif
