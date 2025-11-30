CXX = g++
CXXFLAGS = -std=c++11 -I.. -I../ARK

# Source files
SRCS = IntegrationTests.cpp \
       ../ARK/Core/FlightSystem.cpp \
       ../ARK/Core/StateMachine.cpp

# Output binary
TARGET = run_tests

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)
