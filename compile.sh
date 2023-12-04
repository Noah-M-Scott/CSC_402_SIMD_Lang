#!/bin/bash

flex frontEnd/lexFile.l
bison frontEnd/parserFile.y
gcc -g parserFile.tab.c -o vComp.out
rm parserFile.tab.c
rm lex.yy.c
