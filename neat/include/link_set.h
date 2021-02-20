#ifndef _LINK_SET_H
#define _LINK_SET_H

#include <unordered_map>
#include <iostream>

template<typename V>
struct link_set_node {
    link_set_node* prev = NULL;
    link_set_node* next = NULL;
    V value;

    link_set_node() {}

    link_set_node(V value) {
        link_set_node(NULL, NULL, value);
    }

    link_set_node(link_set_node* prev, link_set_node* next, V value) {
        this->prev = prev;
        this->next = next;
        this->value = value;
    }
};

template<
    typename T,
    typename C = std::hash<T>
>
class link_set {
private:
    std::unordered_map<T, link_set_node<T*>*, C> map;
    link_set_node<T*>* head = NULL;
    link_set_node<T*>* tail = NULL;

public:
    // CONSTRUCTORS
    link_set() {
        map.clear();
    }

    ~link_set() {
        link_set_node<T*>* next;

        while (head->next != NULL) {
            next = head->next;

            delete head->value;
            delete head;

            head = next;
        }

        delete head->value;
        delete head;
    }

    // GETTERS
    link_set_node<T*>* get_head() {
        return head;
    }

    link_set_node<T*>* get_tail() {
        return tail;
    }

    // METHODS
    bool is_empty() {
        return head == NULL;
    }

    bool contains(const T& key) {
        return map.find(key) != map.end();
    }

    void push_node(const T& val) {
        if (head == NULL) {
            head = new link_set_node<T*>(NULL, NULL, new T(val));
            tail = head;
            map[val] = head;
        } else {
            link_set_node<T*>* node = new link_set_node<T*>(tail, NULL, new T(val));
            tail->next = node;
            tail = node;
            map[val] = node;
        }
    }

    link_set_node<T*>* operator[](const T& key) {
        if (map.find(key) != map.end()) {
            return map[key];
        }

        return NULL;
    }

    link_set_node<T*>* at(const T& key) {
        if (map.find(key) != map.end()) {
            return map[key];
        }
        return NULL;
    }

    void add_node_prev(const T& key, const T& val) {
        add_node_prev(map[key], val);
    }

    void add_node_prev(link_set_node<T*>* curr, const T& val) {
        link_set_node<T*>* prev = new link_set_node<T*>(
            curr->prev,     // Prev
            curr,           // Next
            new T(val)      // This Value
        );

        curr->prev = prev;
        map[val] = prev;

        if (curr == head) {
            head = prev;
        } else {
            prev->prev->next = prev;
        }
    }

    void add_node_next(const T& key, const T& val) {
        add_node_next(map[key], val);
    }

    void add_node_next(link_set_node<T*>* curr, const T& val) {
        link_set_node<T*>* next = new link_set_node<T*>(
            curr,           // Prev
            curr->next,     // Next
            new T(val)      // This Value
        );

        curr->next = next;
        map[val] = next;

        if (curr == tail) {
            tail = next;
        } else {
            next->next->prev = next;
        }
    }
};

#endif