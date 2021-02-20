#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>
#include <time.h>
#include <string>
#include <deque>

#include "./neat/include/neat.h"
#include "./flappy_bird/include/bird.h"
// #include "./flappy_bird/include/pipe.h"

const int POPULATION = 500;
const int WIDTH = 640;
const int HEIGHT = 480; 

int main() {
    srand(time(NULL));
    srand48(time(NULL));
    
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) { 
        printf("error initializing SDL: %s\n", SDL_GetError()); 
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("NEAT",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH,
        HEIGHT,
        SDL_WINDOW_OPENGL
    );

    // triggers the program that controls 
    // your graphics hardware and sets flags
    Uint32 render_flags = SDL_RENDERER_ACCELERATED; 
  
    // creates a renderer to render our images 
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, render_flags); 

    bool running = true;
    const int FPS = 60;
    const int SPF = 1000 / FPS;
    Uint32 start;

    int generation = 1, score = 0;
    std::vector<Bird*> alive_birds;
    std::vector<Bird*> dead_birds;
    std::deque<Pipe*> pipes;

    // Create Neat
    Neat neat(4, 1);
    neat.genesis(POPULATION);

    // Init the birds
    auto end = neat.get_genomes_end();
    for (auto it = neat.get_genomes_begin(); it != end; it++) {
        alive_birds.push_back(new Bird(WIDTH, HEIGHT, *it));
    }

    int spawn_pipe_timer = 2250;
    pipes.push_back(new Pipe(3, 200, 60, WIDTH, HEIGHT));

    // Texture
    SDL_Surface* tmp = IMG_Load("./flappy_bird/img/pipe.png");
    SDL_Texture* pipe_texture = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);

    tmp = IMG_Load("./flappy_bird/img/macky_idle.png");
    SDL_Texture* idle_texture = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);

    tmp = IMG_Load("./flappy_bird/img/macky_flap.png");
    SDL_Texture* flap_texture = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);

    Bird* bird;

    while(running) {
        start = SDL_GetTicks();
        SDL_Event event;

        while (SDL_PollEvent(&event)) { 
            int key;
            switch (event.type) {
                case SDL_QUIT:
                    running = false; 
                    break;
            }
        }

        // Update values
        spawn_pipe_timer -= SPF;
        if (spawn_pipe_timer <= 0) {
            pipes.push_back(new Pipe(3, 200, 60, WIDTH, HEIGHT));
            spawn_pipe_timer += 2250;
        }

        for (int i = pipes.size() - 1; i > -1; i--) {
            pipes[i]->update(SPF / 1000.0);
        }

        if (pipes.front()->is_outside()) {
            score++;
            std::cout << "GEN " << generation << ", " << score << "\n";
            delete pipes.front();
            pipes.pop_front();
        }

        for (int i = alive_birds.size() - 1; i > -1; i--) {
            Bird* bird = alive_birds[i];
            bird->think(pipes[0]);
            bird->update(SPF / 1000.0f);

            // Collision
            if (!bird->is_alive() || bird->check_collision(pipes[0])) {
                dead_birds.push_back(bird);
                alive_birds.erase(alive_birds.begin() + i);

                if (alive_birds.empty()) {
                    neat.evolve();
                    alive_birds = dead_birds;

                    end = neat.get_genomes_end();
                    int i = 0;
                    for (auto it = neat.get_genomes_begin(); it != end; it++, i++) {
                        alive_birds[i]->reset(*it);
                    }

                    dead_birds.clear();
                    pipes.clear();

                    pipes.push_back(new Pipe(3, 200, 60, WIDTH, HEIGHT));
                    spawn_pipe_timer = 2250;
                    generation++;
                    score = 0;
                    std::cout << "START: GEN " << generation << ", " << score << "\n";
                }
            }
        }

        // Clears the screen with a blue tint
        SDL_SetRenderDrawColor(renderer, 173, 216, 230, 255);
        SDL_RenderClear(renderer);

        // Render the pipes
        for (int i = pipes.size() - 1; i > -1; i--) {
            pipes[i]->render(renderer, pipe_texture);
        }

        // Render Birds
        // Lower opacity except for one bird
        SDL_SetTextureAlphaMod(idle_texture, 64);
        SDL_SetTextureAlphaMod(flap_texture, 64);

        for (int i = alive_birds.size() - 1; i > 0; i--) {
            bird = alive_birds[i];
            bird->render(renderer, bird->get_vely() > 0.0f ? idle_texture : flap_texture);
        }

        // Highlight 1 bird
        SDL_SetTextureAlphaMod(idle_texture, 255);
        SDL_SetTextureAlphaMod(flap_texture, 255);

        bird = alive_birds[0];
        bird->render(renderer, bird->get_vely() > 0.0f ? idle_texture : flap_texture);

        // TODO: Render that birds brain

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderPresent(renderer);

        // FPS Regulator
        if (SPF > SDL_GetTicks() - start) {
            SDL_Delay(SPF - (SDL_GetTicks() - start));
        }
    }

    SDL_DestroyTexture(pipe_texture);
    SDL_DestroyTexture(idle_texture);
    SDL_DestroyTexture(flap_texture);

    SDL_DestroyWindow(window);
    SDL_Quit();
}