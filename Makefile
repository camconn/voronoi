CC = gcc

# compiler flags:
# -std=gnu99      use c99 standard with gnu extensions
# -lm             use math.h library
# -lpng           use libpng library
# -Wall           enable all warnings
# -O3             optimize for speed
# -march=native   use processor-specific extensions (e.g. SSE2)
CFLAGS = -std=gnu99 -lm -lpng -Wall -O3 -march=native

TARGET = voronoi

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET)
