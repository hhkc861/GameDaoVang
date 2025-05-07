#ifndef STONE_H
#define STONE_H

#include "GameObject.h"
#include <SDL.h>
#include <vector>

class Stone : public GameObject {
public:
    int value;

    Stone(SDL_Texture* tex, int x, int y, int w, int h);

    static Stone createRandomStone(SDL_Renderer* renderer);
    void render(SDL_Renderer* renderer) const override;
};

#endif // STONE_H
