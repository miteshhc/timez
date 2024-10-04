# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++11

# Source and target
SRCDIR = src
SOURCES = $(SRCDIR)/main.cpp $(SRCDIR)/timez.cpp $(SRCDIR)/utils.cpp $(SRCDIR)/args.cpp
TARGET = timez
DESTDIR = /usr/local

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

install: $(TARGET)
	install -Dm755 $(TARGET) $(DESTDIR)/bin/$(TARGET)

clean:
	rm -f $(TARGET)
