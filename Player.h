#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include <SDL.h>

class Player : public GameObject {
public:
    // No movement variables or methods needed for stationary player

    Player(SDL_Texture* tex, int x, int y, int w, int h);

    // Inherits render from GameObject
    // No update() needed for stationary player
};

#endif // PLAYER_H
