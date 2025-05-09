#include "GameObject.h"
#include <iostream>

GameObject::GameObject(SDL_Texture* tex, int x, int y, int w, int h) : texture(tex) {
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
}

void GameObject::render(SDL_Renderer* renderer) const {
    if (texture != nullptr) {
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }
}
