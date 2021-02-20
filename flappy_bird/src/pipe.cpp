#include "../include/pipe.h"
#include <iostream>

// CONSTRUCTOR
Pipe::Pipe(float velx, int gap, int width, int w_width, int w_height) {
    this->velx = velx;

    x = w_width;
    this->width = width;

    // Calculate top and bottom colission
    top = rand() % (w_height - gap) + 10;
    bottom = top + gap;

    // Set the rects for both pipes
    top_pipe.x = x;
    top_pipe.w = width;
    top_pipe.y = top - w_height;
    top_pipe.h = w_height;

    bottom_pipe.x = x;
    bottom_pipe.w = width;
    bottom_pipe.y = bottom;
    bottom_pipe.h = w_height;
}

// GETTERS
int Pipe::get_x() {
    return x;
}

int Pipe::get_width() {
    return width;
}

int Pipe::get_top() {
    return top;
}

int Pipe::get_bottom() {
    return bottom;
}

// METHODS
bool Pipe::is_outside() {
    return x < -40;
}

void Pipe::update(float delta) {
    x -= velx;

    top_pipe.x = x;
    bottom_pipe.x = x;
}

// DRAW
void Pipe::render(SDL_Renderer* renderer, SDL_Texture* texture) {
    SDL_RenderCopyEx(renderer, texture, NULL, &top_pipe, 0, NULL, SDL_FLIP_VERTICAL);
    SDL_RenderCopy(renderer, texture, NULL, &bottom_pipe);
}