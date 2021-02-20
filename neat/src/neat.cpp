#include "../include/neat.h"

Neat::Neat() {}

Neat::Neat(int input, int output) {
    reset(input, output);
}

Neat::~Neat() {
    for (auto it = all_connections.begin(); it != all_connections.end(); it++) {
        delete it->second;
    }

    for (auto it = all_nodes.begin(); it != all_nodes.end(); it++) {
        delete it->second;
    }

    // for (auto it = alive.begin(); it != alive.end(); it++) {
    //     delete *it;
    // }
}

// GETTERS
int Neat::get_input() {
    return input;
}

int Neat::get_output() {
    return output;
}

double Neat::get_excess_coefficient() {
    return excess_coefficient;
}

double Neat::get_disjoint_coefficient() {
    return disjoint_coefficient;
}

double Neat::get_weight_diff_coefficient() {
    return weight_diff_coefficient;
}

double Neat::get_weight_shift_strength() {
    return weight_shift_strength;
}

double Neat::get_weight_random_strength() {
    return weight_random_strength;
}

double Neat::get_probability_mutate_link() {
    return probability_mutate_link;
}

double Neat::get_probability_mutate_node() {
    return probability_mutate_node;
}

double Neat::get_probability_mutate_weight_shift() {
    return probability_mutate_weight_shift;
}

double Neat::get_probability_mutate_weight_random() {
    return probability_mutate_weight_random;
}

double Neat::get_probability_mutate_toggle_link() {
    return probability_mutate_toggle_link;
}

double Neat::get_speciation_distance_threshold() {
    return speciation_distance_threshold;
}

// ITERATORS
std::unordered_set<Genome*>::iterator Neat::get_genomes_begin() {
    return genomes.begin();
}

std::unordered_set<Genome*>::iterator Neat::get_genomes_end() {
    return genomes.end();
}

// METHODS
void Neat::genesis(int population) {
    if (population < 1) {
        population = 1;
    }

    this->population = population;
    Genome* genome;
    int temp = input + output;

    for (int i = 0; i < population; i++) {
        genome = new Genome(this);

        for (int i = 0; i < input; i++) {
            genome->add_node(i, get_node(i));
        }

        for (int i = input; i < temp; i++) {
            genome->add_node(i, get_node(i));
            genome->push_calculation_order(std::make_pair(i, i));
        }

        genome->mutate_link();
        genomes.insert(genome);
    }
}

void Neat::reset(int input, int output) {
    this->input = input;
    this->output = output;

    all_connections.clear();
    all_nodes.clear();

    for (int in = 0; in < input; in++) {
        Node* node = get_node();
        node->set_x(0.1); // LEFT
        node->set_y((in + 1) / (double) (input + 1));
    }

    for (int out = 0; out < output; out++) {
        Node* node = get_node();
        node->set_x(0.9); // RIGHT
        node->set_y((out + 1) / (double) (output + 1));
    }
}

Connection* Neat::get_connection(Connection* connection) {
    return new Connection(connection);
}

Connection* Neat::get_connection(Node* from, Node* to) {
    Connection* connection = new Connection(from, to);

    std::pair<int, int> key = connection->get_innovation_pair();
    int innovation_number;
    if (all_markings.find(key) == all_markings.end()) {
        innovation_number = all_connections.size();
        connection->set_innovation_number(innovation_number);

        all_markings[key] = innovation_number;
        all_connections[innovation_number] = connection;

        connection = get_connection(connection);
    } else {
        innovation_number = all_markings[key];
        connection->set_innovation_number(innovation_number);
    }

    return connection;
}

Node* Neat::get_node() {
    int innovation_number = all_nodes.size();
    Node* node = new Node(innovation_number);
    all_nodes[innovation_number] = node;
    return node;
}

Node* Neat::get_node(int id) {
    if (id < all_nodes.size()) {
        return all_nodes[id];
    }
    return get_node();
}

// EVOLUTION
void Neat::evolve() {
    generate_species();
    kill();
    reproduce();
    mutate();
}

void Neat::generate_species() {
    all_species.clear();

    Genome* genome;
    bool inserted;

    for (auto it = genomes.begin(); it != genomes.end(); it++) {
        genome = *it;
        inserted = false;

        for (auto species_it = all_species.begin(); species_it != all_species.end(); species_it++) {
            if (species_it->second->add_genome(genome)) {
                inserted = true;
                break;
            }
        }

        if (!inserted) {
            Species* species = new Species(this);
            species->set_representative(genome);
            all_species[all_species.size()] = species;
        }
    }

    for (auto it = all_species.begin(); it != all_species.end(); it++) {
        it->second->evaluate_score();
    }
}

void Neat::kill() {
    Species* species;
    std::unordered_map<int, Species*>::iterator it = all_species.begin();
    std::unordered_map<int, Species*>::iterator del;
    
    while (it != all_species.end()) {
        species = it->second;
        species->kill(death_rate);

        if (species->get_species_size() < 2) {
            delete species;

            del = it;
            it++;
            all_species.erase(del);

            continue;
        }

        it++;
    }
}

void Neat::reproduce() {
    int size = all_species.size();
    int children = population / size;
    int remaining = population - children * size;
    std::unordered_set<Genome*> new_genomes;

    for (auto it = all_species.begin(); it != all_species.end(); it++) {
        for (int i = 0; i < children; i++) {
            new_genomes.insert(it->second->breed());
        }
    }

    // Insert all the alive genomes to a specie
    for (int i = 0; i < remaining; i++) {
        new_genomes.insert(std::next(all_species.begin(), rand() % size)->second->breed());
    }

    // Free all the genomes
    for (auto it = genomes.begin(); it != genomes.end(); it++) {
        delete *it;
    }
    genomes = new_genomes;

    // Free all the species
    for (auto it = all_species.begin(); it != all_species.end(); it++) {
        delete it->second;
    }
    all_species.clear();

}

void Neat::mutate() {
    for (auto it = genomes.begin(); it != genomes.end(); it++) {
        (*it)->mutate();
    }
}

void Neat::to_file(std::string filename) {
    FILE* fp = fopen(filename.c_str(), "w");
    Node* node;
    std::pair<int, int> pair;

    fprintf(fp, "Nodes\n");
    fprintf(fp, "Input: %d\n", input);
    fprintf(fp, "Output: %d\n", output);
    fprintf(fp, "Hidden: %d\n", (int) all_nodes.size() - input - output);

    fprintf(fp, "\nConnections:\n");
    for (auto it = all_markings.begin(); it != all_markings.end(); it++) {
        pair = it->first;
        fprintf(fp, "%d: %d->%d\n",
            it->second,
            pair.first,
            pair.second
        );
    }

    fclose(fp);
}

void Neat::load_file(std::string filename) {
    // Delete all connections and nodes
    for (auto it = all_connections.begin(); it != all_connections.end(); it++) {
        delete it->second;
    }

    for (auto it = all_nodes.begin(); it != all_nodes.end(); it++) {
        delete it->second;
    }

    all_connections.clear();
    all_nodes.clear();
    all_markings.clear();

    FILE* fp = fopen(filename.c_str(), "r");

    int hidden, innov, from, to;
    Connection* connection;

    // TODO: Check if this works
    fscanf(fp, "Nodes\nInput: %d\nOutput: %d\nHidden: %d\n", &input, &output, &hidden);

    hidden += input + output;
    for (int i = 0; i < hidden; i++) {
        all_nodes[i] = new Node(i);
    }

    fscanf(fp, "\nConnections:\n");
    while (fscanf(fp, "%d: %d->%d", &innov, &from, &to)) {
        all_connections[innov] = new Connection(get_node(from), get_node(to));
        all_markings[std::make_pair(from, to)] = innov;
    }

    fclose(fp);
}
