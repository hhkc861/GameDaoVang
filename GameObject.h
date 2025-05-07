#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <SDL.h>

class GameObject {
public:
    SDL_Texture* texture;
    SDL_Rect rect;

    GameObject(SDL_Texture* tex, int x, int y, int w, int h);
    virtual ~GameObject() = default;
    virtual void render(SDL_Renderer* renderer) const;
    virtual void update() {};
};

#endif // GAMEOBJECT_H
