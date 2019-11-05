# Impartial-Engine
Image processing pipeline for public performance and jammimg
I have just dumped some code here to get started.

The impartial engine is a basic pipeline of functions that operate on a stream of images. The purpose of it is to create art out of the srream.

It does not use GPUs or any sensible clever things to do the matrix operations but just chuggs along on the CPU.

There are three basic programs that communicate with each other via a chunk of shared memory.

Loader - loads up the jpegs into RAM. Needs a control file to give a list of the files.
Control - controls what goes on
Display - Does the displaying into an X window

font/merge creates fonts/sprites for the engine from a picture of a grid of them.

Requires suffient RAM on the hold the processed jpegs. About 2-3 hundred per GB depending on screen resolution.
