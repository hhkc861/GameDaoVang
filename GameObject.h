#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <SDL.h>

class GameObject {
public:
    SDL_Texture* texture;
    SDL_Rect rect;

    GameObject(SDL_Texture* tex, int x, int y, int w, int h);
    virtual ~GameObject() = default; // Good practice to have a virtual destructor
    virtual void render(SDL_Renderer* renderer) const;
    // Keep update virtual in case subclasses need it, even if Player doesn't anymore
    virtual void update() {};
};

#endif // GAMEOBJECT_H
