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

const int S_WIDTH = WIDTH / 2;
const int S_HEIGHT = HEIGHT / 2;

int scale_x(double x) {
    return x * S_WIDTH + S_WIDTH;
}

int scale_y(double y) {
    return y * S_HEIGHT;
}

void render_brain(SDL_Renderer* renderer, Genome* brain) {
    Connection* connection;
    Node* from;
    Node* to;
    Node* node;

    std::map<int, Connection*>::iterator connection_start = brain->get_connections_begin();
    std::map<int, Connection*>::iterator connection_end = brain->get_connections_end();
    std::map<int, Node*>::iterator node_start = brain->get_nodes_begin();
    std::map<int, Node*>::iterator node_end = brain->get_nodes_end();

    // Draw all connections
    while (connection_start != connection_end) {
        connection = connection_start->second;
        from = connection->get_from();
        to = connection->get_to();

        if (connection->is_enabled()) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 192); // green
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 192); // red
        }

        SDL_RenderDrawLine(
            renderer,
            scale_x(from->get_x()),
            scale_y(from->get_y()),
            scale_x(to->get_x()),
            scale_y(to->get_y())
        );

        connection_start++;
    }

    SDL_SetRenderDrawColor(renderer, 64, 64, 64, 192);

    while (node_start != node_end) {
        node = node_start->second;

        SDL_Rect rect;
        rect.x = scale_x(node->get_x()) - 5;
        rect.y = scale_y(node->get_y()) - 5;
        rect.w = 10;
        rect.h = 10;
        
        SDL_RenderFillRect(renderer, &rect);

        node_start++;
    }
}

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

    // Init system
    bool running = true;
    const int FPS = 60;
    const int SPF = 1000 / FPS;
    Uint32 start;

    bool KEYS[322];

    for(int i = 0; i < 322; i++) {
        KEYS[i] = false;
    }

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

    SDL_Rect bg1, bg2;
    bg1.x = 0;
    bg1.y = 0;
    bg1.w = 1940 / 2;
    bg1.h = 1000 / 2;

    bg2.x = 1940 / 2;
    bg2.y = 0;
    bg2.w = 1940 / 2;
    bg2.h = 1000 / 2;

    // Texture
    SDL_Surface* tmp = IMG_Load("./img/pipe.png");
    SDL_Texture* pipe_texture = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);

    tmp = IMG_Load("./img/macky_idle.png");
    SDL_Texture* idle_texture = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);

    tmp = IMG_Load("./img/macky_flap.png");
    SDL_Texture* flap_texture = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);

    tmp = IMG_Load("./img/bg.png");
    SDL_Texture* bg_texture = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);
    SDL_SetTextureAlphaMod(bg_texture, 225);

    Bird* bird;
    Genome* brain;
    int target_bird = 0;
    bool right_up = true;
    bool left_up = true;

    while(running) {
        start = SDL_GetTicks();
        SDL_Event event;

        while (SDL_PollEvent(&event)) { 
            int key;

            switch (event.type) {
                case SDL_QUIT:
                    running = false; 
                    break;
                case SDL_KEYDOWN:
                    key = event.key.keysym.scancode;
                    if (key < 322) KEYS[key] = true;
                    break;
                case SDL_KEYUP:
                    key = event.key.keysym.scancode;
                    if (key < 322) KEYS[key] = false;
                    break;
            }
        }

        // QUIT
        if (KEYS[SDL_SCANCODE_Q]) {
            running = false;
            break;
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

                    bg1.x = 0;
                    bg2.x = 1940 / 2;
                    std::cout << "START: GEN " << generation << ", " << score << "\n";
                }
            }
        }

        // Clears the screen with a blue tint
        SDL_SetRenderDrawColor(renderer, 173, 216, 230, 255);
        SDL_RenderClear(renderer);
        
        // BG SCROLL
        bg1.x--; bg2.x--;

        if (bg1.x + bg1.w < 0) {
            bg1.x = bg2.x + bg2.w;
        }

        if (bg2.x + bg2.w < 0) {
            bg2.x = bg1.x + bg1.w;
        }

        SDL_RenderCopy(renderer, bg_texture, NULL, &bg1);
        SDL_RenderCopy(renderer, bg_texture, NULL, &bg2);

        // Render the pipes
        for (int i = pipes.size() - 1; i > -1; i--) {
            pipes[i]->render(renderer, pipe_texture);
        }

        // Update target bird
        if (KEYS[SDL_SCANCODE_RIGHT] && right_up) {
            target_bird++;
            right_up = false;
        } else if (!KEYS[SDL_SCANCODE_RIGHT] && !right_up) {
            right_up = true;
        }

        if (KEYS[SDL_SCANCODE_LEFT] && left_up) {
            target_bird--;
            left_up = false;
        } else if (!KEYS[SDL_SCANCODE_LEFT] && !left_up) {
            left_up = true;
        }

        if (target_bird < 0) {
            target_bird = alive_birds.size() - 1;
        } else if (target_bird >= alive_birds.size()) {
            target_bird = 0;
        }

        // Render Birds
        // Lower opacity except for one bird
        SDL_SetTextureAlphaMod(idle_texture, 64);
        SDL_SetTextureAlphaMod(flap_texture, 64);

        for (int i = alive_birds.size() - 1; i > -1; i--) {
            if (target_bird != i) {
                bird = alive_birds[i];
                bird->render(renderer, bird->get_vely() > 0.0f ? idle_texture : flap_texture);
            }
        }

        // Highlight 1 bird
        SDL_SetTextureAlphaMod(idle_texture, 255);
        SDL_SetTextureAlphaMod(flap_texture, 255);

        bird = alive_birds[target_bird];
        bird->render(renderer, bird->get_vely() > 0.0f ? idle_texture : flap_texture);
        brain = bird->get_brain();

        render_brain(renderer, brain);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderPresent(renderer);

        if (KEYS[SDL_SCANCODE_S]) {
            neat.to_file();
            brain->to_file();
        }

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