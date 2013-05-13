# 
CC=g++
# 
CFLAGS=-Wall -lwiringPi

all: max7219
	ls -lh --color=auto

run: max7219
	ls -lh --color=auto
	./max7219 -DEBUG

runInit: max7219
	ls -lh --color=auto
	./max7219 -init -DEBUG

max7219: max7219.cpp
	$(CC) $(CFLAGS) max7219.cpp -o max7219

clean:
	rm -rf *o max7219
	ls -lh --color=auto
