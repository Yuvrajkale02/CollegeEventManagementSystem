#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>
using namespace std;

// ---------------------------------------------------------------------
// Custom exception classes (Exception Handling - inherits from std::exception)
// Each class OVERRIDES the virtual what() function -> Polymorphism
// ---------------------------------------------------------------------

class EventNotFoundException : public exception {
private:
    string message;
public:
    EventNotFoundException(int id) {
        message = "Event with ID " + to_string(id) + " was not found.";
    }
    const char* what() const noexcept override {
        return message.c_str();
    }
};

class SeatsFullException : public exception {
private:
    string message;
public:
    SeatsFullException(const string &eventTitle) {
        message = "No seats available for event: " + eventTitle;
    }
    const char* what() const noexcept override {
        return message.c_str();
    }
};

class DuplicateRegistrationException : public exception {
private:
    string message;
public:
    DuplicateRegistrationException(const string &rollNo) {
        message = "Student with Roll No " + rollNo + " has already registered for this event.";
    }
    const char* what() const noexcept override {
        return message.c_str();
    }
};

class InvalidCredentialsException : public exception {
private:
    string message;
public:
    InvalidCredentialsException() {
        message = "Invalid username or password.";
    }
    const char* what() const noexcept override {
        return message.c_str();
    }
};

class DatabaseConnectionException : public exception {
private:
    string message;
public:
    DatabaseConnectionException(const string &err) {
        message = "Database connection failed: " + err;
    }
    const char* what() const noexcept override {
        return message.c_str();
    }
};

class InvalidInputException : public exception {
private:
    string message;
public:
    InvalidInputException(const string &field) {
        message = "Invalid input provided for: " + field;
    }
    const char* what() const noexcept override {
        return message.c_str();
    }
};

#endif
