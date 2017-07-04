CXX = clang++
CXXFLAGS = `llvm-config --cxxflags` -Wall -MMD -I.
LDFLAGS = `llvm-config --ldflags`

SRC = $(shell find . -name "*.cpp")
OBJ = $(SRC:.cpp=.o)
DEP = $(SRC:.cpp=.d)

default: debug

debug: CXXFLAGS += -O0 -g
debug: c

release: CXXFLAGS += -O3 -DNDEBUG -Werror
release: c

c: $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f c $(OBJ) $(DEP)

# Optimize build with auto-generated dependencies.
-include $(SRC:%.cpp=%.d)
