CC=gcc
display: display.c martin.h 
	$(CC) -o display display.c -l X11 -lpthread -lm

loader: loader.c martin.h 
	$(CC) -o loader loader.c -ljpeg 

control: control.c martin.h 
	$(CC) -o control control.c -lpthread 

font: font.c 
	$(CC) -o font font.c -ljpeg -l X11

show: show.c 
	$(CC) -o show show.c -l X11

merge: merge.c 
	$(CC) -o merge merge.c -l X11
flicker: flicker.c 
	$(CC) -o flicker flicker.c -l X11 -lm -ljpeg
all: control loader display
