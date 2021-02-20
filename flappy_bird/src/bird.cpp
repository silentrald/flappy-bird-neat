#include "../include/bird.h"

// CONSTRUCTORS
Bird::Bird(int width, int height, Genome* brain) {
    this->brain = brain;

    alive = true;
    w_width = width;
    w_height = height;

    // Set
    rect.x = 50;
    rect.y = height / 2 - 20;
    rect.w = 40;
    rect.h = 40;
}

// DECONSTRUCTOR
Bird::~Bird() {
    delete brain;
}

// GETTERS
Genome* Bird::get_brain() {
    return brain;
}

float Bird::get_vely() {
    return vely;
}

bool Bird::is_alive() {
    return alive;
}

// METHODS
void Bird::reset(Genome* brain) {
    this->brain = brain;

    rect.y = w_height / 2 - 10;
    vely = 0;
    alive = true;
}

void Bird::think(Pipe* pipe) {
    double output = brain->calculate({
        (double)(rect.y + rect.h / 2) / w_height,
        (double)pipe->get_top() / w_height,
        (double)pipe->get_bottom() / w_height,
        (double)pipe->get_x() / w_width
    })[0];

    if (output > 0.5) {
        jump();
    }
}

void Bird::jump() {
    if (alive) {
        vely = -8;
    }
}

bool Bird::check_collision(Pipe* pipe) {
    // check x collision
    if (rect.x + rect.w > pipe->get_x() &&
        rect.x < pipe->get_x() + pipe->get_width())
    {
        // top collision
        if (rect.y < pipe->get_top()) {
            return true;
        }
        // bottom collision
        if (rect.y + rect.h > pipe->get_bottom()) {
            return true;
        }
    }
    return false;
}

void Bird::die() {
    vely = 1;
    alive = false;
}

void Bird::update(float delta) {
    // Accelaration update
    vely += gravity * delta;

    // Update Y val
    rect.y += vely;

    brain->add_fitness(1.0);
    
    // bird hits the ceiling
    if (rect.y < 0) {
        rect.y = 0;
        vely = 1;
    }

    // bird hits the ground
    if (rect.y > w_height - 20) {
        rect.y = w_height - 20;
        vely = 0;
        alive = false;
    }

    // bird dies at the ceiling
    if (rect.y < 5) {
        rect.y = 5;
        vely = 0;
        alive = false;
    }
}

void Bird::render(SDL_Renderer* renderer, SDL_Texture* texture) {
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}