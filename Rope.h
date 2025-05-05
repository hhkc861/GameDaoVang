#ifndef ROPE_H
#define ROPE_H

#include "GameObject.h" // Base class
#include "Gold.h"       // Needs to know about Gold
#include "Stone.h"      // Needs to know about Stone
#include <SDL.h>
#include <vector>       // Needed for function parameters (golds, stones)

// Forward declare vector if only used in function signatures in header
// #include <vector> // Include if using vector members, otherwise forward declare is better for compile times
// class Gold; // Forward declaration
// class Stone; // Forward declaration
// template<typename T> class std::vector; // Forward declaration of vector template

// using namespace std; // Avoid in headers

class Rope : public GameObject {
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
    int startX, startY;          // Anchor point coordinates (usually player's position)

    // Constructor: Takes anchor point and max length. No texture needed.
    Rope(int anchorX, int anchorY, int maxLen);

    // Methods
    void setAnchor(int x, int y); // Update anchor point if player moved (not needed now)
    void extend();                // Start extending the rope at the current angle
    void retract();               // Start retracting the rope (usually called internally)

    // Update handles all logic: swinging, extending, retracting, collision, collection
    // Needs score and object vectors to modify them upon successful retraction
    void update(int& score, std::vector<Gold>& golds, std::vector<Stone>& stones);

    // Override render to draw the rope line and attached object
    void render(SDL_Renderer* renderer) const override;

    // Collision checks (called internally by update)
    Gold* checkCollision(std::vector<Gold>& golds);
    Stone* checkCollision(std::vector<Stone>& stones);
};

#endif // ROPE_H
