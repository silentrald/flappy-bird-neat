#include "../include/genome.h"

// PRIVATE METHODS
void Genome::add_connection_to_child(Genome* child, Connection* connection) {
    // Add Connection
    child->add_connection(connection->get_innovation_number(), new Connection(connection));

    // Add Nodes // TODO: Might double write to a node
    Node* node = connection->get_from();
    child->add_node(node->get_innovation_number(), node);

    node = connection->get_to();
    child->add_node(node->get_innovation_number(), node); 
}

Connection* Genome::random_connection() {
    return std::next(connections.begin(), rand() % connections.size())->second;
}

// CONSTRUCTOR
// Genome::Genome() {}

Genome::Genome(Neat* neat) {
    this->neat = neat;
}

// DECONSTRUCTOR
Genome::~Genome() {
    for (auto it = connections.begin(); it != connections.end(); it++) {
        delete it->second;
    }

    // The same
    // for (auto it = nodes.begin(); it != nodes.end(); it++) {
    //     delete it->second;
    // }
}

// GETTERS
double Genome::get_fitness() {
    return fitness;
}

std::map<int, Connection*> Genome::get_connections() {
    return connections;
}

std::map<int, Node*> Genome::get_nodes() {
    return nodes;
}

// SETTERS
void Genome::set_fitness(double fitness) {
    this->fitness = fitness;
}

// ITERATORS, For looping
std::map<int, Connection*>::iterator Genome::get_connections_begin() {
    return connections.begin();
}

std::map<int, Connection*>::iterator Genome::get_connections_end() {
    return connections.end();
}

std::map<int, Node*>::iterator Genome::get_nodes_begin() {
    return nodes.begin();
}

std::map<int, Node*>::iterator Genome::get_nodes_end() {
    return nodes.end();
}

// PUBLIC METHODS
void Genome::add_connection(std::pair<int, int> pair, int key, Connection* connection) {
    connections[key] = connection;
    markings[pair] = key;
}

void Genome::add_connection(std::pair<int, int> pair, Connection* connection) {
    int innovation_number = connection->get_innovation_number();

    connections[innovation_number] = connection;
    markings[pair] = innovation_number;
}

void Genome::add_connection(int key, Connection* connection) {
    connections[key] = connection;
    markings[connection->get_innovation_pair()] = key;
}

void Genome::add_node(int key, Node* node) {
    nodes[key] = node;
}

void Genome::push_calculation_order(const std::pair<int, int>& pair) {
    calculation_order.push_node(pair);
}

int Genome::connections_size() {
    return connections.size();
}

int Genome::nodes_size() {
    return nodes.size();
}

double Genome::distance(Genome* other) {
    int s1 = connections.size();
    int s2 = other->connections.size();

    int index_c1 = 0;
    int index_c2 = 0;

    int similar = 0;
    int disjoint = 0;
    int excess = 0;
    double weight_diff = 0;

    if (s1 > 0 && s2 > 0) {
        std::map<int, Connection*>::iterator p1 = connections.begin();
        std::map<int, Connection*>::iterator p2 = other->connections.begin();

        std::map<int, Connection*>::iterator e1 = connections.end();
        std::map<int, Connection*>::iterator e2 = other->connections.end();

        Connection* c1 = p1->second;
        Connection* c2 = p2->second;

        int i1 = c1->get_innovation_number();
        int i2 = c2->get_innovation_number();

        while (true) {
            if (i1 == i2) {         // Similar Genes
                similar++;
                weight_diff += std::abs(c1->get_weight() - c2->get_weight());
                index_c1++;
                index_c2++;

                p1++;
                p2++;
                if (p1 == e1 || p2 == e2) break;

                c1 = p1->second;
                i1 = c1->get_innovation_number();

                c2 = p2->second;
                i2 = c2->get_innovation_number();
            } else if (i1 < i2) {   // Disjoint Genes of self
                disjoint++;
                index_c1++;

                p1++;
                if (p1 == e1) break;
                
                c1 = p1->second;
                i1 = c1->get_innovation_number();
            } else { // i1 > i2     // Disjoint Genes of other
                disjoint++;
                index_c2++;

                p2++;
                if (p2 == e2) break;
                
                c2 = p2->second;
                i2 = c2->get_innovation_number();
            }
        }

        excess = p1 == e1 ? s2 - index_c2 : s1 - index_c1;
        weight_diff /= similar;

    } else if (s1 > 0) {
        excess = s1;
    } else {
        excess = s2;
    }

    double N = s1 > s2 ? s1 : s2;       // Get the max between the two
    if (N < 20.0) {
        N = 1.0;
    }

    // = c1 * E / N + c2 * D / N + c3 * W
    // = (c1 * E + c2 * D) / N + c3 * W
    return (neat->get_excess_coefficient() * excess + neat->get_disjoint_coefficient() * disjoint) / N + neat->get_weight_diff_coefficient() * weight_diff;
}

Genome* Genome::crossover(Genome* other) {
    Genome* child = new Genome(neat);

    int s1 = connections.size();
    int s2 = other->connections.size();

    for (int i = neat->get_input() + neat->get_output() - 1; i > -1; i--) {
        child->add_node(i, neat->get_node(i));
    }

    if (s1 > 0 && s2 > 0) {
        std::map<int, Connection*>::iterator p1 = connections.begin();
        std::map<int, Connection*>::iterator p2 = other->connections.begin();

        std::map<int, Connection*>::iterator e1 = connections.end();
        std::map<int, Connection*>::iterator e2 = other->connections.end();

        Connection* c1 = p1->second;
        Connection* c2 = p2->second;

        int i1 = c1->get_innovation_number();
        int i2 = c2->get_innovation_number();

        while (true) { 
            if (i1 == i2) {         // Randomly pick one  of the connection
                add_connection_to_child(child, rand() & 1 ? c1 : c2);

                p1++;
                p2++;
                if (p1 == e1 || p2 == e2) break;

                c1 = p1->second;
                i1 = c1->get_innovation_number();

                c2 = p2->second;
                i2 = c2->get_innovation_number();
            } else if (i1 < i2) {   // Insert connection 1
                add_connection_to_child(child, c1);

                p1++;
                if (p1 == e1) break;
                
                c1 = p1->second;
                i1 = c1->get_innovation_number();
            } else { // i1 > i2     // Insert connection 2
                add_connection_to_child(child, c2);

                p2++;
                if (p2 == e2) break;
                
                c2 = p2->second;
                i2 = c2->get_innovation_number();
            }
        }

        while (p1 != e1) {
            add_connection_to_child(child, p1->second);
            p1++;
        }

        while (p2 != e2) {
            add_connection_to_child(child, p2->second);
            p2++;
        }

        // New Calculation Order
        link_set_node<std::pair<int, int>*>* curr = calculation_order.get_head();
        while (curr != NULL) {
            child->push_calculation_order(*curr->value);
            curr = curr->next;
        }

        curr = other->calculation_order.get_head();
        std::pair<int, int>* curr_pair = calculation_order.get_head()->value;
        std::pair<int, int>* pair;

        while (curr != NULL) {
            pair = curr->value;
            if (child->calculation_order.contains(*pair)) {
                curr_pair = pair;
            } else {
                child->calculation_order.add_node_prev(*curr_pair, *pair);
            }

            curr = curr->next;
        }

    } else if (s1 > 0) {
        for (auto it = connections.begin(); it != connections.end(); it++)
            add_connection_to_child(child, it->second);
        
        link_set_node<std::pair<int, int>*>* curr = calculation_order.get_head();
        while (curr != NULL) {
            child->push_calculation_order(*curr->value);
            curr = curr->next;
        }
    } else {
        for (auto it = other->connections.begin(); it != other->connections.end(); it++)
            add_connection_to_child(child, it->second);
        
        link_set_node<std::pair<int, int>*>* curr = other->calculation_order.get_head();
        while (curr != NULL) {
            child->push_calculation_order(*curr->value);
            curr = curr->next;
        }
    }

    return child;
}

double activation_function(double x) {
    return x / (1.0 + std::abs(x));
}

std::vector<double> Genome::calculate(const std::vector<double>& input_vector) {
    int n = input_vector.size();
    int input = neat->get_input();
    int output = neat->get_output();

    // if (n != input) return NULL;

    std::map<int, Node*>::iterator itn = nodes.begin();
    std::vector<double> output_vector(output);
    link_set_node<std::pair<int, int>*>* node = calculation_order.get_head();
    std::pair<int, int>* pair;

    for (int i = 0; i < n; i++, itn++) {
        itn->second->set_val(input_vector[i]);
    }

    while (node != NULL) {
        pair = node->value;

        if (pair->first != pair->second)
            connections[markings[*pair]]->calculate();
        
        node = node->next;
    }

    n += output;
    for (int i = input; i < n; i++, itn++) {
        output_vector[i - input] = activation_function(itn->second->get_val());
    }

    for (itn = nodes.begin(); itn != nodes.end(); itn++) {
        itn->second->set_val(0.0);
    }

    return output_vector;
}

void Genome::add_fitness(double inc) {
    fitness += inc;
}

// MUTATIONS
void Genome::mutate() {
    if (neat->get_probability_mutate_link() > drand48()) {
        mutate_link();
    }

    if (neat->get_probability_mutate_node() > drand48()) {
        mutate_node();
    }

    if (neat->get_probability_mutate_weight_shift() > drand48()) {
        mutate_weight_shift();
    }

    if (neat->get_probability_mutate_weight_random() > drand48()) {
        mutate_weight_random();
    }

    if (neat->get_probability_mutate_toggle_link() > drand48()) {
        mutate_toggle_link();
    }
}

void Genome::mutate_link() {
    std::map<int, Node*>::iterator it = std::next(nodes.begin(), rand() % nodes.size());
    std::map<int, Node*>::iterator curr_it = std::next(nodes.begin(), rand() % nodes.size());
    std::map<int, Node*>::iterator end_it = nodes.end();

    bool n1_left; // If node1 is on the left side
    std::pair<int, int> key;

    Node* node1 = it->second;
    Node* node2;
    Connection* connection;

    int end = std::min(100, (int) nodes.size());
    
    while (end-- > 0) {
        node2 = curr_it->second;

        curr_it++;
        if (curr_it == end_it)
            curr_it = nodes.begin();

        if (std::abs(node1->get_x() - node2->get_x()) < 0.000001) {
            continue;
        }
        
        n1_left = node1->get_x() < node2->get_x();
        key = n1_left
            ? std::make_pair(node1->get_innovation_number(), node2->get_innovation_number())
            : std::make_pair(node2->get_innovation_number(), node1->get_innovation_number());
        
        if (markings.find(key) != markings.end()) {
            continue;
        }
        
        connection = n1_left
            ? neat->get_connection(node1, node2)
            : neat->get_connection(node2, node1);
        
        connection->set_weight((drand48() * 2.0 - 1.0) * neat->get_weight_random_strength());

        int innov = connection->get_innovation_number();
        connections[innov] = connection;
        markings[key] = innov;

        innov = connection->get_to()->get_innovation_number();
        calculation_order.add_node_prev(
            std::make_pair(innov, innov),
            key
        );
        return;
    }
}

void Genome::mutate_node() {
    if (connections.size() == 0) return;
    Connection* connection = random_connection();
    
    // Create 2 new connection and 1 node
    Node* from = connection->get_from();
    Node* to = connection->get_to();

    Node* mid = neat->get_node();
    mid->set_x((from->get_x() + to->get_x()) / 2);
    mid->set_y((from->get_y() + to->get_y()) / 2 + drand48() * 0.2 - 0.1);

    int from_innov = from->get_innovation_number();
    int to_innov = to->get_innovation_number();
    int mid_innov = mid->get_innovation_number();

    Connection* c1 = neat->get_connection(from, mid);
    Connection* c2 = neat->get_connection(mid, to);

    int c1_innov = c1->get_innovation_number();
    int c2_innov = c2->get_innovation_number();

    std::pair<int, int> p1 = std::make_pair(from_innov, mid_innov);
    std::pair<int, int> p2 = std::make_pair(mid_innov, to_innov);

    c1->set_weight(1);
    c2->set_weight(connection->get_weight());
    c2->set_enabled(connection->is_enabled());

    connection->set_enabled(false);

    nodes[mid_innov] = mid;
    connections[c1_innov] = c1;
    connections[c2_innov] = c2;

    markings[p1] = c1_innov;
    markings[p2] = c2_innov;

    link_set_node<std::pair<int, int>*>* node = calculation_order[std::make_pair(from_innov, to_innov)];
    calculation_order.add_node_prev(node, p1);
    calculation_order.add_node_prev(node, std::make_pair(mid_innov, mid_innov));
    calculation_order.add_node_prev(node, p2);
}

void Genome::mutate_weight_shift() {
    if (connections.size() == 0) return;
    Connection* connection = random_connection();

    connection->set_weight(connection->get_weight() + (drand48() * 2 - 1) * neat->get_weight_shift_strength());
}

void Genome::mutate_weight_random() {
    if (connections.size() == 0) return;
    Connection* connection = random_connection();
    
    connection->set_weight((drand48() * 2 - 1) * neat->get_weight_random_strength());
}

void Genome::mutate_toggle_link() {
    if (connections.size() == 0) return;
    Connection* connection = random_connection();
    connection->set_enabled(!connection->is_enabled());
}

void Genome::print() {
    printf("Nodes\nInput: [ ");

    std::map<int, Node*>::iterator it = nodes.begin();
    Connection* connection;
    std::pair<int, int> pair;

    int temp = neat->get_input();
    for (int i = 0; i < temp; i++) {
        printf("%d ", it->first);
        it++;
    }

    printf("]\nOutput: [ ");
    temp = neat->get_output();
    for (int i = 0; i < temp; i++) {
        printf("%d ", it->first);
        it++;
    }

    printf("]\nHidden: [ ");
    while (it != nodes.end()) {
        printf("%d ", it->first);
        it++;
    }
    printf("]\n\nConnections\n");
    for (auto it = connections.begin(); it != connections.end(); it++) {
        connection = it->second;
        printf("%d: %d->%d ; e: %d ; w: %lf\n",
            it->first,
            connection->get_from()->get_innovation_number(),
            connection->get_to()->get_innovation_number(),
            connection->is_enabled(),
            connection->get_weight()
        );
    }

    printf("\nCalc Order\n");
    link_set_node<std::pair<int, int>*>* head = calculation_order.get_head();
    while(head != NULL) {
        pair = *head->value;
        printf("%p: %d->%d\n", head, pair.first, pair.second);
        head = head->next;
    }
    printf("\n");
}
