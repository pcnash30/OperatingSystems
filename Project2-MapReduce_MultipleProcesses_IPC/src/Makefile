mapreduce: main.o phase1.o phase2.o phase3.o phase4.o
		gcc -o mapreduce main.o phase1.o phase2.o phase3.o phase4.o
main.o: main.c main.h
		gcc -c main.c
phase1.o: phase1.c phase1.h
		gcc -c phase1.c
phase2.o: phase2.c phase2.h
		gcc -c phase2.c
phase3.o: phase3.c phase3.h
		gcc -c phase3.c
phase4.o: phase4.c phase4.h
		gcc -c phase4.c
clean:
	rm *.o mapreduce
