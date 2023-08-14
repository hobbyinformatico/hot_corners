CXX = g++
CXXFLAGS = -Wall -O2
LIBS = -lX11 -ljsoncpp

SRCS = mouse_position.cpp
OBJS = $(SRCS:.cpp=.o)
EXECUTABLE = mouse_position

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(EXECUTABLE) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) $(EXECUTABLE)
