#ifndef ROPE_H
#define ROPE_H

#include "GameObject.h" // Base class
#include "Gold.h"       // Needs to know about Gold
#include "Stone.h"      // Needs to know about Stone
#include <SDL.h>
#include <vector>       // Needed for function parameters (golds, stones)

class Rope : public GameObject { // Inherits from GameObject
public:
    int length;
    int maxLength;
    int extendSpeed;
    int retractSpeedFactorGold;  // Amount to reduce speed by when carrying gold
    int retractSpeedFactorStone; // Amount to reduce speed by when carrying stone
    bool isExtending;
    bool isRetracting;
    Gold* attachedGold;          // Pointer to the gold being carried (nullptr if none)
    Stone* attachedStone;        // Pointer to the stone being carried (nullptr if none)

    // Swinging mechanics
    double angle;                // Current angle in radians (0 is straight down)
    double angleSpeed;           // Angular velocity (radians per update)
    double maxAngle;             // Maximum swing angle (radians) from vertical
    bool isSwinging;             // True if currently swinging, false if extending/retracting
    int startX, startY;          // Anchor point coordinates (center of rotation)

    // Visual property
    int thickness; // Store the thickness for rendering calculation

    // !!!!! THIS IS THE CORRECT 5-ARGUMENT CONSTRUCTOR DECLARATION !!!!!
    Rope(SDL_Texture* tex, int thick, int anchorX, int anchorY, int maxLen);

    // Methods
    void setAnchor(int x, int y); // Update anchor point
    void extend();                // Start extending the rope at the current angle
    void retract();               // Start retracting the rope (usually called internally)

    // Update handles all logic: swinging, extending, retracting, collision, collection
    void update(int& score, std::vector<Gold>& golds, std::vector<Stone>& stones);

    // Override render to draw the rope using the texture and thickness
    void render(SDL_Renderer* renderer) const override;

    // Collision checks (called internally by update)
    Gold* checkCollision(std::vector<Gold>& golds);
    Stone* checkCollision(std::vector<Stone>& stones);
};

#endif // ROPE_H
