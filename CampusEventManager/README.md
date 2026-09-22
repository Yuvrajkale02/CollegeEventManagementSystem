# Campus Circular — College Event Management System

A full-stack mini-project built for the **Object Oriented Programming using C++**
course (Diploma, Second Year). The C++ backend is the heart of the project —
it owns all the OOP design, talks directly to a **MySQL** database, and
exposes a small REST API that a plain **HTML/CSS/JavaScript** frontend
consumes.

```
Browser (HTML/CSS/JS)  <-- HTTP/JSON -->  C++ Server (cpp-httplib)  <-- MySQL C API -->  MySQL Database
      frontend/                              backend/                                  database/schema.sql
```

---

## 1. Folder structure

```
CampusEventManager/
├── backend/
│   ├── include/            <- all class headers (the OOP core)
│   │   ├── Exceptions.h     custom exception classes
│   │   ├── Person.h         abstract base class
│   │   ├── Student.h        derived class
│   │   ├── Organizer.h      derived class
│   │   ├── Event.h          event entity
│   │   ├── Registration.h   registration entity
│   │   ├── Database.h       Singleton MySQL connection wrapper
│   │   ├── EventManager.h   business logic layer
│   │   └── httplib.h        3rd-party single-header HTTP server library
│   ├── src/
│   │   └── main.cpp         HTTP routes -> glue between DB and frontend
│   └── Makefile
├── frontend/
│   ├── index.html
│   ├── style.css
│   └── script.js
├── database/
│   └── schema.sql           run this first
└── README.md
```

---

## 2. One-time setup

### Step 1 — Install MySQL and the C++ dev headers

**Windows (XAMPP):** install XAMPP and start the **MySQL** module from the
XAMPP control panel. Its default root user has no password, which matches
the defaults already set in `Database.h`.

**Linux:**
```bash
sudo apt-get install mysql-server libmysqlclient-dev g++ make
sudo service mysql start
```

### Step 2 — Create the database

Import the schema (creates the database, 3 tables, a default admin login,
and 3 sample events):

```bash
mysql -u root < database/schema.sql
```
*(In phpMyAdmin: open the SQL tab and paste the contents of `schema.sql`.)*

### Step 3 — Check the connection settings

Open `backend/include/Database.h` and confirm `HOST`, `USER`, `PASSWORD`
match your MySQL setup (defaults work out-of-the-box with XAMPP).

### Step 4 — Build and run the backend

```bash
cd backend
make          # compiles backend/src/server
make run      # starts the server on http://localhost:8080
```

### Step 5 — Open the app

Visit **http://localhost:8080** in a browser. The C++ server itself serves
the frontend files, so nothing else needs to run.

- **Students:** browse the notice board, click *Register* on any event.
- **Organizer/Admin:** click *Organizer Desk* → sign in with
  `admin` / `admin123` → post, edit, delete events and view registrations.

---

## 3. OOP concepts used (CO mapping — 22316 Object Oriented Programming using C++)

| CO | Concept | Where it's implemented |
|----|---------|-------------------------|
| **CO1** | Classes & Objects, Encapsulation | Every entity (`Event`, `Student`, `Organizer`, `Registration`, `Database`, `EventManager`) keeps its data `private`/`protected` and exposes it only through getters/setters. |
| **CO1** | Abstraction | `Person` is an **abstract class** — it declares pure virtual functions (`displayInfo()`, `getRole()`) and can never be instantiated directly. |
| **CO2** | Constructors & Destructors | Default + parameterized constructors in `Event`; constructor chaining in `Student`/`Organizer` (`Student(...) : Person(n, e), ...`); every class has a destructor. |
| **CO2** | Constructor Overloading | `Event` has both a default constructor and a parameterized constructor. |
| **CO3** | Inheritance | `Student : public Person` and `Organizer : public Person` — single-level inheritance from a common base. |
| **CO3** | Runtime Polymorphism | `displayInfo()` and `getRole()` are declared `virtual` in `Person` and `override`n in `Student`/`Organizer` — the correct version runs based on the actual object type. |
| **CO3** | Function Overloading | `EventManager::getEvents()` (no args → all events) and `EventManager::getEvents(string category)` (filtered) — same name, different signatures. |
| **CO3** | Operator Overloading | `Event::operator==` (compares two events by ID) and `operator<<` as a **friend function** (lets you `cout << event` directly). |
| **CO4** | File Handling (fstream) | `EventManager::logActivity()` appends every add/update/delete/register/login action to `activity_log.txt` using `ofstream`. |
| **CO4** | Exception Handling | Custom exception hierarchy in `Exceptions.h` (`EventNotFoundException`, `SeatsFullException`, `DuplicateRegistrationException`, `InvalidCredentialsException`, `DatabaseConnectionException`, `InvalidInputException`), all deriving from `std::exception` and overriding `what()`. Every risky operation in `EventManager` is wrapped in `try/catch` at the API boundary in `main.cpp`. |
| — | Design Pattern (Singleton) | `Database` uses a private constructor + static `getInstance()` so only **one** MySQL connection object ever exists — commonly taught alongside OOP as a real-world application of encapsulation + static members. |
| — | Database Connectivity | `Database.h` connects to MySQL using the **MySQL C API** (`mysql.h`); `EventManager` builds and executes real SQL (`SELECT`/`INSERT`/`UPDATE`/`DELETE`) against the `college_event_db` schema. |

---

## 4. REST API reference (for viva / documentation)

| Method | Endpoint | Purpose |
|--------|----------|---------|
| GET | `/api/events` | List all events (optional `?category=` filter) |
| GET | `/api/events/:id` | Get one event |
| POST | `/api/events` | Add an event (admin) |
| PUT | `/api/events/:id` | Update an event (admin) |
| DELETE | `/api/events/:id` | Delete an event (admin) |
| POST | `/api/register` | Student registers for an event |
| GET | `/api/registrations` | List registrations (optional `?eventId=` filter) |
| POST | `/api/admin/login` | Admin login |

---

## 5. Likely viva questions & quick answers

- **Why is `Person` abstract?** It models a concept ("a person in the
  system") that should never exist on its own — only concrete roles
  (`Student`, `Organizer`) should. Pure virtual functions enforce this.
- **Where's polymorphism actually visible at runtime?** If you held a
  `Person*` pointing to a `Student` or an `Organizer` and called
  `displayInfo()`, the correct overridden version runs — decided at
  runtime via the vtable, not at compile time.
- **Why Singleton for `Database`?** The whole app should share exactly one
  MySQL connection instead of opening a new one per request.
- **Why custom exceptions instead of just returning error codes?**
  Exceptions separate error-handling code from normal logic and let each
  error type carry its own message and be caught selectively.
- **Where's file handling if you're using MySQL?** `activity_log.txt` is
  written with `ofstream` independently of the database, satisfying the
  file-handling CO while MySQL handles persistent structured data.

---

## 6. Notes

- Passwords are stored in plain text in the `admin` table for simplicity —
  call this out as a known limitation if asked, and mention hashing
  (e.g. bcrypt) as the production-grade fix.
- SQL values are escaped with `mysql_real_escape_string` before being
  placed in queries to prevent basic SQL injection.
