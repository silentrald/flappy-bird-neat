#ifndef _NEAT_H
#define _NEAT_H

#include <map>
#include <unordered_map>
#include <unordered_set>
#include <stdio.h>

class Genome;
#include "./genome.h"

class Species;
#include "./species.h"

class Neat {
private:
    int input;
    int output;
    int population;

    std::unordered_map<int, Connection*> all_connections;
    std::unordered_map<int, Node*> all_nodes;
    std::map<std::pair<int, int>, int> all_markings;

    std::unordered_map<int, Species*> all_species;
    std::unordered_set<Genome*> genomes;

    double death_rate = 0.20;
    double excess_coefficient = 0.20;
    double disjoint_coefficient = 0.20;
    double weight_diff_coefficient = 0.20;
    double weight_shift_strength = 0.8;
    double weight_random_strength = 1.0;
    double probability_mutate_link = 0.30;
    double probability_mutate_node = 0.10;
    double probability_mutate_weight_shift = 0.80;
    double probability_mutate_weight_random = 0.40;
    double probability_mutate_toggle_link = 0.01;
    double speciation_distance_threshold = 1.0;

    // EVOLUTION
    void generate_species();
    void kill();
    void reproduce();
    void mutate();

public:
    // CONSTRUCTORS
    Neat();
    Neat(int input, int output);
    ~Neat(); // DECONSTRUCTOR

    // GETTERS
    int get_input();
    int get_output();
    int get_population();
    double get_excess_coefficient();
    double get_disjoint_coefficient();
    double get_weight_diff_coefficient();
    double get_weight_shift_strength();
    double get_weight_random_strength();
    double get_probability_mutate_link();
    double get_probability_mutate_node();
    double get_probability_mutate_weight_shift();
    double get_probability_mutate_weight_random();
    double get_probability_mutate_toggle_link();
    double get_speciation_distance_threshold();

    // ITERATORS
    std::unordered_set<Genome*>::iterator get_genomes_begin();
    std::unordered_set<Genome*>::iterator get_genomes_end();

    // METHODS
    void genesis(int population = 500);
    void reset(int input, int output);
    Connection* get_connection(Connection* connection);
    Connection* get_connection(Node* from, Node* to);
    Node* get_node();
    Node* get_node(int id);

    void evolve();

    void to_file(std::string filename = "neat.txt");
    void load_file(std::string filename = "neat.txt");
};

#endif