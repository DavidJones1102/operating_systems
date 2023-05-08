CC = gcc

all: main barber client
main:
	${CC} main.c -lpthread -lrt -o main.exe 
barber: 
	${CC} barber.c -lpthread -lrt -o barber.exe 
client:
	${CC} client.c -lpthread -lrt -o client.exe 
clean:
	rm -f *.exe