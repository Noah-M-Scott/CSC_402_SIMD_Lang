# CSC 402: MENTORED RESEARCH, SIMD LANGUAGE AND COMPILER

A Proof of concept language for implementing SIMD concepts into a typed language.

Use the compile.sh script to run the demo.

This class had two focuses: to research SIMD implementations, and to act as the 
replacement for the compiler's class, which has not been taught for several years.

Provided is a compiler that covers the majority of the specified language;
however the backend is very buggy, and may often generate incorrect code.

Provided is a demo file with code that it will compile correctly.

The compiler is broken into two ends, a front end and a back end.
The front end uses Flex+Bison to generate a directed graph reflecting the syntax.
The back end traverses that graph, and produces assembler snipets as it goes.

Overall, I feel that I ultimatly ran into a time crunch, and it's much
less pretty/functional than I wanted it to be.
But I feel that the class still met it's goals: we researched SIMD implementations,
and I learned how to write a compiler.
