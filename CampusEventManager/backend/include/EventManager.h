#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <vector>
#include <fstream>
#include <ctime>
#include "Database.h"
#include "Event.h"
#include "Registration.h"
#include "Exceptions.h"
using namespace std;

// ---------------------------------------------------------------------
// EventManager class
// This is the "business logic" layer sitting between the database and
// the HTTP routes in main.cpp.
//
// OOP Concepts demonstrated:
//   - Composition (EventManager "has-a" Database)
//   - Function Overloading (two versions of getEvents())
//   - Exception Handling (throws & the caller catches)
//   - File Handling (writes every action to activity_log.txt)
// ---------------------------------------------------------------------
class EventManager {
private:
    Database *db;

    // Returns current timestamp as a string - used for logs & registration date
    string currentTimestamp() {
        time_t now = time(0);
        char buf[30];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
        return string(buf);
    }

    // File Handling: appends a line to activity_log.txt (fstream)
    void logActivity(const string &action) {
        ofstream logFile("activity_log.txt", ios::app);
        if (logFile.is_open()) {
            logFile << "[" << currentTimestamp() << "] " << action << endl;
            logFile.close();
        }
    }

    // Escapes a string safely for use in a MySQL query
    string esc(const string &raw) {
        char *buffer = new char[raw.length() * 2 + 1];
        mysql_real_escape_string(db->getConnection(), buffer, raw.c_str(), raw.length());
        string result(buffer);
        delete[] buffer;
        return result;
    }

public:
    EventManager() {
        db = Database::getInstance();
    }

    // ---------------- EVENTS ----------------

    // Function Overloading (1): get ALL events
    vector<Event> getEvents() {
        vector<Event> events;
        string query = "SELECT event_id, title, description, category, "
                        "event_date, venue, total_seats, seats_filled FROM events "
                        "ORDER BY event_date ASC";

        if (mysql_query(db->getConnection(), query.c_str())) {
            throw DatabaseConnectionException(mysql_error(db->getConnection()));
        }

        MYSQL_RES *result = mysql_store_result(db->getConnection());
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result))) {
            Event e(atoi(row[0]), row[1], row[2] ? row[2] : "", row[3] ? row[3] : "",
                     row[4], row[5] ? row[5] : "", atoi(row[6]), atoi(row[7]));
            events.push_back(e);
        }
        mysql_free_result(result);
        return events;
    }

    // Function Overloading (2): get events filtered by category
    vector<Event> getEvents(const string &category) {
        vector<Event> events;
        string query = "SELECT event_id, title, description, category, "
                        "event_date, venue, total_seats, seats_filled FROM events "
                        "WHERE category = '" + esc(category) + "' ORDER BY event_date ASC";

        if (mysql_query(db->getConnection(), query.c_str())) {
            throw DatabaseConnectionException(mysql_error(db->getConnection()));
        }

        MYSQL_RES *result = mysql_store_result(db->getConnection());
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result))) {
            Event e(atoi(row[0]), row[1], row[2] ? row[2] : "", row[3] ? row[3] : "",
                     row[4], row[5] ? row[5] : "", atoi(row[6]), atoi(row[7]));
            events.push_back(e);
        }
        mysql_free_result(result);
        return events;
    }

    Event getEventById(int id) {
        string query = "SELECT event_id, title, description, category, "
                        "event_date, venue, total_seats, seats_filled FROM events "
                        "WHERE event_id = " + to_string(id);

        if (mysql_query(db->getConnection(), query.c_str())) {
            throw DatabaseConnectionException(mysql_error(db->getConnection()));
        }

        MYSQL_RES *result = mysql_store_result(db->getConnection());
        MYSQL_ROW row = mysql_fetch_row(result);

        if (!row) {
            mysql_free_result(result);
            throw EventNotFoundException(id);
        }

        Event e(atoi(row[0]), row[1], row[2] ? row[2] : "", row[3] ? row[3] : "",
                 row[4], row[5] ? row[5] : "", atoi(row[6]), atoi(row[7]));
        mysql_free_result(result);
        return e;
    }

    bool addEvent(Event &e) {
        if (e.getTitle().empty() || e.getTotalSeats() <= 0) {
            throw InvalidInputException("title / totalSeats");
        }

        string query = "INSERT INTO events (title, description, category, event_date, "
                        "venue, total_seats, seats_filled) VALUES ('" +
                        esc(e.getTitle()) + "','" + esc(e.getDescription()) + "','" +
                        esc(e.getCategory()) + "','" + esc(e.getEventDate()) + "','" +
                        esc(e.getVenue()) + "'," + to_string(e.getTotalSeats()) + ",0)";

        if (mysql_query(db->getConnection(), query.c_str())) {
            throw DatabaseConnectionException(mysql_error(db->getConnection()));
        }

        logActivity("Event added: " + e.getTitle());
        return true;
    }

    bool updateEvent(Event &e) {
        // Confirms the event exists first - throws EventNotFoundException otherwise
        getEventById(e.getEventId());

        string query = "UPDATE events SET title='" + esc(e.getTitle()) +
                        "', description='" + esc(e.getDescription()) +
                        "', category='" + esc(e.getCategory()) +
                        "', event_date='" + esc(e.getEventDate()) +
                        "', venue='" + esc(e.getVenue()) +
                        "', total_seats=" + to_string(e.getTotalSeats()) +
                        " WHERE event_id=" + to_string(e.getEventId());

        if (mysql_query(db->getConnection(), query.c_str())) {
            throw DatabaseConnectionException(mysql_error(db->getConnection()));
        }

        logActivity("Event updated: ID " + to_string(e.getEventId()));
        return true;
    }

    bool deleteEvent(int id) {
        getEventById(id); // throws EventNotFoundException if it doesn't exist

        string delRegs = "DELETE FROM registrations WHERE event_id=" + to_string(id);
        mysql_query(db->getConnection(), delRegs.c_str());

        string query = "DELETE FROM events WHERE event_id=" + to_string(id);
        if (mysql_query(db->getConnection(), query.c_str())) {
            throw DatabaseConnectionException(mysql_error(db->getConnection()));
        }

        logActivity("Event deleted: ID " + to_string(id));
        return true;
    }

    // ---------------- REGISTRATIONS ----------------

    bool registerStudent(Registration &r) {
        Event e = getEventById(r.getEventId());  // throws if event not found

        if (!e.hasAvailableSeats()) {
            throw SeatsFullException(e.getTitle());
        }

        // Duplicate check: same roll number can't register twice for the same event
        string checkQuery = "SELECT reg_id FROM registrations WHERE event_id=" +
                             to_string(r.getEventId()) + " AND roll_no='" + esc(r.getRollNo()) + "'";
        mysql_query(db->getConnection(), checkQuery.c_str());
        MYSQL_RES *checkResult = mysql_store_result(db->getConnection());
        if (mysql_fetch_row(checkResult)) {
            mysql_free_result(checkResult);
            throw DuplicateRegistrationException(r.getRollNo());
        }
        mysql_free_result(checkResult);

        string query = "INSERT INTO registrations (event_id, student_name, roll_no, "
                        "department, email) VALUES (" + to_string(r.getEventId()) + ",'" +
                        esc(r.getStudentName()) + "','" + esc(r.getRollNo()) + "','" +
                        esc(r.getDepartment()) + "','" + esc(r.getEmail()) + "')";

        if (mysql_query(db->getConnection(), query.c_str())) {
            throw DatabaseConnectionException(mysql_error(db->getConnection()));
        }

        string updateSeats = "UPDATE events SET seats_filled = seats_filled + 1 WHERE event_id=" +
                              to_string(r.getEventId());
        mysql_query(db->getConnection(), updateSeats.c_str());

        logActivity("Registration: " + r.getStudentName() + " (" + r.getRollNo() +
                    ") -> Event ID " + to_string(r.getEventId()));
        return true;
    }

    vector<Registration> getRegistrations(int eventId = -1) {
        vector<Registration> regs;
        string query = "SELECT reg_id, event_id, student_name, roll_no, department, "
                        "email, reg_date FROM registrations";
        if (eventId != -1) {
            query += " WHERE event_id=" + to_string(eventId);
        }
        query += " ORDER BY reg_date DESC";

        if (mysql_query(db->getConnection(), query.c_str())) {
            throw DatabaseConnectionException(mysql_error(db->getConnection()));
        }

        MYSQL_RES *result = mysql_store_result(db->getConnection());
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result))) {
            Registration r(atoi(row[0]), atoi(row[1]), row[2], row[3], row[4], row[5], row[6]);
            regs.push_back(r);
        }
        mysql_free_result(result);
        return regs;
    }

    // ---------------- ADMIN AUTH ----------------

    bool authenticateAdmin(const string &username, const string &password) {
        string query = "SELECT admin_id FROM admin WHERE username='" + esc(username) +
                        "' AND password='" + esc(password) + "'";

        if (mysql_query(db->getConnection(), query.c_str())) {
            throw DatabaseConnectionException(mysql_error(db->getConnection()));
        }

        MYSQL_RES *result = mysql_store_result(db->getConnection());
        MYSQL_ROW row = mysql_fetch_row(result);
        bool valid = (row != nullptr);
        mysql_free_result(result);

        if (!valid) {
            throw InvalidCredentialsException();
        }

        logActivity("Admin login: " + username);
        return true;
    }
};

#endif
