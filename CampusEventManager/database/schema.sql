-- ============================================================
-- College Event Management System - Database Schema
-- Run this file in MySQL / phpMyAdmin (XAMPP) BEFORE starting
-- the C++ server.
-- ============================================================

CREATE DATABASE IF NOT EXISTS college_event_db;
USE college_event_db;

-- Admin / Organizer table
CREATE TABLE IF NOT EXISTS admin (
    admin_id INT PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(50) UNIQUE NOT NULL,
    password VARCHAR(50) NOT NULL
);

-- Events table
CREATE TABLE IF NOT EXISTS events (
    event_id INT PRIMARY KEY AUTO_INCREMENT,
    title VARCHAR(100) NOT NULL,
    description VARCHAR(500),
    category VARCHAR(50),
    event_date DATE NOT NULL,
    venue VARCHAR(100),
    total_seats INT NOT NULL DEFAULT 0,
    seats_filled INT NOT NULL DEFAULT 0
);

-- Registrations table (links a student to an event)
CREATE TABLE IF NOT EXISTS registrations (
    reg_id INT PRIMARY KEY AUTO_INCREMENT,
    event_id INT NOT NULL,
    student_name VARCHAR(100) NOT NULL,
    roll_no VARCHAR(20) NOT NULL,
    department VARCHAR(50),
    email VARCHAR(100),
    reg_date DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (event_id) REFERENCES events(event_id) ON DELETE CASCADE
);

-- Default admin login -> username: admin | password: admin123
INSERT INTO admin (username, password) VALUES ('admin', 'admin123');

-- Sample events so the frontend isn't empty on first run
INSERT INTO events (title, description, category, event_date, venue, total_seats, seats_filled) VALUES
('Tech Fest 2026', 'Annual technical festival with coding and robotics competitions.', 'Technical', '2026-10-15', 'Main Auditorium', 100, 0),
('Cultural Night', 'Music, dance and drama performances by students.', 'Cultural', '2026-10-20', 'Open Air Theatre', 200, 0),
('Sports Meet', 'Inter-department annual sports competition.', 'Sports', '2026-11-01', 'College Ground', 150, 0);
