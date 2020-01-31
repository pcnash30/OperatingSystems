client: src/client.c src/phase1.c src/phase2.c
	gcc -Wall -std=c99 -pthread -D_BSD_SOURCE -D_DEFAULT_SOURCE -Iinclude -o client src/client.c src/phase1.c src/phase2.c

clean:
	rm client
	rm -rf log
	rm -rf MapperInput
