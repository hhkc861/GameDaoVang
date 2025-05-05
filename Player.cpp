#include "Player.h"
#include "constants.h" // Include if needed, though not directly used here

// Simplified constructor
Player::Player(SDL_Texture* tex, int x, int y, int w, int h) : GameObject(tex, x, y, w, h) {
    // No initialization needed beyond GameObject constructor
}

// render() is inherited from GameObject.
// No update() needed.
