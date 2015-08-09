TARGET = GRAVITON
CC     = cc
CFLAGS = -Wall -O3 -ffast-math -g `sdl-config --cflags`
LIBS   = -lm `sdl-config --libs` -lSDL_image
DEFS   = -DDEBUG
OBJ    = main.o vector.o ray.o object.o light.o font.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(LIBS) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) -c $(CFLAGS) $? $(DEFS)

clean:
	rm -f $(OBJ) $(TARGET) *~

