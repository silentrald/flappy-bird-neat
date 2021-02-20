#ifndef _GENOME_H
#define _GENOME_H

#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include <stdio.h>

#include "./connection.h"
#include "./link_set.h"

class Neat;
#include "./neat.h"

struct hash_pair { 
    template <class T1, class T2> 
    size_t operator()(const std::pair<T1, T2>& p) const {
        return std::hash<T1>{}(p.first) ^ std::hash<T2>{}(p.second); 
    }
};

class Genome {
private:
    std::map<int, Connection*> connections;
    // first n-input nodes and n-output nodes
    std::map<int, Node*> nodes;
    std::unordered_map<std::pair<int, int>, int, hash_pair> markings;
    link_set<std::pair<int, int>, hash_pair> calculation_order;

    Neat* neat;

    double fitness;

    void add_connection_to_child(Genome* child, Connection* connection);
    Connection* random_connection();

public:
    // CONSTRUCTORS
    // Genome();
    Genome(Neat* neat);
    ~Genome();

    // GETTERS
    double get_fitness();
    std::map<int, Connection*> get_connections();
    std::map<int, Node*> get_nodes();

    // SETTERS
    void set_fitness(double fitness);

    // ITERATORS, For looping
    std::map<int, Connection*>::iterator get_connections_begin();
    std::map<int, Connection*>::iterator get_connections_end();
    std::map<int, Node*>::iterator get_nodes_begin();
    std::map<int, Node*>::iterator get_nodes_end();

    // METHODS
    void add_connection(std::pair<int, int> pair, int key, Connection* connection);
    void add_connection(std::pair<int, int> pair, Connection* connection);
    void add_connection(int key, Connection* connection);
    void add_node(int key, Node* node);

    void push_calculation_order(const std::pair<int, int>& pair);

    int connections_size();
    int nodes_size();

    double distance(Genome* other);
    Genome* crossover(Genome* other);

    std::vector<double> calculate(const std::vector<double>& input_vector);

    void add_fitness(double inc);

    // MUTATIONS
    void mutate();
    void mutate_link();
    void mutate_node();
    void mutate_weight_shift();
    void mutate_weight_random();
    void mutate_toggle_link();

    void print();
    void to_file();
};

#endif