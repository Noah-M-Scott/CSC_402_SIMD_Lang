#include <stdlib.h>
#include <stdio.h>
#include "frontEndDefs.h"

struct genericNode** DAGin;

FILE outFile;

//reserve r15 as vecLoop counter and helper register
//reserve xmm0 as a helper register

//used for typing the data literals
void lastType(struct genericNode* in){

}

//how many elements are in a xmm register of in's type
int  elementCount(struct genericNode* in){

}

//keeps a list of registers for use, what address a currently using them
char* getReg(struct genericNode* me, struct genericNode* mine){

}

//turns local variables into offset(%rbp), or if it's a global variable name(%rip)
char* getSymbol(struct genericNode* in){

}

//get the type prefix/trailing type
char getType(struct genericNode* in){

}

//is the trailing type a int (this counts pointers)
long isInt(struct genericNode* in){

}

//is the trailing type a int (this counts pointers)
long isPointer(struct genericNode* in){

}

//is the trailing type a float
long isFloat(struct genericNode* in){
    
}

//is the trailing type vector, returns size
long isVec(struct genericNode* in){
    
}

long labelCounter;
long stackOffsetCounter;
long scopeCounter;
long lastWasVec;


void symbolNode(struct genericNode* in){ //if a symbol isn't handled, it's a definition

}

void labelNode(struct genericNode* in){}

void gotoNode(struct genericNode* in){}


void scopeNode(struct genericNode* in){
    scopeCounter++;
    for(int i = 0; i < in->childCount; i++)
        generalNode(in->children[i]);

    scopeCounter--;
    free(in);
}

long loopLabelStackTop[512];
long loopLabelStackExit[512];
long loopLabelPointerTop = 0;
long loopLabelPointerExit = 0;

void forNode(struct genericNode* in){

    generalNode(in->children[0]);

    long tempLabel = labelCounter++;
    long exitLabel = labelCounter++;

    loopLabelStackTop[++loopLabelPointerTop] = tempLabel;
    loopLabelStackExit[++loopLabelPointerExit] = exitLabel;

    fprintf(outFile, "L%d:\n", tempLabel);

    generalNode(in->children[1]);

    fprintf(outFile, "movq   %s, \%r15\n", getReg(in, in->children[1]));  //movq handles both floats and clearing short ints
    fprintf(outFile, "cmpq   $0, \%r15\n");
    fprintf(outFile, "jz    L%d\n", exitLabel);

    generalNode(in->children[2]);

    fprintf(outFile, "jmp   L%d\n", tempLabel);
    fprintf(outFile, "L%d:\n", exitLabel);

    loopLabelPointerTop--;
    loopLabelPointerExit--;

    free(in);
}

void whileNode(struct genericNode* in){
    
    long tempLabel = labelCounter++;
    long exitLabel = labelCounter++;

    loopLabelStackTop[++loopLabelPointerTop] = tempLabel;
    loopLabelStackExit[++loopLabelPointerExit] = exitLabel;

    fprintf(outFile, "L%d:\n", tempLabel);

    generalNode(in->children[0]);

    fprintf(outFile, "movq   %s, \%r15\n", getReg(in, in->children[0]));  //movq handles both floats and clearing short ints
    fprintf(outFile, "cmpq   $0, \%r15\n");
    fprintf(outFile, "jz    L%d\n", exitLabel);

    generalNode(in->children[1]);

    fprintf(outFile, "jmp   L%d\n", tempLabel);
    fprintf(outFile, "L%d:\n", exitLabel);

    loopLabelPointerTop--;
    loopLabelPointerExit--;

    free(in);
}

void continueNode(struct genericNode* in){
    fprintf(outFile, "jmp   L%d\n", loopLabelStackTop[loopLabelPointerTop]);
}

void breakNode(struct genericNode* in){
    fprintf(outFile, "jmp   L%d\n", loopLabelStackExit[loopLabelPointerExit]);
}

void ifNode(struct genericNode* in){

    generalNode(in->children[0]);

    long tempLabel = labelCounter++;

    fprintf(outFile, "movq   %s, \%r15\n", getReg(in, in->children[0]));  //movq handles both floats and clearing short ints
    fprintf(outFile, "cmpq   $0, \%r15\n");
    fprintf(outFile, "jz    L%d\n", tempLabel);

    generalNode(in->children[1]);

    fprintf(outFile, "L%d:\n", tempLabel);
    free(in);
}

void ifElseNode(struct genericNode* in){
    generalNode(in->children[0]);

    long tempLabel = labelCounter++;
    long tempLabelElse = labelCounter++;

    fprintf(outFile, "movq   \%%s, \%r15\n", getReg(in, in->children[0]));  //movq handles both floats and clearing short ints
    fprintf(outFile, "cmpq   $0, \%r15\n");
    fprintf(outFile, "jz    L%d\n", tempLabel);

    generalNode(in->children[1]);

    fprintf(outFile, "jmp   %d\nL%d:\n", tempLabelElse, tempLabel);

    generalNode(in->children[2]);

    fprintf(outFile, "L%d:\n", tempLabelElse);
    free(in);
}

void derefNode(struct genericNode* in){}
void equNode(struct genericNode* in){}

void ternNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        long openner = 0;
        long tempLabel;
        if(lastWasVec == 0){
            tempLabel = labelCounter++;
            openner = 1;
            fprintf(outFile, "movq  $%ld, \%r15\nL%ld:\n", vectorSize, tempLabel);
        }
        lastWasVec = 1;

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "pmov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "pmov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "padd%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "padd%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movp%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movp%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "addp%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "addp%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

        if(openner)
            fprintf(outFile, "subq $%ld, \%r15\njns L%ld\n", elementCount(in), tempLabel);

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        lastWasVec = 0;
        
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "add%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "add%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "adds%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "adds%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
    }
}

void logicOrNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        long openner = 0;
        long tempLabel;
        if(lastWasVec == 0){
            tempLabel = labelCounter++;
            openner = 1;
            fprintf(outFile, "movq  $%ld, \%r15\nL%ld:\n", vectorSize, tempLabel);
        }
        lastWasVec = 1;

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqu %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movdqa %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "por %s, %s\n", getSymbol(in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor \%xmm0, \%xmm0\n");
                fprintf(outFile, "pcmpeq%c \%xmm0, %s\n", getType(in), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "por %s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor \%xmm0, \%xmm0\n");
                fprintf(outFile, "pcmpeq%c \%xmm0, %s\n", getType(in), getReg(in, in));
            }

            fprintf(outFile, "pcmpeqb    \%xmm0, \%xmm0\n");
            fprintf(outFile, "pxor \%xmm0, %s\n", getReg(in, in));

        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "por %s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor \%xmm0, \%xmm0\n");
                fprintf(outFile, "cmpneqp%c \%xmm0, %s\n", getType(in), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "por %s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor \%xmm0, \%xmm0\n");
                fprintf(outFile, "cmpneqp%c \%xmm0, %s\n", getType(in), getReg(in, in));
            }
        }

        if(openner)
            fprintf(outFile, "subq $%ld, \%r15\njns L%ld\n", elementCount(in), tempLabel);

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        lastWasVec = 0;
        
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "or%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
                fprintf(outFile, "cmp%c $0, %s\nsetne   %s\n", getType(in), getReg(in, in), getByteReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "or%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "cmp%c $0, %s\nsetne   %s\n", getType(in), getReg(in, in), getByteReg(in, in));
            }
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "por %s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor \%xmm0, \%xmm0\n");
                fprintf(outFile, "cmpneqs%c \%xmm0, %s\n", getType(in), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "por %s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor \%xmm0, \%xmm0\n");
                fprintf(outFile, "cmpneqs%c \%xmm0, %s\n", getType(in), getReg(in, in));
            }
        }
    }
}

void logicAndNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        long openner = 0;
        long tempLabel;
        if(lastWasVec == 0){
            tempLabel = labelCounter++;
            openner = 1;
            fprintf(outFile, "movq  $%ld, \%r15\nL%ld:\n", vectorSize, tempLabel);
        }
        lastWasVec = 1;

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqu %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movdqa %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pand %s, %s\n", getSymbol(in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor \%xmm0, \%xmm0\n");
                fprintf(outFile, "pcmpeq%c \%xmm0, %s\n", getType(in), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pand %s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor \%xmm0, \%xmm0\n");
                fprintf(outFile, "pcmpeq%c \%xmm0, %s\n", getType(in), getReg(in, in));
            }

            fprintf(outFile, "pcmpeqb    \%xmm0, \%xmm0\n");
            fprintf(outFile, "pxor \%xmm0, %s\n", getReg(in, in));

        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "andps %s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor \%xmm0, \%xmm0\n");
                fprintf(outFile, "cmpneqp%c \%xmm0, %s\n", getType(in), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "andps %s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor \%xmm0, \%xmm0\n");
                fprintf(outFile, "cmpneqp%c \%xmm0, %s\n", getType(in), getReg(in, in));
            }
        }

        if(openner)
            fprintf(outFile, "subq $%ld, \%r15\njns L%ld\n", elementCount(in), tempLabel);

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        lastWasVec = 0;
        
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "and%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
                fprintf(outFile, "cmp%c $0, %s\nsetne   %s\n", getType(in), getReg(in, in), getByteReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "and%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "cmp%c $0, %s\nsetne   %s\n", getType(in), getReg(in, in), getByteReg(in, in));
            }
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "andps %s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor \%xmm0, \%xmm0\n");
                fprintf(outFile, "cmpneqs%c \%xmm0, %s\n", getType(in), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "andps %s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor \%xmm0, \%xmm0\n");
                fprintf(outFile, "cmpneqs%c \%xmm0, %s\n", getType(in), getReg(in, in));
            }
        }
    }
}

void logicalNotNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        long openner = 0;
        long tempLabel;
        if(lastWasVec == 0){
            tempLabel = labelCounter++;
            openner = 1;
            fprintf(outFile, "movq  $%ld, \%r15\nL%ld:\n", vectorSize, tempLabel);
        }
        lastWasVec = 1;

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqu %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movdqa %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            
            fprintf(outFile, "pxor \%xmm0, \%xmm0\n");
            fprintf(outFile, "pcmpeq%c \%xmm0, %s\n", getType(in), getReg(in, in));
            fprintf(outFile, "pcmpeqb    \%xmm0, \%xmm0\n");
            fprintf(outFile, "pxor \%xmm0, %s\n", getReg(in, in));

        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
            }

            
            fprintf(outFile, "pxor \%xmm0, \%xmm0\n");
            fprintf(outFile, "cmpneqp%c \%xmm0, %s\n", getType(in), getReg(in, in));

        }

        if(openner)
            fprintf(outFile, "subq $%ld, \%r15\njns L%ld\n", elementCount(in), tempLabel);

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        lastWasVec = 0;
        
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            fprintf(outFile, "cmp%c $0, %s\nsetne   %s\n", getType(in), getReg(in, in), getByteReg(in, in));
            
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            fprintf(outFile, "pxor \%xmm0, \%xmm0\n");
            fprintf(outFile, "cmpneqs%c \%xmm0, %s\n", getType(in), getReg(in, in));
        }
    }
}

void notEquNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        long openner = 0;
        long tempLabel;
        if(lastWasVec == 0){
            tempLabel = labelCounter++;
            openner = 1;
            fprintf(outFile, "movq  $%ld, \%r15\nL%ld:\n", vectorSize, tempLabel);
        }
        lastWasVec = 1;

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqu %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movdqa %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pcmpeq%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pcmpeq%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }

            fprintf(outFile, "pcmpeqb    \%xmm0, \%xmm0\n");
            fprintf(outFile, "pxor \%xmm0, %s\n", getReg(in, in));

        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmpneqp%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmpneqp%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

        if(openner)
            fprintf(outFile, "subq $%ld, \%r15\njns L%ld\n", elementCount(in), tempLabel);

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        lastWasVec = 0;
        
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmp%c %s, %s\nsetne   %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in), getByteReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmp%c %s, %s\nsetne   %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in), getByteReg(in, in));
            }
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmpneqs%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmpneqs%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
    }
}

void gtEquNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        long openner = 0;
        long tempLabel;
        if(lastWasVec == 0){
            tempLabel = labelCounter++;
            openner = 1;
            fprintf(outFile, "movq  $%ld, \%r15\nL%ld:\n", vectorSize, tempLabel);
        }
        lastWasVec = 1;

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqu %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movdqa %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqa %s, \%xmm0\n", getReg(in, in));
                fprintf(outFile, "pcmpgt%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
                fprintf(outFile, "pcmpeq%c %s, \%xmm0\n", getType(in), getSymbol(in->children[1]));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "movdqa %s, \%xmm0\n", getReg(in, in));
                fprintf(outFile, "pcmpgt%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pcmpeq%c %s, \%xmm0\n", getType(in), getReg(in, in->children[1]));
            }

            fprintf(outFile, "por \%xmm0, %s\n", getReg(in, in));

        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmpnltp%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmpnltp%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

        if(openner)
            fprintf(outFile, "subq $%ld, \%r15\njns L%ld\n", elementCount(in), tempLabel);

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        lastWasVec = 0;
        
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmp%c %s, %s\nsetge   %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in), getByteReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmp%c %s, %s\nsetge   %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in), getByteReg(in, in));
            }
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmpnlts%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmpnlts%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
    }
}

void ltNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        long openner = 0;
        long tempLabel;
        if(lastWasVec == 0){
            tempLabel = labelCounter++;
            openner = 1;
            fprintf(outFile, "movq  $%ld, \%r15\nL%ld:\n", vectorSize, tempLabel);
        }
        lastWasVec = 1;

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqu %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movdqa %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqa %s, \%xmm0\n", getReg(in, in));
                fprintf(outFile, "pcmpgt%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
                fprintf(outFile, "pcmpeq%c %s, \%xmm0\n", getType(in), getSymbol(in->children[1]));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "movdqa %s, \%xmm0\n", getReg(in, in));
                fprintf(outFile, "pcmpgt%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pcmpeq%c %s, \%xmm0\n", getType(in), getReg(in, in->children[1]));
            }

            fprintf(outFile, "por \%xmm0, %s\n", getReg(in, in));
            fprintf(outFile, "pcmpgeb \%xmm0, \%xmm0\n");
            fprintf(outFile, "pxor \%xmm0, %s\n", getReg(in, in));

        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmpltp%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmpltp%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

        if(openner)
            fprintf(outFile, "subq $%ld, \%r15\njns L%ld\n", elementCount(in), tempLabel);

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        lastWasVec = 0;
        
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmp%c %s, %s\nsetlt   %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in), getByteReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmp%c %s, %s\nsetlt   %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in), getByteReg(in, in));
            }
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmplts%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmplts%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
    }
}

void ltEquNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        long openner = 0;
        long tempLabel;
        if(lastWasVec == 0){
            tempLabel = labelCounter++;
            openner = 1;
            fprintf(outFile, "movq  $%ld, \%r15\nL%ld:\n", vectorSize, tempLabel);
        }
        lastWasVec = 1;

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqu %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movdqa %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pcmpgt%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pcmpgt%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }

            fprintf(outFile, "pcmpeqb    \%xmm0, \%xmm0\n");
            fprintf(outFile, "pxor \%xmm0, %s\n", getReg(in, in));

        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmplep%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmplep%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

        if(openner)
            fprintf(outFile, "subq $%ld, \%r15\njns L%ld\n", elementCount(in), tempLabel);

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        lastWasVec = 0;
        
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmp%c %s, %s\nsetle   %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in), getByteReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmp%c %s, %s\nsetle   %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in), getByteReg(in, in));
            }
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmples%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmples%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
    }
}

void equEquNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        long openner = 0;
        long tempLabel;
        if(lastWasVec == 0){
            tempLabel = labelCounter++;
            openner = 1;
            fprintf(outFile, "movq  $%ld, \%r15\nL%ld:\n", vectorSize, tempLabel);
        }
        lastWasVec = 1;

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqu %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movdqa %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pcmpeq%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pcmpeq%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmpeqp%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmpeqp%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

        if(openner)
            fprintf(outFile, "subq $%ld, \%r15\njns L%ld\n", elementCount(in), tempLabel);

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        lastWasVec = 0;
        
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmp%c %s, %s\nseteq   %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in), getByteReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmp%c %s, %s\nseteq   %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in), getByteReg(in, in));
            }
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmpeqs%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmpeqs%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
    }
}

void gtNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        long openner = 0;
        long tempLabel;
        if(lastWasVec == 0){
            tempLabel = labelCounter++;
            openner = 1;
            fprintf(outFile, "movq  $%ld, \%r15\nL%ld:\n", vectorSize, tempLabel);
        }
        lastWasVec = 1;

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqu %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movdqa %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pcmpgt%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pcmpgt%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmpnlep%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmpnlep%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

        if(openner)
            fprintf(outFile, "subq $%ld, \%r15\njns L%ld\n", elementCount(in), tempLabel);

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        lastWasVec = 0;
        
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            long tempLabel = labelCounter++;

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmp%c %s, %s\nsetgt   %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in), getByteReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmp%c %s, %s\nsetgt   %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in), getByteReg(in, in));
            }
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmpnles%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmpnles%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
    }
}

void orNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        long openner = 0;
        long tempLabel;
        if(lastWasVec == 0){
            tempLabel = labelCounter++;
            openner = 1;
            fprintf(outFile, "movq  $%ld, \%r15\nL%ld:\n", vectorSize, tempLabel);
        }
        lastWasVec = 1;

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "por %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "por %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
        // no floating point and

        if(openner)
            fprintf(outFile, "subq $%ld, \%r15\njns L%ld\n", elementCount(in), tempLabel);

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        lastWasVec = 0;
        
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "or%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "or%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
        // no floating point or
    }
}

void eorNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        long openner = 0;
        long tempLabel;
        if(lastWasVec == 0){
            tempLabel = labelCounter++;
            openner = 1;
            fprintf(outFile, "movq  $%ld, \%r15\nL%ld:\n", vectorSize, tempLabel);
        }
        lastWasVec = 1;

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pxor %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pxor %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
        // no floating point eor

        if(openner)
            fprintf(outFile, "subq $%ld, \%r15\njns L%ld\n", elementCount(in), tempLabel);

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        lastWasVec = 0;
        
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "xor%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "xor%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
        // no floating point eor
    }
}

void andNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        long openner = 0;
        long tempLabel;
        if(lastWasVec == 0){
            tempLabel = labelCounter++;
            openner = 1;
            fprintf(outFile, "movq  $%ld, \%r15\nL%ld:\n", vectorSize, tempLabel);
        }
        lastWasVec = 1;

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pand %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pand %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
        // no floating point and

        if(openner)
            fprintf(outFile, "subq $%ld, \%r15\njns L%ld\n", elementCount(in), tempLabel);

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        lastWasVec = 0;
        
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "and%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "and%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
        // no floating point and
    }
}

void notNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        long openner = 0;
        long tempLabel;
        if(lastWasVec == 0){
            tempLabel = labelCounter++;
            openner = 1;
            fprintf(outFile, "movq  $%ld, \%r15\nL%ld:\n", vectorSize, tempLabel);
        }
        lastWasVec = 1;

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            fprintf(outFile, "pcmpeq%c %s, %s\n", getType(in), getReg(in, in), getReg(in, in));
            
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "pxor %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "pxor %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }
        }

        // no floating point not

        if(openner)
            fprintf(outFile, "subq $%ld, \%r15\njns L%ld\n", elementCount(in), tempLabel);

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        lastWasVec = 0;
        
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }
            
            fprintf(outFile, "not%c %s\n", getType(in), getReg(in, in));
            
        }

        // no floating point not

    }
}

void negNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        long openner = 0;
        long tempLabel;
        if(lastWasVec == 0){
            tempLabel = labelCounter++;
            openner = 1;
            fprintf(outFile, "movq  $%ld, \%r15\nL%ld:\n", vectorSize, tempLabel);
        }
        lastWasVec = 1;

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            fprintf(outFile, "pxor%c %s, %s\n", getType(in), getReg(in, in), getReg(in, in));
            
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "psub%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "psub%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }
        }else{ // ------------------ FLOATING --------------------------------------------------------------
            fprintf(outFile, "subp%c %s, %s\n", getType(in), getReg(in, in), getReg(in, in));
            
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "subp%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "subp%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }
        }

        if(openner)
            fprintf(outFile, "subq $%ld, \%r15\njns L%ld\n", elementCount(in), tempLabel);

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        lastWasVec = 0;
        
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }
            
            fprintf(outFile, "neg%c %s\n", getType(in), getReg(in, in));
            
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            fprintf(outFile, "subs%c %s, %s\n", getType(in), getReg(in, in), getReg(in, in));
            
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "subs%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "subs%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }
        }
    }
}

void lshNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        if(elementCount(in) == 16){
            fprintf(stderr, "ERR: Sorry, but a packed byte shift has not been implemented yet");
            exit(1);
        }

        long openner = 0;
        long tempLabel;
        if(lastWasVec == 0){
            tempLabel = labelCounter++;
            openner = 1;
            fprintf(outFile, "movq  $%ld, \%r15\nL%ld:\n", vectorSize, tempLabel);
        }
        lastWasVec = 1;

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "pmov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "pmov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "psll%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "psll%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
        //no floating point right shift

        if(openner)
            fprintf(outFile, "subq $%ld, \%r15\njns L%ld\n", elementCount(in), tempLabel);

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        lastWasVec = 0;
        
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "sll%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "sll%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

        //no floating point right shift

    }
}

void rshNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        if(elementCount(in) == 16){
            fprintf(stderr, "ERR: Sorry, but a packed byte shift has not been implemented yet");
            exit(1);
        }

        long openner = 0;
        long tempLabel;
        if(lastWasVec == 0){
            tempLabel = labelCounter++;
            openner = 1;
            fprintf(outFile, "movq  $%ld, \%r15\nL%ld:\n", vectorSize, tempLabel);
        }
        lastWasVec = 1;

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "pmov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "pmov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "psra%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "psra%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
        //no floating point right shift

        if(openner)
            fprintf(outFile, "subq $%ld, \%r15\njns L%ld\n", elementCount(in), tempLabel);

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        lastWasVec = 0;
        
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "sra%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "sra%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

        //no floating point right shift

    }
}

void subNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        long openner = 0;
        long tempLabel;
        if(lastWasVec == 0){
            tempLabel = labelCounter++;
            openner = 1;
            fprintf(outFile, "movq  $%ld, \%r15\nL%ld:\n", vectorSize, tempLabel);
        }
        lastWasVec = 1;

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "pmov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "pmov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "psub%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "psub%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movp%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movp%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "subp%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "subp%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

        if(openner)
            fprintf(outFile, "subq $%ld, \%r15\njns L%ld\n", elementCount(in), tempLabel);

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        lastWasVec = 0;
        
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "sub%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "sub%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "subs%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "subs%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
    }
}

void addNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        long openner = 0;
        long tempLabel;
        if(lastWasVec == 0){
            tempLabel = labelCounter++;
            openner = 1;
            fprintf(outFile, "movq  $%ld, \%r15\nL%ld:\n", vectorSize, tempLabel);
        }
        lastWasVec = 1;

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "pmov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "pmov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "padd%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "padd%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movp%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movp%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "addp%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "addp%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

        if(openner)
            fprintf(outFile, "subq $%ld, \%r15\njns L%ld\n", elementCount(in), tempLabel);

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        lastWasVec = 0;
        
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "add%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "add%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "adds%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "adds%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
    }
}

void modNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------

        long openner = 0;
        long tempLabel;
        if(lastWasVec == 0){
            tempLabel = labelCounter++;
            openner = 1;
            fprintf(outFile, "movq  $%ld, \%r15\nL%ld:\n", vectorSize, tempLabel);
        }
        lastWasVec = 1;

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------

            fprintf(stderr, "ERR: Sorry, but packed integer remainder hasn't been implemented yet\n");
            exit(1);

        }
        // there is no floating point mod

        if(openner)
            fprintf(outFile, "subq $%ld, \%r15\njns L%ld\n", elementCount(in), tempLabel);

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        lastWasVec = 0;
        
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pushq \%rax\npushq    \%rdx\nmovq  %s, \%rax\nxorq \%rdx, \%rdx", getReg(in, in));
                fprintf(outFile, "div%c %s\n", getType(in), getSymbol(in->children[1]));
                fprintf(outFile, "movq  \%rdx, %s\npopq  \%rdx\npopq  \%rax\n", getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pushq \%rax\npushq    \%rdx\nmovq  %s, \%rax\nxorq \%rdx, \%rdx", getReg(in, in));
                fprintf(outFile, "div%c %s\n", getType(in), getReg(in, in->children[1]));
                fprintf(outFile, "movq  \%rdx, %s\npopq  \%rdx\npopq  \%rax\n", getReg(in, in));
            }
        }

        // there is no floating point mod

    }
}

void divNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------

        long openner = 0;
        long tempLabel;
        if(lastWasVec == 0){
            tempLabel = labelCounter++;
            openner = 1;
            fprintf(outFile, "movq  $%ld, \%r15\nL%ld:\n", vectorSize, tempLabel);
        }
        lastWasVec = 1;

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------

            fprintf(stderr, "ERR: Sorry, but packed integer divide hasn't been implemented yet\n");
            exit(1);

        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movp%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movp%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "divp%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "divp%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

        if(openner)
            fprintf(outFile, "subq $%ld, \%r15\njns L%ld\n", elementCount(in), tempLabel);

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        lastWasVec = 0;
        
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pushq \%rax\npushq    \%rdx\nmovq  %s, \%rax\nxorq \%rdx, \%rdx", getReg(in, in));
                fprintf(outFile, "div%c %s\n", getType(in), getSymbol(in->children[1]));
                fprintf(outFile, "movq  \%rax, %s\npopq  \%rdx\npopq  \%rax\n", getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pushq \%rax\npushq    \%rdx\nmovq  %s, \%rax\nxorq \%rdx, \%rdx", getReg(in, in));
                fprintf(outFile, "div%c %s\n", getType(in), getReg(in, in->children[1]));
                fprintf(outFile, "movq  \%rax, %s\npopq  \%rdx\npopq  \%rax\n", getReg(in, in));
            }
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "divs%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "divs%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "divs%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "divs%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
    }
}

void mulNode(struct genericNode* in){

    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        if(elementCount(in) == 16){
            fprintf(stderr, "ERR: Sorry, but a packed byte multiply has not been implemented yet");
            exit(1);
        }

        long openner = 0;
        long tempLabel;
        if(lastWasVec == 0){
            tempLabel = labelCounter++;
            openner = 1;
            fprintf(outFile, "movq  $%ld, \%r15\nL%ld:\n", vectorSize, tempLabel);
        }
        lastWasVec = 1;

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "pmov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "pmov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pmull%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pmull%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movp%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movp%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "mulp%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "mulp%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

        if(openner)
            fprintf(outFile, "subq $%ld, \%r15\njns L%ld\n", elementCount(in), tempLabel);

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        lastWasVec = 0;
        
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "imul%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "imul%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "muls%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "muls%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
    }
}


void runSumNode(struct genericNode* in){}

void permuteNode(struct genericNode* in){}
void vecIndexNode(struct genericNode* in){}

void callNode(struct genericNode* in){}
void callParamNode(struct genericNode* in){}
void returnNode(struct genericNode* in){}
void returnExpNode(struct genericNode* in){}

void generalNode(struct generalNode* in){
    if(in == NULL)
        return;

	switch(in->type){
	case(SYMBOL_TYPE):              symbolNode(in); break;
	case(SCOPE_TYPE):               scopeNode(in); break;
	case(FOR_TYPE):                 forNode(in); break;
	case(GOTO_TYPE):                gotoNode(in); break;
	case(LABEL_TYPE):               labelNode(in); break;
	case(RETURN_TYPE):              returnNode(in); break;
	case(RETURN_EXP_TYPE):          returnExpNode(in); break;
	case(CONTINUE_TYPE):            continueNode(in); break;
	case(BREAK_TYPE):               breakNode(in); break;
	case(WHILE_TYPE):               whileNode(in); break;
	case(IF_TYPE):                  ifNode(in); break;
	case(IF_ELSE_TYPE):             ifElseNode(in); break;
	case(EQU_TYPE):                 equNode(in); break;
	case(TERN_TYPE):                ternNode(in); break;
	case(LOGIC_OR_TYPE):            logicOrNode(in); break;
	case(LOGIC_AND_TYPE):           logicAndNode(in); break;
	case(OR_TYPE):                  orNode(in); break;
	case(EOR_TYPE):                 eorNode(in); break;
	case(AND_TYPE):                 andNode(in); break;
	case(EQU_EQU_TYPE):             equEquNode(in); break;
	case(NOT_EQU_TYPE):             notEquNode(in); break;
	case(LT_TYPE):                  ltNode(in); break;
	case(GT_TYPE):                  gtNode(in); break;
	case(LT_EQU_TYPE):              ltEquNode(in); break;
	case(GT_EQU_TYPE):              gtEquNode(in); break;
	case(LSH_TYPE):                 lshNode(in); break;
	case(RSH_TYPE):                 rshNode(in); break;
	case(SUB_TYPE):                 subNode(in); break;
	case(ADD_TYPE):                 addNode(in); break;
	case(MOD_TYPE):                 modNode(in); break;
	case(DIV_TYPE):                 divNode(in); break;
	case(MUL_TYPE):                 mulNode(in); break;
	case(RUN_SUM_TYPE):             runSumNode(in); break;
	case(DEREF_TYPE):               derefNode(in); break;
	case(NEG_TYPE):                 negNode(in); break;
	case(LOGICAL_NOT_TYPE):         logicalNotNode(in); break;
	case(NOT_TYPE):                 notNode(in); break;
	case(PERMUTE_TYPE):             permuteNode(in); break;
	case(VEC_INDEX_TYPE):           vecIndexNode(in); break;
	case(CALL_TYPE):                callNode(in); break;
	case(CALL_PARAM_TYPE):          callParamNode(in); break;
    default:
        fprintf(stderr, "ERR UNKNOWN NODE TYPE\n");
        exit(1);
	}

}



int backEnd(char* outFile, struct genericNode* DAGinin[]){
	
	DAGin = DAGinin;

	dagConvert(DAGin[0]);

	return 0;
}
