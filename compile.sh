#!/bin/bash

flex frontEnd/lexFile.l
bison frontEnd/parserFile.y
gcc -g main.c parserFile.tab.c middleEnd/converterDAG2TAC.c -o vComp.out
rm parserFile.tab.c
rm lex.yy.c
