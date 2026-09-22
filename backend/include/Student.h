#ifndef STUDENT_H
#define STUDENT_H

#include "Person.h"
#include <iostream>
using namespace std;

// ---------------------------------------------------------------------
// Student : public Person
// OOP Concepts demonstrated:
//   - Inheritance (Student IS-A Person)
//   - Runtime Polymorphism (overrides displayInfo() and getRole())
//   - Constructor chaining (calls base class constructor)
// ---------------------------------------------------------------------
class Student : public Person {
private:
    string rollNo;
    string department;

public:
    // Constructor - chains to Person's constructor
    Student(string n, string e, string roll, string dept)
        : Person(n, e), rollNo(roll), department(dept) {}

    string getRollNo() const { return rollNo; }
    string getDepartment() const { return department; }

    // Overriding base class virtual function -> Polymorphism
    void displayInfo() const override {
        cout << "Student: " << name << " | Roll No: " << rollNo
             << " | Dept: " << department << " | Email: " << email << endl;
    }

    string getRole() const override {
        return "Student";
    }

    ~Student() {}
};

#endif
