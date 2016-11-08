
OBJS	= main.o graph.o buffer.o node_index.o list_node.o queue.o 
SOURCE	= main.c graph.cpp buffer.cpp node_index.cpp list_node.cpp hashtable.cpp queue.cpp
HDRS	= graph.h buffer.h node_index.h list_node.h queue.h types.h hashtable.h
OUT		= gsp
CC		= g++
FLAGS 	= -g -c


all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)

main.o: main.cpp graph.h
	$(CC) $(FLAGS) main.cpp

graph.o: graph.cpp graph.h queue.h hashtable.h hashtable.cpp
	$(CC) $(FLAGS) graph.cpp

buffer.o: buffer.cpp buffer.h list_node.h
	$(CC) $(FLAGS) buffer.cpp

node_index.o: node_index.cpp node_index.h buffer.h hashtable.h hashtable.cpp
	$(CC) $(FLAGS) node_index.cpp

list_node.o: list_node.cpp list_node.h types.h
	$(CC) $(FLAGS) list_node.cpp

queue.o: queue.cpp queue.h
	$(CC) $(FLAGS) queue.cpp

clean:
	rm -f $(OBJS) $(OUT)

count:
	wc $(SOURCE) $(HDRS)