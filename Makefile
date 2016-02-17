CC = gcc

# DEBUGFLAGS is the same as CFLAGS except it doesn't have "-O3"
CFLAGS = -std=gnu99 -lm -lpng -Wall -g -O3 -march=native -flto -Wextra -pedantic
DEBUGFLAGS = -std=gnu99 -lm -lpng -Wall -g -march=native -flto -Wextra -pedantic

TARGET = voronoi

all: $(TARGET)

debug: $(TARGET)-debug

$(TARGET)-debug: $(TARGET).c
	# THIS IS A DEBUG BUILD!
	$(CC) $(DEBUGFLAGS) -o $(TARGET) $(TARGET).c

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET)
