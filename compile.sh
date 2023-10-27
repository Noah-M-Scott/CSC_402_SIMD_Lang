#!/bin/bash

flex lexFile.l
bison parserFile.y
gcc parserFile.tab.c
