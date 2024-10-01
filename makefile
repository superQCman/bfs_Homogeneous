# Project environment
# SIMULATOR_ROOT, defined by setup_env.sh
BENCHMARK_ROOT=$(SIMULATOR_ROOT)/benchmark/bfs

# Compiler environment of C/C++
CC=g++
CFLAGS=-Wall -Werror -g -I$(SIMULATOR_ROOT)/interchiplet/includes
LDFLAGS=-lrt -lpthread
INTERCHIPLET_C_LIB=$(SIMULATOR_ROOT)/interchiplet/lib/libinterchiplet_c.a

# C/C++ Source files
BFS_SRCS= bfs.cpp 
BFS_BLOCK_SRCS= bfs_block.cpp kernel.cpp

# Object files
BFS_OBJS=$(patsubst %.cpp, obj/%.o, $(BFS_SRCS))
BFS_BLOCK_OBJS=$(patsubst %.cpp, obj/%.o, $(BFS_BLOCK_SRCS))

# Targets
BFS_TARGET=bin/bfs
BFS_BLOCK_TARGET=bin/bfs_block

all: bin_dir obj_dir bfs bfs_block

# BFS target
bfs: $(BFS_OBJS)
	$(CC) $(BFS_OBJS) $(INTERCHIPLET_C_LIB) $(LDFLAGS) -o $(BFS_TARGET)

# BFS Block target
bfs_block: $(BFS_BLOCK_OBJS)
	$(CC) $(BFS_BLOCK_OBJS) $(INTERCHIPLET_C_LIB) $(LDFLAGS) -o $(BFS_BLOCK_TARGET)

# Rule for BFS object
obj/%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Directory for binary files.
bin_dir:
	mkdir -p bin

# Directory for object files for C.
obj_dir:
	mkdir -p obj

# Clean generated files.
clean:
	rm -rf bench.txt delayInfo.txt buffer* message_record.txt
	rm -rf proc_r*_t* *.log
clean_all: clean
	rm -rf obj bin

run:
	../../interchiplet/bin/interchiplet bfs.yaml