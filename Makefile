CC := g++
CFLAGS := -std=c++20 -I/usr/local/Cellar/botan/3.1.1/include/botan-3/
LDFLAGS := -L/usr/local/Cellar/botan/3.1.1/lib/ -lbotan-3

# List of source files
SRCS := sample_app.cpp

# List of object files generated from source files
OBJS := $(SRCS:.cpp=.o)

# The target executable
TARGET := sample_app

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)