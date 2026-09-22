#ifndef ORGANIZER_H
#define ORGANIZER_H

#include "Person.h"
#include <iostream>
using namespace std;

// ---------------------------------------------------------------------
// Organizer (Admin) : public Person
// OOP Concepts demonstrated:
//   - Inheritance (Organizer IS-A Person)
//   - Runtime Polymorphism (overrides displayInfo() and getRole())
// ---------------------------------------------------------------------
class Organizer : public Person {
private:
    string username;
    string password;

public:
    Organizer(string n, string e, string u, string p)
        : Person(n, e), username(u), password(p) {}

    string getUsername() const { return username; }

    bool authenticate(const string &u, const string &p) const {
        return (username == u && password == p);
    }

    void displayInfo() const override {
        cout << "Organizer: " << name << " | Username: " << username
             << " | Email: " << email << endl;
    }

    string getRole() const override {
        return "Organizer";
    }

    ~Organizer() {}
};

#endif
