#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include <SDL.h>

class Player : public GameObject {
public:

    Player(SDL_Texture* tex, int x, int y, int w, int h);

};

#endif // PLAYER_H
