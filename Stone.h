#ifndef STONE_H
#define STONE_H

#include "GameObject.h"
#include <SDL.h>
#include <vector>

// using namespace std; // Avoid in headers

class Stone : public GameObject {
public:
    int value; // Stones can have value too (e.g., small positive or even negative)

    Stone(SDL_Texture* tex, int x, int y, int w, int h);

    static Stone createRandomStone(SDL_Renderer* renderer); // See Gold::createRandomGold comments

    // Override render for potential null texture
    void render(SDL_Renderer* renderer) const override;
};

#endif // STONE_H
