#include "./node.h"
#include <utility>

/**
 *  Connection Gene 
 */
class Connection {
private:
    Node* from;
    Node* to;

    double weight = 0.0;
    bool enabled = true;
    int innovation_number;

public:
    // CONSTRUCTORS
    Connection();
    Connection(Connection* connection);
    Connection(Node* from, Node* to);

    // GETTERS
    Node* get_from();
    Node* get_to();
    double get_weight();
    bool is_enabled();
    int get_innovation_number();

    // SETTERS
    void set_from(Node* from);
    void set_to(Node* to);
    void set_weight(double weight);
    void set_enabled(bool enabled);
    void set_innovation_number(int innovation_number);

    // METHODS
    std::pair<int, int> get_innovation_pair();
    void calculate();
};