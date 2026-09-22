# ============================================================
# Makefile - College Event Management System (Backend)
# ------------------------------------------------------------
# Usage:
#   make          -> builds ./bin/server
#   make run      -> builds (if needed) and runs the server
#   make clean    -> removes build files
# ============================================================

CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude -I/usr/include/mysql
LDFLAGS  = -lmysqlclient -lpthread
SRC      = src/main.cpp
TARGET   = src/server

# NOTE: the server is run with its working directory set to backend/src/
# (see the 'run' target below) because main.cpp mounts the frontend
# folder using the relative path "../../frontend". Keep this in mind if
# you run the binary manually from a different directory.

$(TARGET): $(SRC) include/*.h
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)
	@echo "Build successful -> $(TARGET)"

run: $(TARGET)
	cd src && ./server

clean:
	rm -f src/server src/activity_log.txt

.PHONY: run clean
