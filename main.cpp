#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <sstream>

#include <SDL_ttf.h>
#include "constants.h"
#include "GameObject.h"
#include "Rope.h"
#include "Gold.h"
#include "Stone.h"
#include "Player.h"

using namespace std;

// Định nghĩa trạng thái game
enum GameState {
    MENU,
    PLAYING,
    GAME_OVER,
    WIN_SCREEN,
    LOSE_SCREEN
};

int main(int argc, char* argv[]) {

    // Khởi tạo SDL
    SDL_Init(SDL_INIT_VIDEO);
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "SDL fail" << SDL_GetError() << endl;
        return 1;
    }

    // Tạo cửa sổ
    SDL_Window* window = SDL_CreateWindow("Gold Miner", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    // Tạo renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Khởi tạo SDL_image
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    //Khởi tạo SDL_ttf
    TTF_Init();
    if(TTF_Init() == -1){
        cerr << "SDL_ttf fail" << TTF_GetError() << endl;
        return 1;
    }

    //Khởi tạo font
    TTF_Font* font = TTF_OpenFont("arial.ttf", 25);

    // Load texture
    SDL_Surface* tmpSurfaceBackground = IMG_Load("background.jpg");
    SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, tmpSurfaceBackground);
    SDL_FreeSurface(tmpSurfaceBackground);

    SDL_Surface* tmpSurfaceRope = SDL_CreateRGBSurface(0, 4, 4, 32, 0, 0, 0, 0);
    SDL_FillRect(tmpSurfaceRope, NULL, SDL_MapRGB(tmpSurfaceRope->format, 255, 255, 255));
    SDL_Texture* ropeTexture = SDL_CreateTextureFromSurface(renderer, tmpSurfaceRope);
    SDL_FreeSurface(tmpSurfaceRope);

    SDL_Surface* tmpSurfaceStone = IMG_Load("Stone.png");
    SDL_Texture* stoneTexture = SDL_CreateTextureFromSurface(renderer, tmpSurfaceStone);
    SDL_FreeSurface(tmpSurfaceStone);

    SDL_Surface* tmpSurfaceGold = IMG_Load("Gold.png");
    SDL_Texture* goldTexture = SDL_CreateTextureFromSurface(renderer, tmpSurfaceGold);
    SDL_FreeSurface(tmpSurfaceGold);

    SDL_Surface* tmpSurfacePlayer = IMG_Load("player.png");
    SDL_Texture* playerTexture = SDL_CreateTextureFromSurface(renderer, tmpSurfacePlayer);
    SDL_FreeSurface(tmpSurfacePlayer);

    // Load texture menu
    SDL_Surface* tmpSurfaceMenu = IMG_Load("menu.png");
    SDL_Texture* menuTexture = SDL_CreateTextureFromSurface(renderer, tmpSurfaceMenu);
    SDL_FreeSurface(tmpSurfaceMenu);

    SDL_Surface* tmpSurfaceStartButton = IMG_Load("start.png");
    SDL_Texture* startButtonTexture = SDL_CreateTextureFromSurface(renderer, tmpSurfaceStartButton);
    SDL_FreeSurface(tmpSurfaceStartButton);

    // Load texture win/lose screen and buttons
    SDL_Surface* tmpSurfaceWin = IMG_Load("win.png");
    SDL_Texture* winTexture = SDL_CreateTextureFromSurface(renderer, tmpSurfaceWin);
    SDL_FreeSurface(tmpSurfaceWin);

    SDL_Surface* tmpSurfaceLose = IMG_Load("lose.png");
    SDL_Texture* loseTexture = SDL_CreateTextureFromSurface(renderer, tmpSurfaceLose);
    SDL_FreeSurface(tmpSurfaceLose);

    SDL_Surface* tmpSurfaceYesButton = IMG_Load("yes.png");
    SDL_Texture* yesButtonTexture = SDL_CreateTextureFromSurface(renderer, tmpSurfaceYesButton);
    SDL_FreeSurface(tmpSurfaceYesButton);

    SDL_Surface* tmpSurfaceNoButton = IMG_Load("no.png");
    SDL_Texture* noButtonTexture = SDL_CreateTextureFromSurface(renderer, tmpSurfaceNoButton);
    SDL_FreeSurface(tmpSurfaceNoButton);

    // Tạo Player và Rope
    Player player(playerTexture, SCREEN_WIDTH / 2 - 48, 20, 76, 76);
    Rope rope(ropeTexture, player.rect.x + player.rect.w / 2 - 2, player.rect.y + player.rect.h, 4, 0);

    // Tạo vàng và đá
    srand(time(0));
    vector<Gold> golds;
    vector<Stone> stones;

    // ... (phần tạo vàng và đá giữ nguyên)
    for (int i = 0; i < 10; ++i) {
        Gold newGold = Gold::createRandomGold(renderer);
        newGold.texture = goldTexture;
        golds.push_back(newGold);
    }
    for (int i = 0; i < 5; ++i) {
        Stone newStone = Stone::createRandomStone(renderer);
        newStone.texture = stoneTexture;
        stones.push_back(newStone);
    }

    int score = 0;
    bool quit = false;
    SDL_Event e;
    Gold* grabbedGold = nullptr;
    Stone* grabbedStone = nullptr;
    Uint32 gameStartTime = SDL_GetTicks();
    int gameDurationSeconds = 30;
    int remainingSeconds;
    bool gameOver = false;
    string gameResultText;

    int totalGoldValue = 0;
    for(const auto& gold : golds){
        totalGoldValue += gold.value;
    }
    int targetScore = (totalGoldValue * 2) / 3;
    if (targetScore < 0) targetScore = 0;

    player.isMoving = true;

    GameState gameState = MENU;

    SDL_Rect startButtonRect;
    startButtonRect.w = 300;
    startButtonRect.h = 250;
    startButtonRect.x = 14;
    startButtonRect.y = 85;

    // Vị trí và kích thước nút Yes/No
    SDL_Rect yesButtonRect;
    yesButtonRect.w = 100;
    yesButtonRect.h = 50;
    yesButtonRect.x = SCREEN_WIDTH / 2 - yesButtonRect.w - 20; // Canh giữa và cách nhau
    yesButtonRect.y = SCREEN_HEIGHT / 2 + 80;

    SDL_Rect noButtonRect;
    noButtonRect.w = 100;
    noButtonRect.h = 50;
    noButtonRect.x = SCREEN_WIDTH / 2 + 20; // Canh giữa và cách nhau
    noButtonRect.y = SCREEN_HEIGHT / 2 + 80;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (gameState == MENU) {
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);
                    // Kiểm tra click vào nút Start
                    if (mouseX >= startButtonRect.x && mouseX < startButtonRect.x + startButtonRect.w &&
                        mouseY >= startButtonRect.y && mouseY < startButtonRect.y + startButtonRect.h) {
                        gameState = PLAYING; // Chuyển sang trạng thái PLAYING khi click nút Start
                        gameStartTime = SDL_GetTicks(); // Reset game time khi bắt đầu chơi
                        score = 0; // Reset score khi bắt đầu chơi
                        gameOver = false; // Reset game over flag
                        golds.clear(); // Reset golds
                        stones.clear(); // Reset stones
                        for (int i = 0; i < 10; ++i) { // Re-create golds
                            Gold newGold = Gold::createRandomGold(renderer);
                            newGold.texture = goldTexture;
                            golds.push_back(newGold);
                        }
                        for (int i = 0; i < 5; ++i) { // Re-create stones
                            Stone newStone = Stone::createRandomStone(renderer);
                            newStone.texture = stoneTexture;
                            stones.push_back(newStone);
                        }
                        totalGoldValue = 0; // Re-calculate target score
                        for(const auto& gold : golds){
                            totalGoldValue += gold.value;
                        }
                        targetScore = (totalGoldValue * 2) / 3;
                        if (targetScore < 0) targetScore = 0;
                    }
                } else if (gameState == PLAYING) {
                    if (!rope.isExtending && !rope.isRetracting) {
                        player.stopMoving();
                        rope.extend();
                    }
                } else if (gameState == WIN_SCREEN || gameState == LOSE_SCREEN) {
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);
                    // Kiểm tra click vào nút Yes
                    if (mouseX >= yesButtonRect.x && mouseX < yesButtonRect.x + yesButtonRect.w &&
                        mouseY >= yesButtonRect.y && mouseY < yesButtonRect.y + yesButtonRect.h) {
                        gameState = PLAYING; // Chuyển sang trạng thái PLAYING để chơi lại
                        gameStartTime = SDL_GetTicks(); // Reset game time
                        score = 0; // Reset score
                        gameOver = false; // Reset game over flag
                        golds.clear(); // Reset golds
                        stones.clear(); // Reset stones
                        for (int i = 0; i < 10; ++i) { // Re-create golds
                            Gold newGold = Gold::createRandomGold(renderer);
                            newGold.texture = goldTexture;
                            golds.push_back(newGold);
                        }
                        for (int i = 0; i < 5; ++i) { // Re-create stones
                            Stone newStone = Stone::createRandomStone(renderer);
                            newStone.texture = stoneTexture;
                            stones.push_back(newStone);
                        }
                        totalGoldValue = 0; // Re-calculate target score
                        for(const auto& gold : golds){
                            totalGoldValue += gold.value;
                        }
                        targetScore = (totalGoldValue * 2) / 3;
                        if (targetScore < 0) targetScore = 0;
                    }
                    // Kiểm tra click vào nút No
                    else if (mouseX >= noButtonRect.x && mouseX < noButtonRect.x + noButtonRect.w &&
                             mouseY >= noButtonRect.y && mouseY < noButtonRect.y + noButtonRect.h) {
                        quit = true; // Thoát game
                    }
                }
            }
        }

        // Cập nhật game chỉ khi ở trạng thái PLAYING
         if (gameState == PLAYING) {
            if(!gameOver){
                Uint32 currentTime = SDL_GetTicks();
                remainingSeconds = gameDurationSeconds - (currentTime - gameStartTime)/1000;

                if(remainingSeconds <= 0){
                    remainingSeconds = 0;
                    gameOver = true;
                    cout << "Game Over" << endl;

                    if(score >= targetScore){
                        gameResultText = "You win!";
                        cout << "You win!" << endl;
                        gameState = WIN_SCREEN; // Chuyển sang trạng thái WIN_SCREEN
                    } else {
                        gameResultText = "You lose!";
                        cout << "You lose!" << endl;
                        gameState = LOSE_SCREEN; // Chuyển sang trạng thái LOSE_SCREEN
                    }
                    // gameState = GAME_OVER; // No longer used, WIN_SCREEN or LOSE_SCREEN directly
                }
            }

            player.update();
            rope.rect.x = player.rect.x + player.rect.w / 2 - 2;
            rope.update();

            if (rope.isExtending && grabbedGold == nullptr) {
                Gold* collidedGold = rope.checkCollision(golds);
                if (collidedGold != nullptr) {
                    grabbedGold = collidedGold;
                    rope.attachedGold = grabbedGold;
                    rope.retract();
                }
            }
            if (rope.isExtending && grabbedStone == nullptr) {
                Stone* collidedStone = rope.checkCollision(stones);
                if (collidedStone != nullptr) {
                    grabbedStone = collidedStone;
                    rope.attachedStone = grabbedStone;
                    rope.retract();
                }
            }
            if (!rope.isExtending && !rope.isRetracting && grabbedGold != nullptr) {
                score += grabbedGold->value;
                golds.erase(remove_if(golds.begin(), golds.end(),
                                            [&](Gold& gold) { return &gold == grabbedGold; }),
                            golds.end());
                grabbedGold = nullptr;
                player.isMoving = true;
            }
            if (!rope.isExtending && !rope.isRetracting && grabbedStone != nullptr) {
                score += grabbedStone -> value;
                stones.erase(remove_if(stones.begin(), stones.end(),
                                            [&](Stone& stone) { return &stone == grabbedStone; }),
                            stones.end());
                grabbedStone = nullptr;
                player.isMoving = true;
            }
            if (!rope.isExtending && !rope.isRetracting) {
                player.isMoving = true;
            }
        }

        // Render
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        // Vẽ menu hoặc game tùy theo trạng thái
        if (gameState == MENU) {
            SDL_RenderCopy(renderer, menuTexture, NULL, NULL);
            SDL_RenderCopy(renderer, startButtonTexture, NULL, &startButtonRect);
        } else if (gameState == PLAYING || gameState == GAME_OVER) {
            SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

            for (const auto& gold : golds) {
                gold.render(renderer);
            }
            for (const auto& stone : stones) {
                stone.render(renderer);
            }
            rope.render(renderer);
            player.render(renderer);

            if (font != nullptr) {
                SDL_Color textColor = {0, 255, 0};
                string scoreText = "$" + to_string(score);
                string timeText = to_string(remainingSeconds);
                string targetText = "$" + to_string(targetScore);

                // Render Score
                SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
                if (scoreSurface != nullptr) {
                    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
                    SDL_Rect scoreRect = {85, 13, scoreSurface->w, scoreSurface->h};
                    SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);
                    SDL_DestroyTexture(scoreTexture);
                    SDL_FreeSurface(scoreSurface);
                }

                // Render Timer
                SDL_Surface* timerSurface = TTF_RenderText_Solid(font, timeText.c_str(), textColor);
                if (timerSurface != nullptr) {
                    SDL_Texture* timerTexture = SDL_CreateTextureFromSurface(renderer, timerSurface);
                    SDL_Rect timerRect {SCREEN_WIDTH - timerSurface->w - 10, 10, timerSurface->w, timerSurface->h};
                    SDL_RenderCopy(renderer, timerTexture, nullptr, &timerRect);
                    SDL_DestroyTexture(timerTexture);
                    SDL_FreeSurface(timerSurface);
                }
                 // Render Target Score
                SDL_Surface* targetSurface = TTF_RenderText_Solid(font, targetText.c_str(), textColor);
                if (targetSurface != nullptr) {
                    SDL_Texture* targetTexture = SDL_CreateTextureFromSurface(renderer, targetSurface);
                    SDL_Rect targetRect {85, 50, targetSurface->w, targetSurface->h};
                    SDL_RenderCopy(renderer, targetTexture, nullptr, &targetRect);
                    SDL_DestroyTexture(targetTexture);
                    SDL_FreeSurface(targetSurface);
                }
            }
        } else if (gameState == WIN_SCREEN) {
            SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL); // Render background
            SDL_RenderCopy(renderer, winTexture, NULL, NULL); // Render win screen
            SDL_RenderCopy(renderer, yesButtonTexture, NULL, &yesButtonRect); // Render Yes button
            SDL_RenderCopy(renderer, noButtonTexture, NULL, &noButtonRect); // Render No button
        } else if (gameState == LOSE_SCREEN) {
            SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL); // Render background
            SDL_RenderCopy(renderer, loseTexture, NULL, NULL); // Render lose screen
            SDL_RenderCopy(renderer, yesButtonTexture, NULL, &yesButtonRect); // Render Yes button
            SDL_RenderCopy(renderer, noButtonTexture, NULL, &noButtonRect); // Render No button
        }


        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // Giải phóng tài nguyên
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(ropeTexture);
    SDL_DestroyTexture(goldTexture);
    SDL_DestroyTexture(stoneTexture);
    SDL_DestroyTexture(menuTexture);
    SDL_DestroyTexture(startButtonTexture);
    SDL_DestroyTexture(winTexture);
    SDL_DestroyTexture(loseTexture);
    SDL_DestroyTexture(yesButtonTexture);
    SDL_DestroyTexture(noButtonTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
