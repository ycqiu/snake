CXX = g++
LD = g++
CXXFLAGS = -g -Wall
LDFLAGS = 
SRC_FILE = $(wildcard *.cpp)
OBJ_FILE = $(patsubst %.cpp, %.o, $(SRC_FILE))

all: hello

snake: $(OBJ_FILE) 
	$(LD) $(LDFLAGS) $^ -o $@


#生成依赖文件
%.d : %.cpp
	$(CXX) $(CXXFLAGS) -M $^  > $@

#include $(SRC_FILE:.cpp=.d) 

print:
	@echo $(SRC_FILE)
	@echo $(OBJ_FILE)

clean:
	rm -f *.o
	rm -f *.d
	rm -f hello

.PHONY: print clean
