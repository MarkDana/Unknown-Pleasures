all: simple_shell.o simple_shell_origin.o 
	gcc -o simple_shell simple_shell.o 
	gcc -o simple_shell_origin simple_shell_origin.o 
simple_shell.o: simple_shell.c
	gcc -c simple_shell.c
simple_shell_origin.o: simple_shell_origin.c
	gcc -c simple_shell_origin.c
clean:
	rm *.o
	rm simple_shell_origin
	rm simple_shell
