main: main.c
	gcc -Wall -o main main.c -lcyaml


clean:
	rm main 