CXX = g++
CXXFLAGS = -std=c++14 -Wall -MMD -I.

SRC = $(shell find . -name "*.cpp")
OBJ = $(SRC:.cpp=.o)

default: debug

debug: CXXFLAGS += -O0 -g
debug: c

release: CXXFLAGS += -O3 -DNDEBUG -Werror
release: c

c: $(OBJ)
	$(CXX) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f c *.o *.d

# Optimize build with auto-generated dependencies.
-include $(SRC:%.cpp=%.d)
