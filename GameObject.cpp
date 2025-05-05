#include "GameObject.h"
#include <iostream>

GameObject::GameObject(SDL_Texture* tex, int x, int y, int w, int h) : texture(tex) {
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
}

void GameObject::render(SDL_Renderer* renderer) const {
    if (texture != nullptr) { // Only render if texture exists
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }
    // Optional: Add else case for drawing placeholder if texture is null
    /* else {
        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255); // Magenta placeholder
        SDL_RenderDrawRect(renderer, &rect);
       } */
}
