#ifndef GOLD_H
#define GOLD_H

#include "GameObject.h"
#include <SDL.h>
#include <vector>


class Gold : public GameObject {
public:
    int value;

    Gold(SDL_Texture* texture, int x, int y, int w, int h);
    static Gold createRandomGold(SDL_Renderer* renderer);

    void render(SDL_Renderer* renderer) const override;
};

#endif // GOLD_H
