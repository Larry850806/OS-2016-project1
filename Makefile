CFLAG = -DDEBUG -D_POSIX_C_SOURCE=199309L -Wall -std=c99

main: main.o scheduler.o task.o util.o
	gcc $(CFLAG) main.o scheduler.o task.o util.o -o main
main.o: main.c Makefile
	gcc $(CFLAG) main.c -c
scheduler.o: scheduler.c scheduler.h Makefile
	gcc $(CFLAG) scheduler.c -c
task.o: task.c task.h Makefile
	gcc $(CFLAG) task.c -c
util.o: util.c util.h Makefile
	gcc $(CFLAG) util.c -c
clean:
	rm -rf *o
run:
	sudo ./main
