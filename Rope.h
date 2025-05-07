#ifndef ROPE_H
#define ROPE_H

#include "GameObject.h"
#include "Gold.h"
#include "Stone.h"
#include <SDL.h>
#include <vector>

class Rope : public GameObject {
public:
    int length;
    int maxLength;
    int extendSpeed;
    int retractSpeedFactorGold;
    int retractSpeedFactorStone;
    bool isExtending;
    bool isRetracting;
    Gold* attachedGold;
    Stone* attachedStone;

    double angle;
    double angleSpeed;
    double maxAngle;
    bool isSwinging;
    int startX, startY;

    int thickness;
    Rope(SDL_Texture* tex, int thick, int anchorX, int anchorY, int maxLen);

    void setAnchor(int x, int y);
    void extend();
    void retract();

    void update(int& score, std::vector<Gold>& golds, std::vector<Stone>& stones);

    void render(SDL_Renderer* renderer) const override;

    Gold* checkCollision(std::vector<Gold>& golds);
    Stone* checkCollision(std::vector<Stone>& stones);
};

#endif // ROPE_H
