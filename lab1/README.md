## UID: 605954269

## Pipe Up

This program simulates the pipe (|) operator, allowing users to pass the output of one command as input to the next in sequence.

## Building

To build your program, run:

make

## Running

An example of running the program could be:

./pipe ls cat wc

And the expected outcome should be the same outcome as running ls | cat |wc -- my output was:

7       7      63

## Cleaning up

To clean up the program run:

make clean
