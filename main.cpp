#include "GameObject.h"
#include "Gold.h"
#include "constants.h"
#include "Stone.h"
#include "Player.h"
#include "Rope.h"

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <sstream>

using namespace std;

enum GameState { MENU, PLAYING, GAME_OVER, WIN_SCREEN, LOSE_SCREEN, INSTRUCTIONS };

SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* filename) {
    SDL_Surface* surface = IMG_Load(filename);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

// Hàm để tải nhạc
Mix_Music *loadMusic(const char* path)
{
    Mix_Music *gMusic = Mix_LoadMUS(path);
    return gMusic;
}

// Hàm để phát nhạc
void playMusic(Mix_Music *gMusic)
{
    if (gMusic == nullptr) return;

    if (Mix_PlayingMusic() == 0) {
        Mix_PlayMusic( gMusic, -1 ); // -1 để lặp lại nhạc vô hạn
    }
    else if( Mix_PausedMusic() == 1 ) {
        Mix_ResumeMusic();
    }
}

int main(int argc, char* argv[]) {
    // Khởi tạo SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        cerr << "SDL fail" << SDL_GetError() << endl;
        return 1;
    }

    // Khởi tạo SDL_mixer
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) {
       cerr << "SDL_mixer không thể khởi tạo! Lỗi SDL_mixer: %s\n" << Mix_GetError() << endl;
        SDL_Quit();
        return 1;
    }
    if (Mix_Init(MIX_INIT_MP3) != MIX_INIT_MP3) {
        cerr << "Lỗi Mix_Init: " << Mix_GetError() << endl;
        Mix_CloseAudio();
        SDL_Quit();
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

    //Khởi tạo font
    TTF_Font* font = TTF_OpenFont("arial.ttf", 25);

    // Load texture
    SDL_Texture* backgroundTexture = loadTexture(renderer, "background.jpg");
    SDL_Texture* ropeTexture = nullptr;
    {
        SDL_Surface* tmpSurfaceRope = SDL_CreateRGBSurface(0, 4, 4, 32, 0, 0, 0, 0); //tao surface tam thoi cho day thung
        SDL_FillRect(tmpSurfaceRope, NULL, SDL_MapRGB(tmpSurfaceRope->format, 255, 255, 255)); //mau trang cho day
        ropeTexture = SDL_CreateTextureFromSurface(renderer, tmpSurfaceRope); //tao texture tu surface
        SDL_FreeSurface(tmpSurfaceRope);
    }
    SDL_Texture* stoneTexture = loadTexture(renderer, "Stone.png");
    SDL_Texture* goldTexture = loadTexture(renderer, "Gold.png");
    SDL_Texture* playerTexture = loadTexture(renderer, "player.png");
    SDL_Texture* menuTexture = loadTexture(renderer, "menu.png");
    SDL_Texture* startButtonTexture = loadTexture(renderer, "start.png");
    SDL_Texture* winTexture = loadTexture(renderer, "win.png");
    SDL_Texture* loseTexture = loadTexture(renderer, "lose.png");
    SDL_Texture* yesButtonTexture = loadTexture(renderer, "yes.png");
    SDL_Texture* noButtonTexture = loadTexture(renderer, "no.png");
    SDL_Texture* instructionsButtonTexture = loadTexture(renderer, "instructions.png");
    SDL_Texture* introTexture = loadTexture(renderer, "intro.jpg");
    Mix_Music* backgroundMusic = loadMusic("background_music.mp3");

    // Tạo Player và Rope
    Player player(playerTexture, SCREEN_WIDTH / 2 - 48, 20, 76, 76);
    Rope rope(ropeTexture, player.rect.x + player.rect.w / 2 - 2, player.rect.y + player.rect.h, 4, 0);

    // Tạo vàng và đá
    srand(time(0)); //khoi tao seed cho random
    vector<Gold> golds;
    vector<Stone> stones;

    for (int i = 0; i < 10; ++i) {
        Gold newGold = Gold::createRandomGold(renderer); //tạo ngẫu nhiên
        newGold.texture = goldTexture; //gán texture
        golds.push_back(newGold); //thêm vào vector golds
    }
    for (int i = 0; i < 5; ++i) {
        Stone newStone = Stone::createRandomStone(renderer);
        newStone.texture = stoneTexture;
        stones.push_back(newStone);
    }

    int score = 0;
    bool quit = false;
    SDL_Event e;
    Gold* grabbedGold = nullptr;  //con trỏ đến cục vàng bị gắp
    Stone* grabbedStone = nullptr;
    Uint32 gameStartTime = SDL_GetTicks();  //thời điểm bắt đầu game
    int gameDurationSeconds = 30; //thời gian chơi
    int remainingSeconds; //thời gian còn lại
    bool gameOver = false;
    string gameResultText; //kết quả game

    int totalGoldValue = 0; //giá trị tổng
    for (const auto& gold : golds) {
        totalGoldValue += gold.value;
    }
    int targetScore = max(0, static_cast<int>((totalGoldValue * 2) / 3));

    player.isMoving = true;  // player di chuyển
    GameState gameState = MENU;  //trạng thái game ban đầu là menu

    SDL_Rect startButtonRect = {14, 85, 300, 250};
    SDL_Rect instructionsButtonRect = {14, 350, 300, 100};
    SDL_Rect yesButtonRect = {SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 + 80, 100, 50};
    SDL_Rect noButtonRect = {SCREEN_WIDTH / 2 + 20, SCREEN_HEIGHT / 2 + 80, 100, 50};

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) { //xử lí sự kiện
            if (e.type == SDL_QUIT) { //khi đóng cửa sổ
                quit = true;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) { //khi nhấn chuột
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                if (gameState == MENU) {
                    if (mouseX >= startButtonRect.x && mouseX < startButtonRect.x + startButtonRect.w &&
                        mouseY >= startButtonRect.y && mouseY < startButtonRect.y + startButtonRect.h) {
                            gameState = PLAYING;
                            gameStartTime = SDL_GetTicks(); //reset tgian bđầu gamme
                            score = 0;
                            gameOver = false;
                            golds.clear();
                            stones.clear();
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
                            totalGoldValue = 0;
                            for (const auto& gold : golds) {
                                totalGoldValue += gold.value;
                            }
                            targetScore = max(0, static_cast<int>((totalGoldValue * 2) / 3));
                    } else if (mouseX >= instructionsButtonRect.x && mouseX < instructionsButtonRect.x + instructionsButtonRect.w &&
                               mouseY >= instructionsButtonRect.y && mouseY < instructionsButtonRect.y + instructionsButtonRect.h) {
                        gameState = INSTRUCTIONS; // Switch to INSTRUCTIONS state
                    }
                } else if (gameState == PLAYING) {
                    if (!rope.isExtending && !rope.isRetracting) { //dây thừng k mở rộng & thu lại
                        player.stopMoving();
                        rope.extend();
                    }
                } else if (gameState == WIN_SCREEN || gameState == LOSE_SCREEN) {
                    if (mouseX >= yesButtonRect.x && mouseX < yesButtonRect.x + yesButtonRect.w &&
                        mouseY >= yesButtonRect.y && mouseY < yesButtonRect.y + yesButtonRect.h) {
                        gameState = PLAYING;
                        gameStartTime = SDL_GetTicks();
                        score = 0;
                        gameOver = false;
                        golds.clear();
                        stones.clear();
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
                        totalGoldValue = 0;
                        for (const auto& gold : golds) {
                            totalGoldValue += gold.value;
                        }
                        targetScore = max(0, static_cast<int>((totalGoldValue * 2) / 3));
                    } else if (mouseX >= noButtonRect.x && mouseX < noButtonRect.x + noButtonRect.w &&
                               mouseY >= noButtonRect.y && mouseY < noButtonRect.y + noButtonRect.h) {
                        quit = true;
                    }
                } else if (gameState == INSTRUCTIONS) {
                    // No mouse click handling in INSTRUCTIONS state for now
                }
            }
            if (e.type == SDL_KEYDOWN) { // Handle key presses
                if (gameState == INSTRUCTIONS) {
                    if (e.key.keysym.sym == SDLK_RETURN) { // If Enter key is pressed
                        gameState = MENU; // Return to MENU state
                    }
                }
            }
        }

        if (gameState == PLAYING) {
            if (!gameOver) {
                remainingSeconds = max(0, static_cast<int>(gameDurationSeconds - (SDL_GetTicks() - gameStartTime) / 1000));
                if (remainingSeconds == 0) {
                    gameOver = true;
                    gameResultText = (score >= targetScore) ? "You win!" : "You lose!";
                    gameState = (score >= targetScore) ? WIN_SCREEN : LOSE_SCREEN;
                }
            }

            player.update();
            rope.rect.x = player.rect.x + player.rect.w / 2 - 2; //update vị trí dây thừng
            rope.update();

            if (rope.isExtending) {
                if (grabbedGold == nullptr && grabbedStone == nullptr) {
                    grabbedGold = rope.checkCollision(golds);
                    grabbedStone = rope.checkCollision(stones);
                    if (grabbedGold != nullptr) {
                        rope.attachedGold = grabbedGold; //gắn vàng vào dây
                        rope.retract();
                    }
                    if (grabbedStone != nullptr) {
                        rope.attachedStone = grabbedStone;
                        rope.retract();
                    }
                }
            } //xoá vàng và đá khi dây = 0
            else if (!rope.isRetracting) {
                if (grabbedGold != nullptr) {
                    score += grabbedGold->value;
                    golds.erase(remove_if(golds.begin(), golds.end(), [&](const Gold& gold){ return &gold == grabbedGold; }), golds.end());
                    grabbedGold = nullptr;
                } else if (grabbedStone != nullptr) {
                    score += grabbedStone->value;
                    stones.erase(remove_if(stones.begin(), stones.end(), [&](const Stone& stone){ return &stone == grabbedStone; }), stones.end());
                    grabbedStone = nullptr;
                }
                player.isMoving = true;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF); // Thiết lập màu vẽ renderer thành đen
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL); //để đảm bảo background đc vẽ đầu tiên

        for (const auto& gold : golds) gold.render(renderer);
        for (const auto& stone : stones) stone.render(renderer);
        rope.render(renderer);
        player.render(renderer);

        if (gameState == MENU) {
            if (Mix_PlayingMusic() == 0) playMusic(backgroundMusic); //phát nhạc
            SDL_RenderCopy(renderer, menuTexture, NULL, NULL); //vẽ backgroung
            SDL_RenderCopy(renderer, startButtonTexture, NULL, &startButtonRect); //vẽ nút start
            SDL_RenderCopy(renderer, instructionsButtonTexture, NULL, &instructionsButtonRect); // Render instructions button
        } else if (gameState == PLAYING || gameState == GAME_OVER) {
            if (font) {
                SDL_Color textColor = {0, 255, 0}; //màu xanh
                SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, ("$" + to_string(score)).c_str(), textColor); //render điểm thành surface
                SDL_Surface* timerSurface = TTF_RenderText_Solid(font, to_string(remainingSeconds).c_str(), textColor);
                SDL_Surface* targetSurface = TTF_RenderText_Solid(font, ("$" + to_string(targetScore)).c_str(), textColor);

                //khi render thành công
                if (scoreSurface) {
                    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface); //tạo texture
                    SDL_Rect scoreRect = {85, 13, scoreSurface->w, scoreSurface->h};  //vị trí
                    SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect); SDL_DestroyTexture(scoreTexture); //texture điểm số & giải phóng
                    SDL_FreeSurface(scoreSurface); } //giải phóng surface
                if (timerSurface) {
                    SDL_Texture* timerTexture = SDL_CreateTextureFromSurface(renderer, timerSurface);
                    SDL_Rect timerRect = {SCREEN_WIDTH - timerSurface->w - 10, 10, timerSurface->w, timerSurface->h};
                    SDL_RenderCopy(renderer, timerTexture, nullptr, &timerRect); SDL_DestroyTexture(timerTexture);
                    SDL_FreeSurface(timerSurface); }
                if (targetSurface) {
                    SDL_Texture* targetTexture = SDL_CreateTextureFromSurface(renderer, targetSurface);
                    SDL_Rect targetRect = {85, 50, targetSurface->w, targetSurface->h};
                    SDL_RenderCopy(renderer, targetTexture, nullptr, &targetRect);
                    SDL_DestroyTexture(targetTexture); SDL_FreeSurface(targetSurface); }
            }
        } else if (gameState == WIN_SCREEN) {
            SDL_RenderCopy(renderer, winTexture, NULL, NULL);
            SDL_RenderCopy(renderer, yesButtonTexture, NULL, &yesButtonRect);
            SDL_RenderCopy(renderer, noButtonTexture, NULL, &noButtonRect);
        } else if (gameState == LOSE_SCREEN) {
            SDL_RenderCopy(renderer, loseTexture, NULL, NULL);
            SDL_RenderCopy(renderer, yesButtonTexture, NULL, &yesButtonRect);
            SDL_RenderCopy(renderer, noButtonTexture, NULL, &noButtonRect);
        } else if (gameState == INSTRUCTIONS) {
            SDL_RenderCopy(renderer, introTexture, NULL, NULL); // Render intro image
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

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
    SDL_DestroyTexture(instructionsButtonTexture);
    SDL_DestroyTexture(introTexture);
    Mix_FreeMusic(backgroundMusic);
    TTF_CloseFont(font);
    TTF_Quit();
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    Mix_Quit();

    return 0;
}
