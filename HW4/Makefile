
CFLAGS=  $(shell  sdl-config --cflags) -g -I/opt/local/include -I. 
LDFLAGS= -g $(shell sdl-config --libs) -lSDL_gfx -lSDLmain -lm 
CC=gcc

OBJS = c335sim.o pong.o screen.o glcdfont.o rect.o
 
all: pong 

pong: $(OBJS) 
	$(CC) $(OBJS) -o pong $(LDFLAGS)
clean:
	rm -f $(OBJS) pong 

