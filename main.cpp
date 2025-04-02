#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>

#include "GameObject.h"
#include "Rope.h"
#include "Gold.h"
#include "Stone.h"

// Kích thước cửa sổ
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int main(int argc, char* argv[]) {
    // Khởi tạo SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Tạo cửa sổ
    SDL_Window* window = SDL_CreateWindow("Gold Miner", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Tạo renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Khởi tạo SDL_image (hỗ trợ cả PNG và JPG)
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        return 1;
    }

    // Load texture ảnh nền (background.jpg)
    SDL_Surface* tmpSurfaceBackground = IMG_Load("background.jpg");
    if (tmpSurfaceBackground == nullptr) {
        std::cerr << "Failed to load background texture! SDL Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, tmpSurfaceBackground);
    SDL_FreeSurface(tmpSurfaceBackground);
    if (backgroundTexture == nullptr) {
        std::cerr << "Failed to create background texture! SDL Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Load texture dây
    SDL_Surface* tmpSurfaceRope = SDL_CreateRGBSurface(0, 4, 4, 32, 0, 0, 0, 0);
    SDL_FillRect(tmpSurfaceRope, NULL, SDL_MapRGB(tmpSurfaceRope->format, 255, 255, 255));
    SDL_Texture* ropeTexture = SDL_CreateTextureFromSurface(renderer, tmpSurfaceRope);
    SDL_FreeSurface(tmpSurfaceRope);
    if (ropeTexture == nullptr) {
        std::cerr << "Failed to create rope texture! SDL Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Load texture đá (vẫn dùng player.png cho đá)
    SDL_Surface* tmpSurfaceStone = IMG_Load("player.png");
    if (tmpSurfaceStone == nullptr) {
        std::cerr << "Failed to load stone texture! SDL Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_Texture* stoneTexture = SDL_CreateTextureFromSurface(renderer, tmpSurfaceStone);
    SDL_FreeSurface(tmpSurfaceStone);
    if (stoneTexture == nullptr) {
        std::cerr << "Failed to create stone texture! SDL Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Load texture vàng
    SDL_Surface* tmpSurfaceGold = IMG_Load("gold.png");
    if (tmpSurfaceGold == nullptr) {
        std::cerr << "Failed to load gold texture! SDL Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_Texture* goldTexture = SDL_CreateTextureFromSurface(renderer, tmpSurfaceGold);
    SDL_FreeSurface(tmpSurfaceGold);
    if (goldTexture == nullptr) {
        std::cerr << "Failed to create gold texture! SDL Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Tạo Rope ở giữa phía trên màn hình
    Rope rope(ropeTexture, SCREEN_WIDTH / 2 - 2, 50, 4, 0);

    // Tạo vàng và đá
    srand(time(0));
    std::vector<Gold> golds;
    std::vector<Stone> stones;
    std::vector<SDL_Rect> objectRects;

    for (int i = 0; i < 10; ++i) {
        Gold newGold = Gold::createRandomGold(renderer, objectRects);
        newGold.texture = goldTexture;
        golds.push_back(newGold);
        objectRects.push_back(newGold.rect);
    }
    for (int i = 0; i < 5; ++i) {
        Stone newStone = Stone::createRandomStone(renderer, objectRects);
        newStone.texture = stoneTexture;
        stones.push_back(newStone);
        objectRects.push_back(newStone.rect);
    }

    bool quit = false;
    SDL_Event e;
    Gold* grabbedGold = nullptr;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_SPACE) {
                    if (!rope.isExtending && !rope.isRetracting) {
                        rope.extend();
                    }
                }
            }
        }

        // Kiểm tra va chạm với vàng khi dây đang kéo dài
        if (rope.isExtending && grabbedGold == nullptr) {
            Gold* collidedGold = rope.checkCollision(golds);
            if (collidedGold != nullptr) {
                grabbedGold = collidedGold;
                rope.attachedGold = grabbedGold;
                rope.retract();
            }
        }

        // Cập nhật game
        rope.update();

        // Khi dây thu lại hoàn tất và đã gắp được vàng, loại bỏ vàng
        if (!rope.isExtending && !rope.isRetracting && grabbedGold != nullptr) {
            golds.erase(std::remove_if(golds.begin(), golds.end(),
                                        [&](const Gold& gold) { return &gold == grabbedGold; }),
                        golds.end());
            grabbedGold = nullptr;
        }

        // Render
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

        for (const auto& gold : golds) {
            gold.render(renderer);
        }
        for (const auto& stone : stones) {
            stone.render(renderer);
        }
        rope.render(renderer);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // Giải phóng tài nguyên
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(ropeTexture);
    SDL_DestroyTexture(goldTexture);
    SDL_DestroyTexture(stoneTexture);
    for (const auto& gold : golds) {
        SDL_DestroyTexture(gold.texture);
    }
    for (const auto& stone : stones) {
        SDL_DestroyTexture(stone.texture);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
