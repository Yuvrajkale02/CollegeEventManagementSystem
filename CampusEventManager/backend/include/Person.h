#ifndef PERSON_H
#define PERSON_H

#include <string>
using namespace std;

// ---------------------------------------------------------------------
// Abstract Base Class : Person
// OOP Concepts demonstrated:
//   - Encapsulation (private data members, public getters)
//   - Abstraction  (pure virtual functions -> makes this an ABSTRACT class)
//   - This class can never be instantiated directly, only through
//     its derived classes Student / Organizer  -> Inheritance base
// ---------------------------------------------------------------------
class Person {
protected:
    string name;
    string email;

public:
    // Parameterized constructor
    Person(string n, string e) : name(n), email(e) {}

    // Getters (encapsulation - controlled access to private/protected data)
    string getName() const { return name; }
    string getEmail() const { return email; }

    // Setters
    void setName(string n) { name = n; }
    void setEmail(string e) { email = e; }

    // Pure virtual functions -> makes Person an ABSTRACT class
    virtual void displayInfo() const = 0;
    virtual string getRole() const = 0;

    // Virtual destructor -> ensures derived class destructors run correctly
    virtual ~Person() {}
};

#endif
