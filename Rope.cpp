#include "Rope.h"
#include "constants.h" // For SCREEN_HEIGHT, SCREEN_WIDTH
#include <cmath>       // For sin, cos, M_PI, abs, min, max
#include <vector>      // For vector parameters
#include <algorithm>   // For std::remove_if, std::min, std::max
#include <iostream>    // For debugging (optional)

#ifndef M_PI // Define PI if not already defined by <cmath>
#define M_PI 3.14159265358979323846
#endif

// Constructor implementation
Rope::Rope(SDL_Texture* tex, int thick, int anchorX, int anchorY, int maxLen)
    : GameObject(nullptr, anchorX, anchorY, 1, 0), // Base constructor: null texture, anchor point, width 1, initial height 0
      length(0),
      maxLength(maxLen),
      extendSpeed(8),                 // Speed at which rope extends
      retractSpeedFactorGold(3),      // Retract speed = extendSpeed - factor (min 1)
      retractSpeedFactorStone(5),     // Stone slows retraction more
      isExtending(false),
      isRetracting(false),
      attachedGold(nullptr),
      attachedStone(nullptr),
      angle(0.0),                     // Start angle (straight down)
      angleSpeed(0.025),              // How fast it swings (adjust for desired speed)
      maxAngle(M_PI / 3.5),           // Max swing angle (e.g., ~51 degrees)
      isSwinging(true),               // Start in swinging state
      startX(anchorX),
      startY(anchorY)
{
    // Ensure rect reflects the anchor point (used by base class render if it had a texture)
    rect.x = startX;
    rect.y = startY;
}

// Method to update the anchor point (if player could move)
void Rope::setAnchor(int x, int y) {
    startX = x;
    startY = y;
    rect.x = x; // Update base rect if needed
    rect.y = y;
}

// Method to initiate rope extension
void Rope::extend() {
    // Can only extend if currently swinging and not already extending/retracting
    if (isSwinging && !isExtending && !isRetracting) {
        isExtending = true;
        isRetracting = false;
        isSwinging = false; // Stop swinging
        length = 0;         // Start extension from length 0
        // The current 'angle' is used for the extension direction
    }
}

// Method to initiate rope retraction (usually called internally)
void Rope::retract() {
    isRetracting = true;
    isExtending = false;
    isSwinging = false; // Ensure not swinging while retracting
}

// Main update logic for the rope
void Rope::update(int& score, std::vector<Gold>& golds, std::vector<Stone>& stones) {
    if (isSwinging) {
        // Update angle for swinging motion
        angle += angleSpeed;
        // Reverse direction if max angle is reached
        if (angle > maxAngle || angle < -maxAngle) {
            angleSpeed = -angleSpeed;
            // Clamp angle to prevent exceeding maxAngle due to discrete steps
            angle = std::max(-maxAngle, std::min(angle, maxAngle));
        }
        // Reset state variables that shouldn't persist while swinging
        length = 0;
        attachedGold = nullptr;
        attachedStone = nullptr;

    } else if (isExtending) {
        // Increase rope length
        length += extendSpeed;

        // Calculate current end point
        int endX = startX + static_cast<int>(length * sin(angle));
        int endY = startY + static_cast<int>(length * cos(angle));

        // Check for boundary collision or max length reached
        bool hitBoundary = (endY >= SCREEN_HEIGHT || endX <= 0 || endX >= SCREEN_WIDTH || length >= maxLength);

        if (hitBoundary) {
            // Adjust length precisely to the boundary hit point (optional, but cleaner)
            if (length >= maxLength) length = maxLength;
            // More precise boundary adjustments (requires careful calculation if needed)
            // else if (endY >= SCREEN_HEIGHT) length = ...
            // else if (endX <= 0) length = ...
            // else if (endX >= SCREEN_WIDTH) length = ...

            retract(); // Hit something or reached max length, start retracting
        } else {
            // If still extending within bounds, check for object collision
            // Only check if we haven't already grabbed something on this extension
            if (attachedGold == nullptr && attachedStone == nullptr) {
                attachedGold = checkCollision(golds);
                if (attachedGold != nullptr) {
                    retract(); // Found gold, start retracting
                } else {
                    // Only check for stone if no gold was found
                    attachedStone = checkCollision(stones);
                    if (attachedStone != nullptr) {
                        retract(); // Found stone, start retracting
                    }
                }
            }
        }

    } else if (isRetracting) {
        // Calculate retraction speed based on attached object
        int currentRetractSpeed = extendSpeed; // Start with base speed
        if (attachedGold != nullptr) {
            currentRetractSpeed -= retractSpeedFactorGold;
        } else if (attachedStone != nullptr) {
            currentRetractSpeed -= retractSpeedFactorStone;
        }
        currentRetractSpeed = std::max(1, currentRetractSpeed); // Ensure speed is at least 1

        // Decrease rope length
        length -= currentRetractSpeed;

        // Calculate end point to position the attached object
        int endX = startX + static_cast<int>(length * sin(angle));
        int endY = startY + static_cast<int>(length * cos(angle));

        // Update the position of the attached object to follow the rope end
        if (attachedGold != nullptr) {
            // Center the object on the rope end
            attachedGold->rect.x = endX - attachedGold->rect.w / 2;
            attachedGold->rect.y = endY - attachedGold->rect.h / 2;
        }
        if (attachedStone != nullptr) {
            // Center the object on the rope end
            attachedStone->rect.x = endX - attachedStone->rect.w / 2;
            attachedStone->rect.y = endY - attachedStone->rect.h / 2;
        }

        // Check if rope is fully retracted
        if (length <= 0) {
            length = 0;
            isRetracting = false;

            // Process collected object
            if (attachedGold != nullptr) {
                score += attachedGold->value; // Add value to score
                // Remove the collected gold object from the main vector
                golds.erase(std::remove_if(golds.begin(), golds.end(),
                                       [&](const Gold& g) { return &g == attachedGold; }),
                            golds.end());
                attachedGold = nullptr; // Release the pointer
            } else if (attachedStone != nullptr) {
                score += attachedStone->value; // Add value to score
                // Remove the collected stone object from the main vector
                stones.erase(std::remove_if(stones.begin(), stones.end(),
                                        [&](const Stone& s) { return &s == attachedStone; }),
                             stones.end());
                attachedStone = nullptr; // Release the pointer
            }

            // Finished retracting, start swinging again
            isSwinging = true;
            // Keep the current angle and angleSpeed for the next swing cycle
        }
    }
}

// Render the rope and any attached object
void Rope::render(SDL_Renderer* renderer) const {
    // Determine the length to draw
    int drawLength = length;
    if (isSwinging) {
        // Optionally draw a small stub or visual indicator even when length is 0
         drawLength = 20; // Example: Draw a short line of length 20 while swinging
    }

    // Calculate the end point based on angle and drawLength
    int endX = startX + static_cast<int>(drawLength * sin(angle));
    int endY = startY + static_cast<int>(drawLength * cos(angle));

    // Set color for the rope line (e.g., brown)
    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); // Brown color

    // Draw the line from anchor (startX, startY) to (endX, endY)
    SDL_RenderDrawLine(renderer, startX, startY, endX, endY);

    // Render the attached object if there is one
    // Its position should already be updated in the update() method
    if (attachedGold != nullptr) {
        attachedGold->render(renderer);
    }
    if (attachedStone != nullptr) {
        attachedStone->render(renderer);
    }
}

// Check for collision between the rope tip and gold objects
Gold* Rope::checkCollision(std::vector<Gold>& golds) {
    // Collision only makes sense when extending
    if (!isExtending) return nullptr;

    // Calculate the rope's current end point
    int endX = startX + static_cast<int>(length * sin(angle));
    int endY = startY + static_cast<int>(length * cos(angle));

    // Define a small rectangle around the rope tip for collision detection
    // Adjust size (e.g., 10x10) as needed for sensitivity
    SDL_Rect ropeTipRect = {endX - 5, endY - 5, 10, 10};

    // Iterate through gold objects and check for intersection
    for (Gold& gold : golds) {
        SDL_Rect intersection; // Output rect for intersection details (optional)
        // Check if the rope tip rectangle overlaps with the gold's rectangle
        if (SDL_IntersectRect(&ropeTipRect, &gold.rect, &intersection)) {
            return &gold; // Return a pointer to the collided gold object
        }
    }

    return nullptr; // No collision detected
}

// Check for collision between the rope tip and stone objects
Stone* Rope::checkCollision(std::vector<Stone>& stones) {
    // Collision only makes sense when extending
    if (!isExtending) return nullptr;

    // Calculate the rope's current end point
    int endX = startX + static_cast<int>(length * sin(angle));
    int endY = startY + static_cast<int>(length * cos(angle));

    // Define a small rectangle around the rope tip
    SDL_Rect ropeTipRect = {endX - 5, endY - 5, 10, 10};

    // Iterate through stone objects and check for intersection
    for (Stone& stone : stones) {
        SDL_Rect intersection;
        if (SDL_IntersectRect(&ropeTipRect, &stone.rect, &intersection)) {
            return &stone; // Return a pointer to the collided stone object
        }
    }

    return nullptr; // No collision detected
}
