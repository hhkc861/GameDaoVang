#ifndef GOLD_H
#define GOLD_H

#include "GameObject.h"
#include <SDL.h>
#include <vector> // Include vector here if used in declarations (not needed now)

// using namespace std; // Avoid 'using namespace std;' in header files

class Gold : public GameObject {
public:
    int value;

    Gold(SDL_Texture* texture, int x, int y, int w, int h);
    static Gold createRandomGold(SDL_Renderer* renderer); // Pass renderer if needed for texture loading inside, but not strictly necessary for just coords/size

    // Override render to handle potential null texture (though main assigns it now)
    void render(SDL_Renderer* renderer) const override;
};

#endif // GOLD_H
