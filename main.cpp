#include "GameObject.h"
#include "Gold.h"
#include "Stone.h"
#include "Player.h"
#include "Rope.h"
#include "constants.h"

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <cmath>

using namespace std;

const int ROPE_THICKNESS = 8;

enum GameState {
    MENU,
    PLAYING,
    WIN_SCREEN,
    LOSE_SCREEN,
    INSTRUCTIONS
};

SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* filename) {
    SDL_Surface* surface = IMG_Load(filename);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

Mix_Music* loadMusic(const char* path) {
    Mix_Music* gMusic = Mix_LoadMUS(path);
    return gMusic;
}

void playMusic(Mix_Music* gMusic) {
    if (gMusic == nullptr) return;
    if (Mix_PlayingMusic() == 0) {
        Mix_PlayMusic(gMusic, -1);
    } else if (Mix_PausedMusic() == 1) {
        Mix_ResumeMusic();
    }
}

int main(int argc, char* argv[]) {

    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;

    if (TTF_Init() == -1) {
        cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << endl;
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << endl;
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Gold Miner - Swinging Rope", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Load Font
    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);

    SDL_Texture* ropeSegmentTexture = nullptr;
    {
        SDL_Surface* surf = SDL_CreateRGBSurface(0, ROPE_THICKNESS, 1, 32, 0, 0, 0, 0);
        if (!surf) {
             cerr << "Failed to create rope surface! SDL Error: " << SDL_GetError() << endl;
             if(font) TTF_CloseFont(font);
             SDL_DestroyRenderer(renderer);
             SDL_DestroyWindow(window);
             Mix_CloseAudio(); Mix_Quit(); TTF_Quit(); IMG_Quit(); SDL_Quit();
             return 1;
        } else {
            SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format, 255, 255, 255));
            ropeSegmentTexture = SDL_CreateTextureFromSurface(renderer, surf);
            if (!ropeSegmentTexture) {
                 cerr << "Failed to create rope texture! SDL Error: " << SDL_GetError() << endl;
                 SDL_FreeSurface(surf);
                 if(font) TTF_CloseFont(font);
                 SDL_DestroyRenderer(renderer);
                 SDL_DestroyWindow(window);
                 Mix_CloseAudio(); Mix_Quit(); TTF_Quit(); IMG_Quit(); SDL_Quit();
                 return 1;
            }
            SDL_FreeSurface(surf);
        }
    }
    SDL_SetTextureBlendMode(ropeSegmentTexture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureColorMod(ropeSegmentTexture, 139, 69, 19);

    SDL_Texture* backgroundTexture = loadTexture(renderer, "background.jpg");
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
    SDL_Texture* backButtonTexture = loadTexture(renderer, "back.png"); // <<< ADDED: Load back button texture
    Mix_Music* backgroundMusic = loadMusic("background_music.mp3");

    int playerWidth = 76;
    int playerHeight = 76;
    Player player(playerTexture, SCREEN_WIDTH / 2 - playerWidth / 2, 20, playerWidth, playerHeight);

    int ropeStartX = player.rect.x + player.rect.w / 2;
    int ropeStartY = player.rect.y + player.rect.h;
    int ropeMaxLength = SCREEN_HEIGHT - ropeStartY - 10;
    Rope rope(ropeSegmentTexture, ROPE_THICKNESS, ropeStartX, ropeStartY, ropeMaxLength);

    vector<Gold> golds;
    vector<Stone> stones;

    int score = 0;
    int targetScore = 100;
    int totalGoldValue = 0;
    bool quit = false;
    SDL_Event e;
    Uint32 gameStartTime = 0;
    int gameDurationSeconds = 30;
    int remainingSeconds = gameDurationSeconds;
    bool gameOver = false;
    GameState gameState = MENU;

    srand(time(0));

    SDL_Rect startButtonRect = {20, 100, 300, 250};
    SDL_Rect instructionsButtonRect = {20, 320, 230, 100};
    SDL_Rect yesButtonRect = {70, SCREEN_HEIGHT / 2 + 50, 200, 70};
    SDL_Rect noButtonRect = {500, SCREEN_HEIGHT / 2 + 50, 200, 70};
    SDL_Rect backButtonRect = {700, 40, 60, 40};

    auto resetGame = [&]() {
        golds.clear();
        stones.clear();
        score = 0;
        gameOver = false;
        gameStartTime = SDL_GetTicks();
        remainingSeconds = gameDurationSeconds;

        rope = Rope(ropeSegmentTexture, ROPE_THICKNESS, ropeStartX, ropeStartY, ropeMaxLength);
        totalGoldValue = 0;

        for (int i = 0; i < 10; ++i) {
            Gold newGold = Gold::createRandomGold(renderer);
            newGold.texture = goldTexture;
            golds.push_back(newGold);
            totalGoldValue += newGold.value;
        }
        for (int i = 0; i < 5; ++i) {
            Stone newStone = Stone::createRandomStone(renderer);
            newStone.texture = stoneTexture;
            stones.push_back(newStone);
        }

        targetScore = max(100, static_cast<int>(round(totalGoldValue * 2.0 / 3.0))); // Use 2.0/3.0 for float division

        gameState = PLAYING;
    };

    while (!quit) {

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }

            switch (gameState) {
                case MENU:
                    if (e.type == SDL_MOUSEBUTTONDOWN) {
                        int mouseX, mouseY;
                        SDL_GetMouseState(&mouseX, &mouseY);
                        SDL_Point mousePoint = {mouseX, mouseY};

                        if (SDL_PointInRect(&mousePoint, &startButtonRect)) {
                            resetGame();
                        } else if (SDL_PointInRect(&mousePoint, &instructionsButtonRect)) {
                            gameState = INSTRUCTIONS;
                        }
                    }
                    break;

                case PLAYING:
                    if (e.type == SDL_MOUSEBUTTONDOWN) {
                        int mouseX, mouseY;
                        SDL_GetMouseState(&mouseX, &mouseY);
                        SDL_Point mousePoint = {mouseX, mouseY};

                        if (SDL_PointInRect(&mousePoint, &backButtonRect)) {
                            gameState = MENU;
                            if (Mix_PlayingMusic()) { // Pause music when going to menu
                                Mix_PauseMusic();
                            }
                        }
                    }
                    else if (e.type == SDL_KEYDOWN) {
                        if (e.key.keysym.sym == SDLK_SPACE) {
                            rope.extend();
                        }
                    }
                    break;

                case WIN_SCREEN:
                case LOSE_SCREEN:
                    if (e.type == SDL_MOUSEBUTTONDOWN) {
                        int mouseX, mouseY;
                        SDL_GetMouseState(&mouseX, &mouseY);
                        SDL_Point mousePoint = {mouseX, mouseY};

                        if (SDL_PointInRect(&mousePoint, &yesButtonRect)) {
                            resetGame();
                        } else if (SDL_PointInRect(&mousePoint, &noButtonRect)) {
                            quit = true;
                        }
                    }
                    else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                        quit = true;
                    }
                    break;

                case INSTRUCTIONS:
                    if (e.type == SDL_KEYDOWN) {
                        if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_ESCAPE || e.key.keysym.sym == SDLK_SPACE) {
                            gameState = MENU;
                        }
                    }
                    else if (e.type == SDL_MOUSEBUTTONDOWN) {
                         gameState = MENU;
                    }
                    break;
            }
        }
        if (gameState == PLAYING) {
            playMusic(backgroundMusic); // Ensure music plays/resumes when entering PLAYING state
            if (!gameOver) {
                Uint32 elapsedTime = SDL_GetTicks() - gameStartTime;
                remainingSeconds = max(0, gameDurationSeconds - static_cast<int>(elapsedTime / 1000));
                if (remainingSeconds == 0) {
                    gameOver = true;
                    gameState = (score >= targetScore) ? WIN_SCREEN : LOSE_SCREEN;
                }
            }
            if (!gameOver) {
                 rope.update(score, golds, stones);
            }
        } else if (gameState == MENU) { // Added this to ensure music plays on menu
            playMusic(backgroundMusic);
        }


        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        switch (gameState) {
            case MENU:
                // playMusic(backgroundMusic); // Moved up to playMusic always when in MENU state
                if (menuTexture) SDL_RenderCopy(renderer, menuTexture, NULL, NULL);
                if (startButtonTexture) SDL_RenderCopy(renderer, startButtonTexture, NULL, &startButtonRect);
                if (instructionsButtonTexture) SDL_RenderCopy(renderer, instructionsButtonTexture, NULL, &instructionsButtonRect);
                break;

            case PLAYING:
                if (backgroundTexture) SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

                for (const auto& gold : golds) gold.render(renderer);
                for (const auto& stone : stones) stone.render(renderer);

                rope.render(renderer);
                player.render(renderer);

                if (backButtonTexture) SDL_RenderCopy(renderer, backButtonTexture, NULL, &backButtonRect); // <<< ADDED: Render back button

                if (font) {
                    SDL_Color textColor = {255, 255, 255, 255};
                    string scoreText = "$" + to_string(score);
                    string timeText = to_string(remainingSeconds);
                    string targetText = " $" + to_string(targetScore);

                    SDL_Surface* scoreSurface = TTF_RenderText_Blended(font, scoreText.c_str(), textColor);
                    SDL_Surface* timerSurface = TTF_RenderText_Blended(font, timeText.c_str(), textColor);
                    SDL_Surface* targetSurface = TTF_RenderText_Blended(font, targetText.c_str(), textColor);

                    if (scoreSurface) {
                        SDL_Texture* scoreTextureUI = SDL_CreateTextureFromSurface(renderer, scoreSurface); // Renamed to avoid conflict
                        SDL_Rect scoreRect = {85, 12, scoreSurface->w, scoreSurface->h};
                        SDL_RenderCopy(renderer, scoreTextureUI, nullptr, &scoreRect);
                        SDL_DestroyTexture(scoreTextureUI); SDL_FreeSurface(scoreSurface);
                    }
                    if (timerSurface) {
                        SDL_Texture* timerTexture = SDL_CreateTextureFromSurface(renderer, timerSurface);
                        SDL_Rect timerRect = {SCREEN_WIDTH - timerSurface->w - 20, 10, timerSurface->w, timerSurface->h};
                        SDL_RenderCopy(renderer, timerTexture, nullptr, &timerRect);
                        SDL_DestroyTexture(timerTexture); SDL_FreeSurface(timerSurface);
                    }
                    if (targetSurface) {
                        SDL_Texture* targetTextureUI = SDL_CreateTextureFromSurface(renderer, targetSurface); // Renamed
                        SDL_Rect targetRect = {80, 50, targetSurface->w, targetSurface->h};
                        SDL_RenderCopy(renderer, targetTextureUI, nullptr, &targetRect);
                        SDL_DestroyTexture(targetTextureUI); SDL_FreeSurface(targetSurface);
                    }
                }
                break;

            case WIN_SCREEN:
                 if (winTexture) SDL_RenderCopy(renderer, winTexture, NULL, NULL);
                 if (yesButtonTexture) SDL_RenderCopy(renderer, yesButtonTexture, NULL, &yesButtonRect);
                 if (noButtonTexture) SDL_RenderCopy(renderer, noButtonTexture, NULL, &noButtonRect);
                 break;

            case LOSE_SCREEN:
                 if (loseTexture) SDL_RenderCopy(renderer, loseTexture, NULL, NULL);
                 if (yesButtonTexture) SDL_RenderCopy(renderer, yesButtonTexture, NULL, &yesButtonRect);
                 if (noButtonTexture) SDL_RenderCopy(renderer, noButtonTexture, NULL, &noButtonRect);
                 break;

            case INSTRUCTIONS:
                if (introTexture) SDL_RenderCopy(renderer, introTexture, NULL, NULL);
                if (font) {
                    SDL_Color textColor = {255, 255, 255, 255}; // White text
                    SDL_Color shadowColor = {0, 0, 0, 255};     // Black shadow

                    const char* lines[] = {
                        " "
                    };
                    int num_lines = sizeof(lines) / sizeof(lines[0]);
                    int yPos = 60; // Initial Y position

                    for (int i = 0; i < num_lines; ++i) {
                        if (strlen(lines[i]) > 0) { // Only render non-empty lines
                            // Render shadow first
                            SDL_Surface* surfShadow = TTF_RenderText_Blended(font, lines[i], shadowColor);
                            if (surfShadow) {
                                SDL_Texture* texShadow = SDL_CreateTextureFromSurface(renderer, surfShadow);
                                SDL_Rect rShadow = {SCREEN_WIDTH / 2 - surfShadow->w / 2 + 1, yPos + 1, surfShadow->w, surfShadow->h}; // Offset for shadow
                                SDL_RenderCopy(renderer, texShadow, NULL, &rShadow);
                                SDL_DestroyTexture(texShadow);
                                SDL_FreeSurface(surfShadow);
                            }

                            // Render actual text
                            SDL_Surface* surfText = TTF_RenderText_Blended(font, lines[i], textColor);
                            if (surfText) {
                                SDL_Texture* texText = SDL_CreateTextureFromSurface(renderer, surfText);
                                SDL_Rect rText = {SCREEN_WIDTH / 2 - surfText->w / 2, yPos, surfText->w, surfText->h};
                                SDL_RenderCopy(renderer, texText, NULL, &rText);
                                SDL_DestroyTexture(texText);
                                SDL_FreeSurface(surfText);
                                yPos += rText.h + 5; // Move Y position for next line
                            } else {
                                yPos += TTF_FontHeight(font) + 5; // Fallback line height
                            }
                        } else {
                             yPos += (TTF_FontHeight(font) / 2) + 5; // Smaller gap for empty lines
                        }
                    }
                }
                break;
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // Cap framerate roughly to 60 FPS
    }

    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(backgroundTexture);
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
    SDL_DestroyTexture(backButtonTexture); // <<< ADDED: Destroy back button texture
    SDL_DestroyTexture(ropeSegmentTexture);


    if (backgroundMusic) Mix_FreeMusic(backgroundMusic);
    if (font) TTF_CloseFont(font);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    Mix_CloseAudio();
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
