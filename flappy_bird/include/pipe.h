#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>

class Pipe {
private:
    float velx;
    int x;
    int width;
    int top;
    int bottom;
    SDL_Rect top_pipe;
    SDL_Rect bottom_pipe;

public:
    // CONSTRUCTOR
    Pipe(float, int, int, int, int);

    // GETTERS
    int get_x();
    int get_width();
    int get_top();
    int get_bottom();

    // METHODS
    bool is_outside();
    void update(float);

    // DRAW
    void render(SDL_Renderer*, SDL_Texture*);
};