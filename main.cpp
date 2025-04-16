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

// Định nghĩa trạng thái game
enum GameState { MENU, PLAYING, GAME_OVER, WIN_SCREEN, LOSE_SCREEN };

// *** ĐỊNH NGHĨA HÀM ĐƯỢC CHUYỂN LÊN ĐÂY, TRƯỚC main() ***
// Hàm hỗ trợ để tải texture và xử lý lỗi
SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* filename) {
    SDL_Surface* surface = IMG_Load(filename);
    if (!surface) {
        cerr << "Lỗi IMG_Load: " << filename << " - " << IMG_GetError() << endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        cerr << "Lỗi SDL_CreateTextureFromSurface: " << filename << " - " << SDL_GetError() << endl;
        return nullptr;
    }
    return texture;
}

// Hàm để tải nhạc - Hàm mới
Mix_Music *loadMusic(const char* path)
{
    Mix_Music *gMusic = Mix_LoadMUS(path);
    if (gMusic == nullptr) {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION,
                       SDL_LOG_PRIORITY_ERROR,
            "Không thể tải nhạc! Lỗi SDL_mixer: %s", Mix_GetError());
    }
    return gMusic;
}

// Hàm để phát nhạc - Hàm mới
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
    // Khởi tạo SDL - Đã sửa đổi để bao gồm SDL_INIT_AUDIO
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        cerr << "SDL fail" << SDL_GetError() << endl;
        return 1;
    }

    // Khởi tạo SDL_mixer - Đoạn code mới
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

    // Tạo cửa sổ - Không thay đổi
    SDL_Window* window = SDL_CreateWindow("Gold Miner", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        cerr << "SDL_CreateWindow failed: " << SDL_GetError() << endl;
        Mix_CloseAudio(); // Dọn dẹp Mixer trước khi thoát
        Mix_Quit();      // Dọn dẹp Mixer trước khi thoát
        SDL_Quit();
        return 1;
    }

    // Tạo renderer - Không thay đổi
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        Mix_CloseAudio(); // Dọn dẹp Mixer trước khi thoát
        Mix_Quit();      // Dọn dẹp Mixer trước khi thoát
        SDL_Quit();
        return 1;
    }

    // Khởi tạo SDL_image - Không thay đổi
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    //Khởi tạo SDL_ttf - Không thay đổi
    TTF_Init();
    if(TTF_Init() == -1){
        cerr << "SDL_ttf fail" << TTF_GetError() << endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        Mix_CloseAudio(); // Dọn dẹp Mixer trước khi thoát
        Mix_Quit();      // Dọn dẹp Mixer trước khi thoát
        SDL_Quit();
        return 1;
    }

    //Khởi tạo font - Không thay đổi
    TTF_Font* font = TTF_OpenFont("arial.ttf", 25);
    if(font == nullptr){
        cerr << "TTF_OpenFont fail" << TTF_GetError() << endl;
        TTF_Quit();
        IMG_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        Mix_CloseAudio(); // Dọn dẹp Mixer trước khi thoát
        Mix_Quit();      // Dọn dẹp Mixer trước khi thoát
        SDL_Quit();
        return 1;
    }

    // Load texture - Không thay đổi (sử dụng hàm hỗ trợ loadTexture, cũng không thay đổi)
    SDL_Texture* backgroundTexture = loadTexture(renderer, "background.jpg");
    SDL_Texture* ropeTexture = nullptr;
    {
        SDL_Surface* tmpSurfaceRope = SDL_CreateRGBSurface(0, 4, 4, 32, 0, 0, 0, 0);
        SDL_FillRect(tmpSurfaceRope, NULL, SDL_MapRGB(tmpSurfaceRope->format, 255, 255, 255));
        ropeTexture = SDL_CreateTextureFromSurface(renderer, tmpSurfaceRope);
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

    if (!backgroundTexture || !ropeTexture || !stoneTexture || !goldTexture || !playerTexture ||
        !menuTexture || !startButtonTexture || !winTexture || !loseTexture || !yesButtonTexture || !noButtonTexture) {
        SDL_DestroyTexture(backgroundTexture);
        SDL_DestroyTexture(ropeTexture);
        SDL_DestroyTexture(stoneTexture);
        SDL_DestroyTexture(goldTexture);
        SDL_DestroyTexture(playerTexture);
        SDL_DestroyTexture(menuTexture);
        SDL_DestroyTexture(startButtonTexture);
        SDL_DestroyTexture(winTexture);
        SDL_DestroyTexture(loseTexture);
        SDL_DestroyTexture(yesButtonTexture);
        SDL_DestroyTexture(noButtonTexture);
        TTF_CloseFont(font);
        TTF_Quit();
        IMG_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        Mix_CloseAudio(); // Dọn dẹp Mixer trước khi thoát
        Mix_Quit();      // Dọn dẹp Mixer trước khi thoát
        SDL_Quit();
        return 1;
    }

    // Tải nhạc nền - Đoạn code mới
    Mix_Music* backgroundMusic = loadMusic("background_music.mp3"); // Thay "background_music.mp3" bằng file nhạc của bạn
    if (!backgroundMusic) {
        SDL_DestroyTexture(backgroundTexture);
        SDL_DestroyTexture(ropeTexture);
        SDL_DestroyTexture(stoneTexture);
        SDL_DestroyTexture(goldTexture);
        SDL_DestroyTexture(playerTexture);
        SDL_DestroyTexture(menuTexture);
        SDL_DestroyTexture(startButtonTexture);
        SDL_DestroyTexture(winTexture);
        SDL_DestroyTexture(loseTexture);
        SDL_DestroyTexture(yesButtonTexture);
        SDL_DestroyTexture(noButtonTexture);
        TTF_CloseFont(font);
        TTF_Quit();
        IMG_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        Mix_CloseAudio(); // Dọn dẹp Mixer trước khi thoát
        Mix_Quit();      // Dọn dẹp Mixer trước khi thoát
        SDL_Quit();
        return 1;
    }


    // Tạo Player và Rope - Không thay đổi
    Player player(playerTexture, SCREEN_WIDTH / 2 - 48, 20, 76, 76);
    Rope rope(ropeTexture, player.rect.x + player.rect.w / 2 - 2, player.rect.y + player.rect.h, 4, 0);

    // Tạo vàng và đá - Không thay đổi
    srand(time(0));
    vector<Gold> golds;
    vector<Stone> stones;

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
    for (const auto& gold : golds) {
        totalGoldValue += gold.value;
    }
    // *** DÒNG THAY ĐỔI BÊN DƯỚI - Đã thêm static_cast<int> ***
    int targetScore = max(0, static_cast<int>((totalGoldValue * 2) / 3));

    player.isMoving = true;
    GameState gameState = MENU;

    SDL_Rect startButtonRect = {14, 85, 300, 250};
    SDL_Rect yesButtonRect = {SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 + 80, 100, 50};
    SDL_Rect noButtonRect = {SCREEN_WIDTH / 2 + 20, SCREEN_HEIGHT / 2 + 80, 100, 50};

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                if (gameState == MENU) {
                    if (mouseX >= startButtonRect.x && mouseX < startButtonRect.x + startButtonRect.w &&
                        mouseY >= startButtonRect.y && mouseY < startButtonRect.y + startButtonRect.h) {
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
                    }
                } else if (gameState == PLAYING) {
                    if (!rope.isExtending && !rope.isRetracting) {
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
            rope.rect.x = player.rect.x + player.rect.w / 2 - 2;
            rope.update();

            if (rope.isExtending) {
                if (grabbedGold == nullptr) {
                    grabbedGold = rope.checkCollision(golds);
                    if (grabbedGold != nullptr) {
                        rope.attachedGold = grabbedGold;
                        rope.retract();
                    }
                }
                if (grabbedStone == nullptr && grabbedGold == nullptr) {
                    grabbedStone = rope.checkCollision(stones);
                    if (grabbedStone != nullptr) {
                        rope.attachedStone = grabbedStone;
                        rope.retract();
                    }
                }
            } else if (!rope.isRetracting) {
                if (grabbedGold != nullptr) {
                    score += grabbedGold->value;
                    golds.erase(remove_if(golds.begin(), golds.end(), [&](const Gold& gold){ return &gold == grabbedGold; }), golds.end());
                    grabbedGold = nullptr;
                } else if (grabbedStone != nullptr) {
                    stones.erase(remove_if(stones.begin(), stones.end(), [&](const Stone& stone){ return &stone == grabbedStone; }), stones.end());
                    grabbedStone = nullptr;
                    // stones may or may not have value, if they do, add score here.
                    // score += grabbedStone->value;
                }
                player.isMoving = true;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

        for (const auto& gold : golds) gold.render(renderer);
        for (const auto& stone : stones) stone.render(renderer);
        rope.render(renderer);
        player.render(renderer);

        if (gameState == MENU) {
            if (Mix_PlayingMusic() == 0) playMusic(backgroundMusic); // Phát nhạc ở trạng thái MENU - Đoạn code mới
            SDL_RenderCopy(renderer, menuTexture, NULL, NULL);
            SDL_RenderCopy(renderer, startButtonTexture, NULL, &startButtonRect);
        } else if (gameState == PLAYING || gameState == GAME_OVER) {
            if (font) {
                SDL_Color textColor = {0, 255, 0};
                SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, ("$" + to_string(score)).c_str(), textColor);
                SDL_Surface* timerSurface = TTF_RenderText_Solid(font, to_string(remainingSeconds).c_str(), textColor);
                SDL_Surface* targetSurface = TTF_RenderText_Solid(font, ("$" + to_string(targetScore)).c_str(), textColor);

                if (scoreSurface) { SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface); SDL_Rect scoreRect = {85, 13, scoreSurface->w, scoreSurface->h}; SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect); SDL_DestroyTexture(scoreTexture); SDL_FreeSurface(scoreSurface); }
                if (timerSurface) { SDL_Texture* timerTexture = SDL_CreateTextureFromSurface(renderer, timerSurface); SDL_Rect timerRect = {SCREEN_WIDTH - timerSurface->w - 10, 10, timerSurface->w, timerSurface->h}; SDL_RenderCopy(renderer, timerTexture, nullptr, &timerRect); SDL_DestroyTexture(timerTexture); SDL_FreeSurface(timerSurface); }
                if (targetSurface) { SDL_Texture* targetTexture = SDL_CreateTextureFromSurface(renderer, targetSurface); SDL_Rect targetRect = {85, 50, targetSurface->w, targetSurface->h}; SDL_RenderCopy(renderer, targetTexture, nullptr, &targetRect); SDL_DestroyTexture(targetTexture); SDL_FreeSurface(targetSurface); }
            }
        } else if (gameState == WIN_SCREEN) {
            SDL_RenderCopy(renderer, winTexture, NULL, NULL);
            SDL_RenderCopy(renderer, yesButtonTexture, NULL, &yesButtonRect);
            SDL_RenderCopy(renderer, noButtonTexture, NULL, &noButtonRect);
        } else if (gameState == LOSE_SCREEN) {
            SDL_RenderCopy(renderer, loseTexture, NULL, NULL);
            SDL_RenderCopy(renderer, yesButtonTexture, NULL, &yesButtonRect);
            SDL_RenderCopy(renderer, noButtonTexture, NULL, &noButtonRect);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // Giải phóng tài nguyên - Đã sửa đổi để bao gồm dọn dẹp nhạc
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
    Mix_FreeMusic(backgroundMusic); // Giải phóng nhạc đã tải - Đoạn code mới
    TTF_CloseFont(font);
    TTF_Quit();
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio(); // Thoát SDL_mixer trước SDL_Quit - Đoạn code mới
    Mix_Quit();      // Thoát SDL_mixer trước SDL_Quit - Đoạn code mới
    SDL_Quit();

    return 0;
}
