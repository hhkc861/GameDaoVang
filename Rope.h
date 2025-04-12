#ifndef ROPE_H
#define ROPE_H

#include "GameObject.h"
#include "Gold.h" // Make sure to include Gold.h here
#include "Stone.h"
#include <SDL.h> // Include SDL.h for SDL_Renderer

using namespace std;

class Rope : public GameObject {
public:
    int length; // Chiều dài hiện tại của dây
    int maxLength; // Chiều dài tối đa của dây
    int speed; // Tốc độ kéo dài/thu lại của dây
    bool isExtending; // Dây đang kéo dài
    bool isRetracting; // Dây đang thu lại
    bool isAttached; // Dây đã gắn vào vàng/đá
    Gold* attachedGold; // Con trỏ đến cục vàng
    Stone* attachedStone; // Con trỏ đến cục đá

    Rope(SDL_Texture* tex, int x, int y, int w, int h);

    void extend();
    void retract();
    void update();
    void render(SDL_Renderer* renderer) const override;
    void handleSwing();
    Gold* checkCollision(vector<Gold>& golds);
    Stone* checkCollision(vector<Stone>& stones);
};

#endif
