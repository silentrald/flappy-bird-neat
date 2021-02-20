#include "../include/node.h"

// CONSTRUCTORS
Node::Node(int innovation_number) {
    this->innovation_number = innovation_number;
}

// GETTERS
int Node::get_innovation_number() {
    return innovation_number;
}

double Node::Node::get_x() {
    return x;
}

double Node::get_y() {
    return y;
}

double Node::get_val() {
    return val;
}

// gene_type Node::get_type() {
//     return type;
// }

// SETTERS
// void Node::set_innovation_number(int innovation_number) {
//     this->innovation_number = innovation_number;
// }

void Node::set_x(double x) {
    this->x = x;
}

void Node::set_y(double y) {
    this->y = y;
}

void Node::set_val(double val) {
    this->val = val;
}

// void Node::set_type(gene_type type) {
//     this->type = type;
// }

// METHODS
void Node::add_val(double inc) {
    val += inc;
}