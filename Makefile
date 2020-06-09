#Makefile
all: main

#Compilacion programa
main: main.o funciones.o
	gcc -o main main.o funciones.o -lm
main.o: main.c funciones.h
	gcc -c main.c
funciones.o: funciones.c funciones.h
	gcc -c funciones.c
clean:
	rm -f main *.o
