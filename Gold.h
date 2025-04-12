#ifndef GOLD_H
#define GOLD_H

#include "GameObject.h"
#include <SDL.h> // Required for SDL_Rect
#include <vector> // Required for std::vector

using namespace std;

class Gold : public GameObject {
public:
    int value;

    Gold(SDL_Texture* texture, int x, int y, int w, int h);
    static Gold createRandomGold(SDL_Renderer* renderer);

    void render(SDL_Renderer* renderer) const override; // <----- IMPORTANT: 'const override' here
};

#endif
