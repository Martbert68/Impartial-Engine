# Impartial-Engine
Image processing pipeline for public performance and jammimg
I have just dumped some code here to get started.

The impartial engine is a basic pipeline of functions that operate on a stream of images. The purpose of it is to create art out of the srream.

It does not use GPUs or any sensible clever things to do the matrix operations but just chuggs along on the CPU.

There are three basic programs that communicate with each other via a chunk of shared memory.

Loader - loads up the jpegs into RAM
Control - controls what goes on
Display - Does the displaying

Other programs can create fonts for the ngine from a picture.
