#!/bin/bash

flex frontEnd/lexFile.l
bison frontEnd/parserFile.y
gcc parserFile.tab.c -o vComp.out
rm parserFile.tab.c
rm lex.yy.c
