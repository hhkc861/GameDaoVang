#include "GameObject.h" // Base class definitions
#include "Gold.h"       // Gold object definition
#include "Stone.h"      // Stone object definition
#include "Player.h"     // Player object definition (now stationary)
#include "Rope.h"       // Rope object definition (now handles swinging)
#include "constants.h"  // Screen dimensions

#define SDL_MAIN_HANDLED // Define this before including SDL.h to handle main() redirection
#include <SDL.h>
#include <SDL_image.h> // For loading images (PNG, JPG)
#include <SDL_ttf.h>   // For rendering text
#include <SDL_mixer.h> // For playing audio

#include <iostream>    // For standard I/O (like cerr)
#include <vector>      // For using std::vector to hold game objects
#include <string>      // For using std::string
#include <sstream>     // For converting numbers to strings (std::to_string alternative)
#include <ctime>       // For seeding random number generator (srand, time)
#include <cstdlib>     // For random number generation (rand)
#include <algorithm>   // For std::max, std::remove_if
#include <cmath>       // For std::max used in target score

// Use std namespace locally within this file
using namespace std;

// Define game states
enum GameState {
    MENU,
    PLAYING,
    // GAME_OVER, // This state might be merged into WIN/LOSE
    WIN_SCREEN,
    LOSE_SCREEN,
    INSTRUCTIONS
};

// Helper function to load a texture
SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* filename) {
    SDL_Log("Loading texture: %s", filename); // Log texture loading
    SDL_Surface* surface = IMG_Load(filename);
    if (!surface) {
        cerr << "Failed to load image " << filename << "! IMG_Error: " << IMG_GetError() << endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        cerr << "Failed to create texture from " << filename << "! SDL Error: " << SDL_GetError() << endl;
    }
    SDL_FreeSurface(surface); // Free the temporary surface
    return texture;
}

// Helper function to load music
Mix_Music* loadMusic(const char* path) {
    Mix_Music* gMusic = Mix_LoadMUS(path);
    if (gMusic == nullptr) {
        cerr << "Failed to load music " << path << "! Mix_Error: " << Mix_GetError() << endl;
    }
    return gMusic;
}

// Helper function to play music (looping)
void playMusic(Mix_Music* gMusic) {
    if (gMusic == nullptr) return; // Don't try to play null music

    if (Mix_PlayingMusic() == 0) {
        // If no music is playing, play the loaded music indefinitely (-1 loops)
        Mix_PlayMusic(gMusic, -1);
    } else if (Mix_PausedMusic() == 1) {
        // If music is paused, resume it
        Mix_ResumeMusic();
    }
    // If music is already playing, do nothing
}

// --- Main Function ---
int main(int argc, char* argv[]) {
    // --- Initialization ---
    // Initialize SDL Video and Audio
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        return 1;
    }

    // Initialize SDL_image for PNG/JPG loading
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        cerr << "SDL_image could not initialize! IMG_Error: " << IMG_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    // Initialize SDL_ttf for font rendering
    if (TTF_Init() == -1) {
        cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << endl;
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Initialize SDL_mixer for audio playback
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cerr << "SDL_mixer could not initialize! Mix_Error: " << Mix_GetError() << endl;
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    // Optional: Initialize specific mixer formats like MP3
    int mixFlags = MIX_INIT_MP3; // Add MIX_INIT_OGG etc. if needed
    if ((Mix_Init(mixFlags) & mixFlags) != mixFlags) {
         cerr << "Failed to init required audio format support! Mix_Error: " << Mix_GetError() << endl;
         // Continue even if MP3 fails? Or quit? Decide based on importance.
    }

    // Create Window
    SDL_Window* window = SDL_CreateWindow("Gold Miner - Swinging Rope", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        Mix_Quit();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Create Renderer (Hardware accelerated, VSync enabled)
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        Mix_Quit();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Load Font
    TTF_Font* font = TTF_OpenFont("arial.ttf", 24); // Try loading a common font, size 24
    if (!font) {
        cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << endl;
        // Continue without text? Or quit? Decide based on importance.
    }

    // --- Load Assets ---
    SDL_Texture* backgroundTexture = loadTexture(renderer, "background.jpg"); // Assuming assets are in an 'assets' folder
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
    Mix_Music* backgroundMusic = loadMusic("background_music.mp3"); // Load background music

    // --- Game Object Creation ---
    // Create Player (centered horizontally, near top)
    int playerWidth = 76;
    int playerHeight = 76;
    Player player(playerTexture, SCREEN_WIDTH / 2 - playerWidth / 2, 20, playerWidth, playerHeight);

    // Create Rope (anchored to player's bottom center)
    int ropeStartX = player.rect.x + player.rect.w / 2;
    int ropeStartY = player.rect.y + player.rect.h;
    int ropeMaxLength = SCREEN_HEIGHT - ropeStartY - 10; // Max length stops just above screen bottom
    Rope rope(ropeStartX, ropeStartY, ropeMaxLength);   // Create the initial rope

    // Containers for gold and stones
    vector<Gold> golds;
    vector<Stone> stones;

    // --- Game Variables ---
    int score = 0;
    int targetScore = 100; // Initial target score
    int totalGoldValue = 0; // To calculate target score dynamically
    bool quit = false;
    SDL_Event e;
    Uint32 gameStartTime = 0; // Will be set when game starts
    int gameDurationSeconds = 60; // Duration of a game round
    int remainingSeconds = gameDurationSeconds;
    bool gameOver = false; // Flag if timer ran out
    GameState gameState = MENU; // Start at the main menu

    // Seed random number generator
    srand(time(0));

    // --- UI Element Positions ---
    SDL_Rect startButtonRect = {SCREEN_WIDTH / 2 - 150, 200, 300, 100}; // Centered buttons
    SDL_Rect instructionsButtonRect = {SCREEN_WIDTH / 2 - 150, 320, 300, 100};
    SDL_Rect yesButtonRect = {SCREEN_WIDTH / 2 - 120 - 10, SCREEN_HEIGHT / 2 + 80, 100, 50}; // Yes/No slightly spaced
    SDL_Rect noButtonRect = {SCREEN_WIDTH / 2 + 20 + 10, SCREEN_HEIGHT / 2 + 80, 100, 50};

    // Lambda function to reset the game state for a new round
    auto resetGame = [&]() {
        golds.clear();
        stones.clear();
        score = 0;
        gameOver = false;
        gameStartTime = SDL_GetTicks(); // Reset timer start point
        remainingSeconds = gameDurationSeconds;

        // Recreate the rope to reset its state (angle, length, attached objects)
        rope = Rope(ropeStartX, ropeStartY, ropeMaxLength);

        // Create new gold and stones
        totalGoldValue = 0;
        for (int i = 0; i < 10; ++i) { // Number of gold nuggets
            Gold newGold = Gold::createRandomGold(renderer);
            newGold.texture = goldTexture; // Assign the loaded texture
            golds.push_back(newGold);
            totalGoldValue += newGold.value;
        }
        for (int i = 0; i < 5; ++i) { // Number of stones
            Stone newStone = Stone::createRandomStone(renderer);
            newStone.texture = stoneTexture; // Assign the loaded texture
            stones.push_back(newStone);
            // Stones might not contribute to target score calculation, or could have negative value
        }

        // Calculate target score (e.g., 2/3 of total gold value, minimum 100)
        targetScore = max(100, static_cast<int>(round(totalGoldValue * 0.66)));

        gameState = PLAYING; // Set state to playing
    };


    // --- Main Game Loop ---
    while (!quit) {
        // --- Event Handling ---
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }

            // Handle input based on Game State
            switch (gameState) {
                case MENU:
                    if (e.type == SDL_MOUSEBUTTONDOWN) {
                        int mouseX, mouseY;
                        SDL_GetMouseState(&mouseX, &mouseY);
                        SDL_Point mousePoint = {mouseX, mouseY};

                        if (SDL_PointInRect(&mousePoint, &startButtonRect)) {
                            resetGame(); // Start a new game
                        } else if (SDL_PointInRect(&mousePoint, &instructionsButtonRect)) {
                            gameState = INSTRUCTIONS;
                        }
                    }
                    break;

                case PLAYING:
                    if (e.type == SDL_KEYDOWN) {
                        if (e.key.keysym.sym == SDLK_SPACE) {
                            rope.extend(); // Tell the rope to start extending
                        }
                        // Add Escape key to pause or go back to menu?
                        else if (e.key.keysym.sym == SDLK_ESCAPE) {
                             // gameState = MENU; // Option to return to menu
                             // Mix_PauseMusic(); // Pause music if returning to menu
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
                            resetGame(); // Play again
                        } else if (SDL_PointInRect(&mousePoint, &noButtonRect)) {
                            quit = true; // Quit the game
                        }
                    }
                     // Allow quitting with Escape key too
                    else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                        quit = true;
                    }
                    break;

                case INSTRUCTIONS:
                    if (e.type == SDL_KEYDOWN) {
                        // Any key press or specific keys (Enter, Escape) return to Menu
                        if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_ESCAPE || e.key.keysym.sym == SDLK_SPACE) {
                            gameState = MENU;
                        }
                    }
                     // Allow clicking to return as well?
                    else if (e.type == SDL_MOUSEBUTTONDOWN) {
                         gameState = MENU;
                    }
                    break;
            }
        } // End event polling

        // --- Game Logic Update ---
        if (gameState == PLAYING) {
            if (!gameOver) {
                // Calculate remaining time
                Uint32 elapsedTime = SDL_GetTicks() - gameStartTime;
                remainingSeconds = max(0, gameDurationSeconds - static_cast<int>(elapsedTime / 1000));

                // Check for time up
                if (remainingSeconds == 0) {
                    gameOver = true;
                    // Determine win/loss state based on score vs target
                    gameState = (score >= targetScore) ? WIN_SCREEN : LOSE_SCREEN;
                    // Optional: Stop rope movement immediately?
                     // rope.isSwinging = false; rope.isExtending = false; rope.isRetracting = false;
                }
            }

            // Update the rope (handles swinging, extending, retracting, collisions, scoring)
            if (!gameOver) { // Only update rope if game isn't over
                 rope.update(score, golds, stones);
            }
             // Player object doesn't need updating as it's stationary
        }

        // --- Rendering ---
        // Clear screen (black background as default)
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        // Render based on Game State
        switch (gameState) {
            case MENU:
                playMusic(backgroundMusic); // Ensure music is playing in menu
                if (menuTexture) SDL_RenderCopy(renderer, menuTexture, NULL, NULL);
                if (startButtonTexture) SDL_RenderCopy(renderer, startButtonTexture, NULL, &startButtonRect);
                if (instructionsButtonTexture) SDL_RenderCopy(renderer, instructionsButtonTexture, NULL, &instructionsButtonRect);
                break;

            case PLAYING:
                // Draw background first
                if (backgroundTexture) SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

                // Draw game objects (Gold, Stones)
                for (const auto& gold : golds) gold.render(renderer);
                for (const auto& stone : stones) stone.render(renderer);

                // Draw the Rope (rope renders itself as a line + attached object)
                rope.render(renderer);

                // Draw Player (on top of rope anchor)
                player.render(renderer); // Uses GameObject::render

                // Draw UI Text (Score, Timer, Target)
                if (font) {
                    SDL_Color textColor = {255, 255, 255, 255}; // White text
                    string scoreText = "$" + to_string(score);
                    string timeText = "Time: " + to_string(remainingSeconds);
                    string targetText = "Target: $" + to_string(targetScore);

                    SDL_Surface* scoreSurface = TTF_RenderText_Blended(font, scoreText.c_str(), textColor); // Use Blended for better quality
                    SDL_Surface* timerSurface = TTF_RenderText_Blended(font, timeText.c_str(), textColor);
                    SDL_Surface* targetSurface = TTF_RenderText_Blended(font, targetText.c_str(), textColor);

                    if (scoreSurface) {
                        SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
                        SDL_Rect scoreRect = {20, 10, scoreSurface->w, scoreSurface->h}; // Top-left area
                        SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);
                        SDL_DestroyTexture(scoreTexture); SDL_FreeSurface(scoreSurface);
                    }
                    if (timerSurface) {
                        SDL_Texture* timerTexture = SDL_CreateTextureFromSurface(renderer, timerSurface);
                         // Position Timer top-right
                        SDL_Rect timerRect = {SCREEN_WIDTH - timerSurface->w - 20, 10, timerSurface->w, timerSurface->h};
                        SDL_RenderCopy(renderer, timerTexture, nullptr, &timerRect);
                        SDL_DestroyTexture(timerTexture); SDL_FreeSurface(timerSurface);
                    }
                    if (targetSurface) {
                        SDL_Texture* targetTexture = SDL_CreateTextureFromSurface(renderer, targetSurface);
                         // Position Target below Score
                        SDL_Rect targetRect = {20, 10 + (scoreSurface ? scoreSurface->h : 25) + 5, targetSurface->w, targetSurface->h};
                        SDL_RenderCopy(renderer, targetTexture, nullptr, &targetRect);
                        SDL_DestroyTexture(targetTexture); SDL_FreeSurface(targetSurface);
                    }
                }
                break;

            case WIN_SCREEN:
                 if (winTexture) SDL_RenderCopy(renderer, winTexture, NULL, NULL);
                 if (yesButtonTexture) SDL_RenderCopy(renderer, yesButtonTexture, NULL, &yesButtonRect);
                 if (noButtonTexture) SDL_RenderCopy(renderer, noButtonTexture, NULL, &noButtonRect);
                 // Optional: Display final score on win screen
                 break;

            case LOSE_SCREEN:
                 if (loseTexture) SDL_RenderCopy(renderer, loseTexture, NULL, NULL);
                 if (yesButtonTexture) SDL_RenderCopy(renderer, yesButtonTexture, NULL, &yesButtonRect);
                 if (noButtonTexture) SDL_RenderCopy(renderer, noButtonTexture, NULL, &noButtonRect);
                  // Optional: Display final score on lose screen
                 break;

            case INSTRUCTIONS:
                if (introTexture) SDL_RenderCopy(renderer, introTexture, NULL, NULL);
                // Add text overlay explaining controls
                if (font) {
                    SDL_Color textColor = {255, 255, 255, 255};
                    const char* line1 = "Swing the rope automatically.";
                    const char* line2 = "Press SPACE to drop the hook.";
                    const char* line3 = "Collect gold and avoid stones!";
                    const char* line4 = "Reach the target score before time runs out.";
                    const char* line5 = "Press Enter/Escape/Space or Click to return.";

                    SDL_Surface* surf1 = TTF_RenderText_Blended(font, line1, textColor);
                    SDL_Surface* surf2 = TTF_RenderText_Blended(font, line2, textColor);
                    SDL_Surface* surf3 = TTF_RenderText_Blended(font, line3, textColor);
                    SDL_Surface* surf4 = TTF_RenderText_Blended(font, line4, textColor);
                    SDL_Surface* surf5 = TTF_RenderText_Blended(font, line5, textColor);

                    // Simple centered layout for text
                    int yPos = SCREEN_HEIGHT / 2 - 60;
                    if(surf1) { SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf1); SDL_Rect r = {SCREEN_WIDTH/2 - surf1->w/2, yPos, surf1->w, surf1->h}; SDL_RenderCopy(renderer, tex, NULL, &r); SDL_DestroyTexture(tex); SDL_FreeSurface(surf1); yPos += r.h + 5; }
                    if(surf2) { SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf2); SDL_Rect r = {SCREEN_WIDTH/2 - surf2->w/2, yPos, surf2->w, surf2->h}; SDL_RenderCopy(renderer, tex, NULL, &r); SDL_DestroyTexture(tex); SDL_FreeSurface(surf2); yPos += r.h + 5; }
                    if(surf3) { SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf3); SDL_Rect r = {SCREEN_WIDTH/2 - surf3->w/2, yPos, surf3->w, surf3->h}; SDL_RenderCopy(renderer, tex, NULL, &r); SDL_DestroyTexture(tex); SDL_FreeSurface(surf3); yPos += r.h + 5; }
                    if(surf4) { SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf4); SDL_Rect r = {SCREEN_WIDTH/2 - surf4->w/2, yPos, surf4->w, surf4->h}; SDL_RenderCopy(renderer, tex, NULL, &r); SDL_DestroyTexture(tex); SDL_FreeSurface(surf4); yPos += r.h + 20; } // Extra space before last line
                    if(surf5) { SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf5); SDL_Rect r = {SCREEN_WIDTH/2 - surf5->w/2, yPos, surf5->w, surf5->h}; SDL_RenderCopy(renderer, tex, NULL, &r); SDL_DestroyTexture(tex); SDL_FreeSurface(surf5); }
                }
                break;
        }

        // Update the screen with rendering performed.
        SDL_RenderPresent(renderer);

        // No SDL_Delay needed if VSync is enabled (SDL_RENDERER_PRESENTVSYNC)
        // SDL_Delay(16); // Use if not using VSync for ~60 FPS

    } // End main game loop

    // --- Cleanup ---
    SDL_Log("Cleaning up resources...");

    // Destroy Textures
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

    // Free Music
    if (backgroundMusic) Mix_FreeMusic(backgroundMusic);

    // Close Font
    if (font) TTF_CloseFont(font);

    // Destroy Renderer and Window
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // Quit SDL Subsystems
    Mix_CloseAudio(); // Close mixer before Mix_Quit
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit(); // Quit SDL last

    SDL_Log("Cleanup finished. Exiting.");
    return 0;
}
