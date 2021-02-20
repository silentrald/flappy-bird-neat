#ifndef _SPECIES_H
#define _SPECIES_H

#include <set>

class Neat;
#include "./neat.h"

struct species_compare { 
    bool operator()(Genome* lhs, Genome* rhs) const;
};

class Species {
private:
    Neat* neat;
    
    std::set<Genome*, species_compare> species;
    Genome* representative;
    double score;

public:
    Species(Neat* neat);

    // GETTERS
    double get_score();

    // SETTERS
    void set_representative(Genome* representative);
    void set_score(double score);

    // METHODS
    bool add_genome(Genome* genome);
    int get_species_size();

    void kill(double percentage);
    void evaluate_score();
    Genome* breed();

    void clear();
};

#endif