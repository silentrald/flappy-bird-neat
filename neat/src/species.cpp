#include "../include/species.h"

// COMPARE
bool species_compare::operator()(Genome* lhs, Genome* rhs) const {
    int f1 = lhs->get_fitness();
    int f2 = rhs->get_fitness();
    return f1 == f2 ? lhs > rhs : f1 > f2; 
}

// SPECIES
Species::Species(Neat* neat) {
    this->neat = neat;
}

// GETTERS
double Species::get_score() {
    return score;
}

// SETTERS
void Species::set_representative(Genome* representative) {
    this->representative = representative;
    species.insert(representative);
}

// METHODS
bool Species::add_genome(Genome* genome) {
    // If the genome is related to the represtentative
    if (genome->distance(representative) < neat->get_speciation_distance_threshold()) {
        species.insert(genome).second;
        return true;
    }
    return false;
}

int Species::get_species_size() {
    return species.size();
}

void Species::kill(double percentage) {
    std::set<Genome*, species_compare>::iterator it = species.end();
    species.erase(std::prev(it, percentage * species.size()), it);
}

void Species::evaluate_score() {
    score = 0;
    for (auto it = species.begin(); it != species.end(); it++) {
        score += (*it)->get_fitness();
    }
    score /= species.size();
}

Genome* Species::breed() {
    int size = species.size();
    Genome* g1 = *std::next(species.begin(), rand() % size);
    Genome* g2 = *std::next(species.begin(), rand() % size);

    return g1->crossover(g2);
}

void Species::clear() {
    species.clear();
}