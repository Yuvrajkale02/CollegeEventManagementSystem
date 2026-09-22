#ifndef EVENT_H
#define EVENT_H

#include <iostream>
#include <string>
using namespace std;

// ---------------------------------------------------------------------
// Event class
// OOP Concepts demonstrated:
//   - Encapsulation (private data + public getters/setters)
//   - Operator Overloading ( == operator, and << using friend function)
//   - Constructor Overloading (default + parameterized)
// ---------------------------------------------------------------------
class Event {
private:
    int eventId;
    string title;
    string description;
    string category;
    string eventDate;   // format: YYYY-MM-DD
    string venue;
    int totalSeats;
    int seatsFilled;

public:
    // Default constructor
    Event() : eventId(0), totalSeats(0), seatsFilled(0) {}

    // Parameterized constructor (Constructor Overloading)
    Event(int id, string t, string desc, string cat, string date,
          string ven, int total, int filled)
        : eventId(id), title(t), description(desc), category(cat),
          eventDate(date), venue(ven), totalSeats(total), seatsFilled(filled) {}

    // ------------- Getters -------------
    int getEventId() const { return eventId; }
    string getTitle() const { return title; }
    string getDescription() const { return description; }
    string getCategory() const { return category; }
    string getEventDate() const { return eventDate; }
    string getVenue() const { return venue; }
    int getTotalSeats() const { return totalSeats; }
    int getSeatsFilled() const { return seatsFilled; }
    int getSeatsAvailable() const { return totalSeats - seatsFilled; }

    // ------------- Setters -------------
    void setEventId(int id) { eventId = id; }
    void setTitle(string t) { title = t; }
    void setDescription(string d) { description = d; }
    void setCategory(string c) { category = c; }
    void setEventDate(string d) { eventDate = d; }
    void setVenue(string v) { venue = v; }
    void setTotalSeats(int t) { totalSeats = t; }
    void setSeatsFilled(int f) { seatsFilled = f; }

    bool hasAvailableSeats() const {
        return seatsFilled < totalSeats;
    }

    void incrementSeatsFilled() {
        seatsFilled++;
    }

    // Operator Overloading: compare two events by their ID
    bool operator==(const Event &other) const {
        return this->eventId == other.eventId;
    }

    // Friend function + operator overloading: allows "cout << event"
    friend ostream& operator<<(ostream &os, const Event &e) {
        os << "[" << e.eventId << "] " << e.title << " (" << e.category
           << ") on " << e.eventDate << " @ " << e.venue
           << " | Seats: " << e.seatsFilled << "/" << e.totalSeats;
        return os;
    }

    // Helper: converts object state to a JSON string for the frontend
    string toJSON() const {
        string j = "{";
        j += "\"eventId\":" + to_string(eventId) + ",";
        j += "\"title\":\"" + title + "\",";
        j += "\"description\":\"" + description + "\",";
        j += "\"category\":\"" + category + "\",";
        j += "\"eventDate\":\"" + eventDate + "\",";
        j += "\"venue\":\"" + venue + "\",";
        j += "\"totalSeats\":" + to_string(totalSeats) + ",";
        j += "\"seatsFilled\":" + to_string(seatsFilled) + ",";
        j += "\"seatsAvailable\":" + to_string(getSeatsAvailable());
        j += "}";
        return j;
    }
};

#endif
