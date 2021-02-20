// enum gene_type {
//     SENSOR,
//     OUTPUT,
//     HIDDEN
// };

/**
 *  Node Gene
 */
class Node {
private:
    int innovation_number;

    double x;
    double y;

    double val = 0.0;

    // gene_type type;

public:
    // CONSTRUCTORSc
    Node(int innovation_number);

    // GETTERS
    int get_innovation_number();
    double get_x();
    double get_y();
    double get_val();
    // gene_type get_type();

    // SETTERS
    // void set_innovation_number(int innovation_number);
    void set_x(double x);
    void set_y(double y);
    void set_val(double val);
    // void set_type(gene_type type);

    // METHODS
    void add_val(double inc);
};