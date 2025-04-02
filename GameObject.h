#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <SDL.h>

class GameObject {
public:
    SDL_Texture* texture;
    SDL_Rect rect;

    GameObject(SDL_Texture* tex, int x, int y, int w, int h);
    virtual ~GameObject() {} // Virtual destructor for proper inheritance cleanup

    virtual void render(SDL_Renderer* renderer) const; // <----- IMPORTANT:  'const' keyword here
};

#endif
