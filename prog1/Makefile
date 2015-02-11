#
#	Makefile for OpenGL example from Chapter 1 in Red Book
#

CC = g++ 
CFLAGS = -D_DEBUG
CPPFLAGS = -g

.cpp.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@  $<

LIBS = -lGL -lGLU -lGLEW -lglut

SRCS = triangle.cpp LoadShaders.cpp
OBJS = triangle.o LoadShaders.o

triangle: $(OBJS)
	g++ -g -o triangle $(OBJS) $(LIBS)

clean:
	rm -f triangle *.o
