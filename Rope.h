#ifndef ROPE_H
#define ROPE_H

#include "GameObject.h"
#include "Gold.h"
#include "Stone.h"
#include <SDL.h>

using namespace std;

class Rope : public GameObject {
public:
    int length;
    int maxLength;
    int speed;
    bool isExtending;
    bool isRetracting;
    bool isAttached;
    Gold* attachedGold;
    Stone* attachedStone;

    Rope(SDL_Texture* tex, int x, int y, int w, int h);

    void extend();
    void retract();
    void update();
    void render(SDL_Renderer* renderer) const override;
    void handleSwing();
    Gold* checkCollision(vector<Gold>& golds);
    Stone* checkCollision(vector<Stone>& stones);
};

#endif
