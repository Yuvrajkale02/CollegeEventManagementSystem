#ifndef REGISTRATION_H
#define REGISTRATION_H

#include <string>
using namespace std;

// ---------------------------------------------------------------------
// Registration class
// Represents the association between a Student and an Event
// OOP Concept: Encapsulation
// ---------------------------------------------------------------------
class Registration {
private:
    int regId;
    int eventId;
    string studentName;
    string rollNo;
    string department;
    string email;
    string regDate;

public:
    Registration() : regId(0), eventId(0) {}

    Registration(int rId, int eId, string sName, string roll,
                 string dept, string mail, string date)
        : regId(rId), eventId(eId), studentName(sName), rollNo(roll),
          department(dept), email(mail), regDate(date) {}

    // Getters
    int getRegId() const { return regId; }
    int getEventId() const { return eventId; }
    string getStudentName() const { return studentName; }
    string getRollNo() const { return rollNo; }
    string getDepartment() const { return department; }
    string getEmail() const { return email; }
    string getRegDate() const { return regDate; }

    // Setters
    void setRegId(int id) { regId = id; }
    void setEventId(int id) { eventId = id; }
    void setStudentName(string n) { studentName = n; }
    void setRollNo(string r) { rollNo = r; }
    void setDepartment(string d) { department = d; }
    void setEmail(string e) { email = e; }
    void setRegDate(string d) { regDate = d; }

    string toJSON() const {
        string j = "{";
        j += "\"regId\":" + to_string(regId) + ",";
        j += "\"eventId\":" + to_string(eventId) + ",";
        j += "\"studentName\":\"" + studentName + "\",";
        j += "\"rollNo\":\"" + rollNo + "\",";
        j += "\"department\":\"" + department + "\",";
        j += "\"email\":\"" + email + "\",";
        j += "\"regDate\":\"" + regDate + "\"";
        j += "}";
        return j;
    }
};

#endif
