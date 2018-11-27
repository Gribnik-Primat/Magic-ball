CC=g++
FLAGS=-Wall -Werror
SRC=$(shell find . -name "*.cpp")
CONN_SRC=$(shell find . -name "conn_*.cpp")
CONN_PREFIX=$(CONN_SRC:./conn_%=host_%)
CONN=$(CONN_PREFIX:%.cpp=%)
OBJ=$(SRC:.cpp=.o)
all: clean build

%.o: %.cpp $(CC) $(FLAGS) -c $<

BUILDHOST=$(CC) $(CFLAGS) -pthread connsyncp.o client.o host.o
host_%: $(OBJ) 
	$(BUILDHOST) conn_$*.o -o $@ -lrt
build: $(CONN)

clean:
	rm -f host_*
	rm -f $(OBJ)
