#ifndef DATABASE_H
#define DATABASE_H

#include <mysql/mysql.h>
#include <iostream>
#include <string>
#include "Exceptions.h"
using namespace std;

// ---------------------------------------------------------------------
// Database class
// OOP Concepts demonstrated:
//   - Singleton Design Pattern (only ONE connection object exists)
//   - Encapsulation (raw MYSQL* pointer is hidden, only exposed via
//     a controlled getConnection() function)
//
// Change the constants below to match YOUR local MySQL/XAMPP setup.
// ---------------------------------------------------------------------
class Database {
private:
    MYSQL *conn;
    static Database *instance;   // the single shared instance

    // Connection settings - EDIT THESE for your system
    const char *HOST     = "127.0.0.1";
    const char *USER     = "root";
    const char *PASSWORD = "";              // XAMPP default is empty password
    const char *DBNAME   = "college_event_db";
    const unsigned int PORT = 3306;

    // Private constructor -> nobody can create a Database object directly
    Database() {
        conn = mysql_init(nullptr);
        if (conn == nullptr) {
            throw DatabaseConnectionException("mysql_init() failed");
        }

        if (mysql_real_connect(conn, HOST, USER, PASSWORD, DBNAME, PORT, nullptr, 0) == nullptr) {
            string err = mysql_error(conn);
            mysql_close(conn);
            conn = nullptr;
            throw DatabaseConnectionException(err);
        }

        cout << "[Database] Connected to MySQL database '" << DBNAME << "' successfully." << endl;
    }

public:
    // Deleting copy constructor/assignment -> enforces Singleton
    Database(const Database &) = delete;
    Database &operator=(const Database &) = delete;

    static Database *getInstance() {
        if (instance == nullptr) {
            instance = new Database();
        }
        return instance;
    }

    MYSQL *getConnection() {
        return conn;
    }

    ~Database() {
        if (conn != nullptr) {
            mysql_close(conn);
            cout << "[Database] Connection closed." << endl;
        }
    }
};

// Static member definition (inline so it's safe to include this header
// in more than one .cpp file without "multiple definition" linker errors)
inline Database *Database::instance = nullptr;

#endif
