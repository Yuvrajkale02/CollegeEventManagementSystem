#include "../include/httplib.h"
#include "../include/EventManager.h"
#include "../include/Event.h"
#include "../include/Registration.h"
#include "../include/Exceptions.h"
#include <iostream>
#include <sstream>
using namespace std;
using namespace httplib;

// ---------------------------------------------------------------------
// Tiny JSON helpers (kept deliberately simple - no external JSON
// library - just enough string parsing to read the fields our own
// frontend sends). This is NOT a general purpose JSON parser.
// ---------------------------------------------------------------------
string jsonGetString(const string &body, const string &key) {
    string search = "\"" + key + "\"";
    size_t pos = body.find(search);
    if (pos == string::npos) return "";
    pos = body.find(':', pos);
    if (pos == string::npos) return "";
    pos = body.find('"', pos);
    if (pos == string::npos) return "";
    size_t end = body.find('"', pos + 1);
    while (end != string::npos && body[end - 1] == '\\') {
        end = body.find('"', end + 1);
    }
    if (end == string::npos) return "";
    return body.substr(pos + 1, end - pos - 1);
}

int jsonGetInt(const string &body, const string &key) {
    string search = "\"" + key + "\"";
    size_t pos = body.find(search);
    if (pos == string::npos) return 0;
    pos = body.find(':', pos);
    if (pos == string::npos) return 0;
    pos++;
    while (pos < body.size() && (body[pos] == ' ')) pos++;
    size_t end = pos;
    while (end < body.size() && (isdigit(body[end]) || body[end] == '-')) end++;
    if (end == pos) return 0;
    return stoi(body.substr(pos, end - pos));
}

// Builds a simple JSON array string like [ {..}, {..} ]
string buildArray(const vector<string> &items) {
    string out = "[";
    for (size_t i = 0; i < items.size(); i++) {
        out += items[i];
        if (i != items.size() - 1) out += ",";
    }
    out += "]";
    return out;
}

int main() {
    Server svr;
    EventManager manager;

    // ---- Allow the frontend (served separately or via file://) to call the API ----
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });
    svr.Options(R"(/.*)", [](const Request &, Response &res) {
        res.status = 200;
    });

    // Serve the frontend static files (index.html, style.css, script.js)
    svr.set_mount_point("/", "../../frontend");

    // ---------------- GET /api/events ----------------
    // Supports optional ?category=Technical  (demonstrates function overloading in EventManager)
    svr.Get("/api/events", [&](const Request &req, Response &res) {
        try {
            vector<Event> events;
            if (req.has_param("category") && !req.get_param_value("category").empty()) {
                events = manager.getEvents(req.get_param_value("category"));
            } else {
                events = manager.getEvents();
            }
            vector<string> jsonItems;
            for (const Event &e : events) jsonItems.push_back(e.toJSON());
            res.set_content(buildArray(jsonItems), "application/json");
        } catch (exception &ex) {
            res.status = 500;
            res.set_content("{\"error\":\"" + string(ex.what()) + "\"}", "application/json");
        }
    });

    // ---------------- GET /api/events/:id ----------------
    svr.Get(R"(/api/events/(\d+))", [&](const Request &req, Response &res) {
        try {
            int id = stoi(req.matches[1]);
            Event e = manager.getEventById(id);
            res.set_content(e.toJSON(), "application/json");
        } catch (EventNotFoundException &ex) {
            res.status = 404;
            res.set_content("{\"error\":\"" + string(ex.what()) + "\"}", "application/json");
        } catch (exception &ex) {
            res.status = 500;
            res.set_content("{\"error\":\"" + string(ex.what()) + "\"}", "application/json");
        }
    });

    // ---------------- POST /api/events (Admin: add event) ----------------
    svr.Post("/api/events", [&](const Request &req, Response &res) {
        try {
            Event e;
            e.setTitle(jsonGetString(req.body, "title"));
            e.setDescription(jsonGetString(req.body, "description"));
            e.setCategory(jsonGetString(req.body, "category"));
            e.setEventDate(jsonGetString(req.body, "eventDate"));
            e.setVenue(jsonGetString(req.body, "venue"));
            e.setTotalSeats(jsonGetInt(req.body, "totalSeats"));

            manager.addEvent(e);
            res.set_content("{\"message\":\"Event added successfully\"}", "application/json");
        } catch (InvalidInputException &ex) {
            res.status = 400;
            res.set_content("{\"error\":\"" + string(ex.what()) + "\"}", "application/json");
        } catch (exception &ex) {
            res.status = 500;
            res.set_content("{\"error\":\"" + string(ex.what()) + "\"}", "application/json");
        }
    });

    // ---------------- PUT /api/events/:id (Admin: update event) ----------------
    svr.Put(R"(/api/events/(\d+))", [&](const Request &req, Response &res) {
        try {
            Event e;
            e.setEventId(stoi(req.matches[1]));
            e.setTitle(jsonGetString(req.body, "title"));
            e.setDescription(jsonGetString(req.body, "description"));
            e.setCategory(jsonGetString(req.body, "category"));
            e.setEventDate(jsonGetString(req.body, "eventDate"));
            e.setVenue(jsonGetString(req.body, "venue"));
            e.setTotalSeats(jsonGetInt(req.body, "totalSeats"));

            manager.updateEvent(e);
            res.set_content("{\"message\":\"Event updated successfully\"}", "application/json");
        } catch (EventNotFoundException &ex) {
            res.status = 404;
            res.set_content("{\"error\":\"" + string(ex.what()) + "\"}", "application/json");
        } catch (exception &ex) {
            res.status = 500;
            res.set_content("{\"error\":\"" + string(ex.what()) + "\"}", "application/json");
        }
    });

    // ---------------- DELETE /api/events/:id (Admin: delete event) ----------------
    svr.Delete(R"(/api/events/(\d+))", [&](const Request &req, Response &res) {
        try {
            int id = stoi(req.matches[1]);
            manager.deleteEvent(id);
            res.set_content("{\"message\":\"Event deleted successfully\"}", "application/json");
        } catch (EventNotFoundException &ex) {
            res.status = 404;
            res.set_content("{\"error\":\"" + string(ex.what()) + "\"}", "application/json");
        } catch (exception &ex) {
            res.status = 500;
            res.set_content("{\"error\":\"" + string(ex.what()) + "\"}", "application/json");
        }
    });

    // ---------------- POST /api/register (Student: register for an event) ----------------
    svr.Post("/api/register", [&](const Request &req, Response &res) {
        try {
            Registration r;
            r.setEventId(jsonGetInt(req.body, "eventId"));
            r.setStudentName(jsonGetString(req.body, "studentName"));
            r.setRollNo(jsonGetString(req.body, "rollNo"));
            r.setDepartment(jsonGetString(req.body, "department"));
            r.setEmail(jsonGetString(req.body, "email"));

            manager.registerStudent(r);
            res.set_content("{\"message\":\"Registration successful!\"}", "application/json");
        } catch (EventNotFoundException &ex) {
            res.status = 404;
            res.set_content("{\"error\":\"" + string(ex.what()) + "\"}", "application/json");
        } catch (SeatsFullException &ex) {
            res.status = 409;
            res.set_content("{\"error\":\"" + string(ex.what()) + "\"}", "application/json");
        } catch (DuplicateRegistrationException &ex) {
            res.status = 409;
            res.set_content("{\"error\":\"" + string(ex.what()) + "\"}", "application/json");
        } catch (exception &ex) {
            res.status = 500;
            res.set_content("{\"error\":\"" + string(ex.what()) + "\"}", "application/json");
        }
    });

    // ---------------- GET /api/registrations (Admin: view registrations) ----------------
    svr.Get("/api/registrations", [&](const Request &req, Response &res) {
        try {
            int eventId = -1;
            if (req.has_param("eventId")) eventId = stoi(req.get_param_value("eventId"));

            vector<Registration> regs = manager.getRegistrations(eventId);
            vector<string> jsonItems;
            for (const Registration &r : regs) jsonItems.push_back(r.toJSON());
            res.set_content(buildArray(jsonItems), "application/json");
        } catch (exception &ex) {
            res.status = 500;
            res.set_content("{\"error\":\"" + string(ex.what()) + "\"}", "application/json");
        }
    });

    // ---------------- POST /api/admin/login ----------------
    svr.Post("/api/admin/login", [&](const Request &req, Response &res) {
        try {
            string username = jsonGetString(req.body, "username");
            string password = jsonGetString(req.body, "password");

            manager.authenticateAdmin(username, password);
            res.set_content("{\"message\":\"Login successful\"}", "application/json");
        } catch (InvalidCredentialsException &ex) {
            res.status = 401;
            res.set_content("{\"error\":\"" + string(ex.what()) + "\"}", "application/json");
        } catch (exception &ex) {
            res.status = 500;
            res.set_content("{\"error\":\"" + string(ex.what()) + "\"}", "application/json");
        }
    });

    cout << "============================================================" << endl;
    cout << " College Event Management System - Backend Server" << endl;
    cout << " Listening on: http://localhost:8080" << endl;
    cout << " Open http://localhost:8080 in your browser." << endl;
    cout << "============================================================" << endl;

    svr.listen("0.0.0.0", 8080);
    return 0;
}
