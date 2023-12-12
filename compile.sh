#!/bin/bash

flex frontEnd/lexFile.l
bison frontEnd/parserFile.y
gcc -g main.c parserFile.tab.c backEnd/createASM.c -o vComp.out
rm parserFile.tab.c
rm lex.yy.c
./vComp.out < demo.txt
gcc demoC.c out.s -o demo
echo demo output:
./demo