#include "GameObject.h"
#include <iostream>

using namespace std;

GameObject::GameObject(SDL_Texture* tex, int x, int y, int w, int h) : texture(tex) {
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
}

void GameObject::render(SDL_Renderer* renderer) const { //định nghĩa render
    SDL_RenderCopy(renderer, texture, nullptr, &rect); //gọi hàm để vẽ texture
}
