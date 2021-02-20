#include "../include/connection.h"
#include <iostream>

// CONSTRUCTORS
Connection::Connection() {}

Connection::Connection(Connection* connection) {
    this->from = connection->from;
    this->to = connection->to;
    this->enabled = connection->enabled;
    this->weight = connection->weight;
    this->innovation_number = connection->innovation_number;
}

Connection::Connection(Node* from, Node* to) {
    this->from = from;
    this->to = to;
}

// GETTERS
Node* Connection::get_from() {
    return from;
}

Node* Connection::get_to() {
    return to;
}

double Connection::get_weight() {
    return weight;
}

bool Connection::is_enabled() {
    return enabled;
}

int Connection::get_innovation_number() {
    return innovation_number;
}

// SETTERS
void Connection::set_from(Node* from) {
    this->from = from;
}

void Connection::set_to(Node* to) {
    this->to = to;
}

void Connection::set_weight(double weight) {
    this->weight = weight;
}

void Connection::set_enabled(bool enabled) {
    this->enabled = enabled;
}

void Connection::set_innovation_number(int innovation_number) {
    this->innovation_number = innovation_number;
}

// METHODS
std::pair<int, int> Connection::get_innovation_pair() {
    return std::make_pair(
        from->get_innovation_number(),
        to->get_innovation_number()
    );
}

void Connection::calculate() {
    if (enabled) {
        to->add_val(weight * from->get_val());
    }
}
