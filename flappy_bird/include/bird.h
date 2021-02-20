#include <SDL2/SDL.h>
#include "./pipe.h"
#include "../../neat/include/genome.h"

class Bird {
private:
    Genome* brain;
    SDL_Rect rect;

    float vely = 0;
    bool alive;
    float gravity = 20;

    int w_width;
    int w_height;

public:
    // CONSTRUCTOR
    Bird(int, int, Genome* brain);
    ~Bird();
    
    // GETTERS
    Genome* get_brain();
    float get_vely();
    bool is_alive();

    // METHODS
    void reset(Genome* brain);
    void think(Pipe* pipe);
    void jump();
    bool check_collision(Pipe*);
    void die();
    void update(float);

    void render(SDL_Renderer*, SDL_Texture*);
};