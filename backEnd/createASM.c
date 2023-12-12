#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "frontEndDefs.h"

struct genericNode** DAGin;

FILE* outFile;

//reserve r15 as a helper register
//reserve xmm0 as a helper register

//keeps a list of registers for use, what address a currently using them
char* getReg(struct genericNode* me, struct genericNode* mine){
    return "%reg";
}

char* getByteReg(struct genericNode* me, struct genericNode* mine){
    return "%reg";
}

void resetRegs(){

}

char* getRaxSize(struct genericNode* me){
    return "%rax";
}

//turns local variables into offset(%rbp), or if it's a global variable name(%rip)
char* getSymbol(struct genericNode* in){
    if(strcmp(((struct symbolEntry*)(in->children[0]))->name, "0imm") == 0)
    
        return ((struct symbolEntry*)(in->children[0]))->constValue;
    else
        return ((struct symbolEntry*)(in->children[0]))->name;
}

//pull the constant number from in
char* getInnerNumber(struct genericNode* in){
    return ((struct symbolEntry*)(in->children[0]))->constValue;
}

long lastBaseTypeWas;
//used for typing the data literals
void lastType(struct genericNode* in){
    for(int i = 31; i >= 0; i--){
		if( in->modString[i] == POINTER_POSTFIX) {lastBaseTypeWas = POINTER_POSTFIX; return;}
		if( in->modString[i] == BYTE_BASE)       {lastBaseTypeWas = BYTE_BASE; return;}
		if( in->modString[i] == WORD_BASE)       {lastBaseTypeWas = WORD_BASE; return;}
        if( in->modString[i] == LONG_BASE)       {lastBaseTypeWas = LONG_BASE; return;}
        if( in->modString[i] == QUAD_BASE)       {lastBaseTypeWas = QUAD_BASE; return;}
        if( in->modString[i] == SINGLE_BASE)     {lastBaseTypeWas = LONG_BASE; return;}
        if( in->modString[i] == DOUBLE_BASE)     {lastBaseTypeWas = QUAD_BASE; return;}
    }
}

//how many elements are in a xmm register of in's type
int elementCount(struct genericNode* in){
    for(int i = 31; i >= 0; i--){
		if( in->modString[i] == POINTER_POSTFIX) return 2;
		if( in->modString[i] == BYTE_BASE) return 16;
		if( in->modString[i] == WORD_BASE) return 8;
        if( in->modString[i] == LONG_BASE) return 4;
        if( in->modString[i] == QUAD_BASE) return 2;
        if( in->modString[i] == SINGLE_BASE) return 4;
        if( in->modString[i] == DOUBLE_BASE) return 2;
    }
    
    return 0;
}

//get the type prefix/trailing type
char getType(struct genericNode* in){
    for(int i = 31; i >= 0; i--){
		if( in->modString[i] == POINTER_POSTFIX) return 'q';
		if( in->modString[i] == BYTE_BASE) return 'b';
		if( in->modString[i] == WORD_BASE) return 'w';
        if( in->modString[i] == LONG_BASE) return 'l';
        if( in->modString[i] == QUAD_BASE) return 'q';
        if( in->modString[i] == SINGLE_BASE) return 's';
        if( in->modString[i] == DOUBLE_BASE) return 'd';
    }
    
    return 0;
}

//get the name of the type's size
char* getTypeSizeName(struct genericNode* in){
    for(int i = 31; i >= 0; i--){
		if( in->modString[i] == POINTER_POSTFIX) return "quad";
		if( in->modString[i] == BYTE_BASE) return "byte";
		if( in->modString[i] == WORD_BASE) return "word";
        if( in->modString[i] == LONG_BASE) return "long";
        if( in->modString[i] == QUAD_BASE) return "quad";
        if( in->modString[i] == SINGLE_BASE) return "long";
        if( in->modString[i] == DOUBLE_BASE) return "quad";
    }
    
    return 0;
}

//get the type prefix/trailing type, alias floats to matching int size
char getTypeInt(struct genericNode* in){
    for(int i = 31; i >= 0; i--){
		if( in->modString[i] == POINTER_POSTFIX) return 'q';
		if( in->modString[i] == BYTE_BASE) return 'b';
		if( in->modString[i] == WORD_BASE) return 'w';
        if( in->modString[i] == LONG_BASE) return 'l';
        if( in->modString[i] == QUAD_BASE) return 'q';
        if( in->modString[i] == SINGLE_BASE) return 'l';
        if( in->modString[i] == DOUBLE_BASE) return 'q';
    }
    
    return 0;
}

//is the trailing type a int (this counts pointers)
long isInt(struct genericNode* in){
	for(int i = 31; i >= 0; i--)
		if( in->modString[i] == POINTER_POSTFIX || 
            in->modString[i] == BYTE_BASE || 
            in->modString[i] == WORD_BASE || 
            in->modString[i] == LONG_BASE || 
            in->modString[i] == QUAD_BASE ){
			return 1;
	    }
    
    return 0;
}

int enumType(struct genericNode* in){
    for(int i = 31; i >= 0; i--){
		if( in->modString[i] == POINTER_POSTFIX) return 3;
        if( in->modString[i] == VECTOR_MOD) return 4;
		if( in->modString[i] == BYTE_BASE) return 0;
		if( in->modString[i] == WORD_BASE) return 1;
        if( in->modString[i] == LONG_BASE) return 2;
        if( in->modString[i] == QUAD_BASE) return 3;
        if( in->modString[i] == SINGLE_BASE) return 4;
        if( in->modString[i] == DOUBLE_BASE) return 4;
    }
    
    return 3;
}

//is the trailing type a pointer
long isPointer(struct genericNode* in){
	for(int i = 31; i >= 0; i--)
		if(in->modString[i] == POINTER_POSTFIX){
			return 1;
	    }
    
    return 0;
}

//is the trailing type a float
long isFloat(struct genericNode* in){

	for(int i = 31; i >= 0; i--)
		if(in->modString[i] == SINGLE_BASE || in->modString[i] == DOUBLE_BASE){
			return 1;
	    }
    
    return 0;
}

//is the trailing type vector, returns size
long isVec(struct genericNode* in){

	for(int i = 31; i >= 0; i--)
		if(in->modString[i] == VECTOR_MOD){
			return -(in->modString[i + 1]); //flip the sign, as the length is stored negative
	    }
    
    return 0;
}

long isVoid(struct genericNode* in){

	for(int i = 31; i >= 0; i--)
		if(in->modString[i] == VOID_BASE){
			return 1;
	    }
    
    return 0;
}

//make pblendw mask to match the length of the vector
long createSizeMask(struct genericNode* in){
    return 4;
}




long labelCounter;
long stackOffsetCounter;
long scopeCounter;
long lastWasVec;

long loopLabelStackTop[512];
long loopLabelStackExit[512];
long loopLabelPointerTop = 0;
long loopLabelPointerExit = 0;

void generalNode(struct genericNode* in);

void symbolNode(struct genericNode* in){ //if a symbol isn't handled, it's a outer scope definition

    long inType = NONE_MOD;
	for(int i = 31; i >= 0; i--)
		if(in->children[0]->modString[i] != NONE_MOD){
			inType = in->children[0]->modString[i];
			break;
	    }
    
    if(inType == CLOSE_FUNCTION_POSTFIX){
        if(((struct symbolEntry*)(in->children[0]))->innerScope != NULL){ //full bodied function
            fprintf(outFile, "\n.text\n");
            fprintf(outFile, ".globl\t%s\n.p2align 4\n%s:\n", ((struct symbolEntry*)(in->children[0]))->name, ((struct symbolEntry*)(in->children[0]))->name);
            fprintf(outFile, "pushq\t%%r15\npushq\t%%r14\npushq\t%%r13\npushq\t%%r12\npushq\t%%rbx\npushq\t%%rbp\nmovq\t%%rsp, %%rbp\n");
            resetRegs();
            generalNode(((struct symbolEntry*)(in->children[0]))->innerScope);
        }else
            return; //don't write function prototypes
    }else{   //print base variable
        fprintf(outFile, "\n.data\n");
        fprintf(outFile, "%s:\t.%s\n", ((struct symbolEntry*)(in->children[0]))->name, getTypeSizeName(in));
    }

}

void labelNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling Label Node\n");
    fprintf(outFile, "%s:\n", ((struct symbolEntry*)(in->children[0]))->name);
    generalNode(in->children[1]);
}

void gotoNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling Goto Node\n");
    fprintf(outFile, "jmp\t%s\n", ((struct symbolEntry*)(in->children[0]->children[0]))->name);
}

void scopeNode(struct genericNode* in){
    scopeCounter++;
    fprintf(outFile, "#\tHandling Scope Node\n");
    for(int i = 0; i < in->childCount; i++)
        generalNode(in->children[i]);

    scopeCounter--;
}

void forNode(struct genericNode* in){

    fprintf(outFile, "#\tHandling For Node\n");

    generalNode(in->children[0]);   //intro

    long tempLabel = labelCounter++;
    long exitLabel = labelCounter++;

    loopLabelStackTop[++loopLabelPointerTop] = tempLabel;
    loopLabelStackExit[++loopLabelPointerExit] = exitLabel;

    fprintf(outFile, "L%ld:\n", tempLabel);

    generalNode(in->children[1]);   //condition

    fprintf(outFile, "movq\t%s, %%r15\n", getReg(in, in->children[1]));  //movq handles both floats and clearing short ints
    fprintf(outFile, "cmpq\t$0, %%r15\n");
    fprintf(outFile, "jz\tL%ld\n", exitLabel);

    generalNode(in->children[3]);   //body of the loop

    generalNode(in->children[2]);   //incrementer

    fprintf(outFile, "jmp\tL%ld\n", tempLabel);
    fprintf(outFile, "L%ld:\n", exitLabel);

    loopLabelPointerTop--;
    loopLabelPointerExit--;

}

void whileNode(struct genericNode* in){

    fprintf(outFile, "#\tHandling While Node\n");

    long tempLabel = labelCounter++;
    long exitLabel = labelCounter++;

    loopLabelStackTop[++loopLabelPointerTop] = tempLabel;
    loopLabelStackExit[++loopLabelPointerExit] = exitLabel;

    fprintf(outFile, "L%ld:\n", tempLabel);

    generalNode(in->children[0]);   //condition

    fprintf(outFile, "movq\t%s, %%r15\n", getReg(in, in->children[0]));  //movq handles both floats and clearing short ints
    fprintf(outFile, "cmpq\t$0, %%r15\n");
    fprintf(outFile, "jz\tL%ld\n", exitLabel);

    generalNode(in->children[1]);   //body

    fprintf(outFile, "jmp\tL%ld\n", tempLabel);
    fprintf(outFile, "L%ld:\n", exitLabel);

    loopLabelPointerTop--;
    loopLabelPointerExit--;

}

void continueNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling Continue Node\n");
    fprintf(outFile, "jmp\tL%ld\n", loopLabelStackTop[loopLabelPointerTop]);
}

void breakNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling Break Node\n");
    fprintf(outFile, "jmp\tL%ld\n", loopLabelStackExit[loopLabelPointerExit]);
}

void ifNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling If Node\n");

    generalNode(in->children[0]);   //condition

    long tempLabel = labelCounter++;

    fprintf(outFile, "movq\t%s, %%r15\n", getReg(in, in->children[0]));  //movq handles both floats and clearing short ints
    fprintf(outFile, "cmpq\t$0, %%r15\n");
    fprintf(outFile, "jz\tL%ld\n", tempLabel);

    generalNode(in->children[1]);   //body

    fprintf(outFile, "L%ld:\n", tempLabel);
}

void ifElseNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling IfElse Node\n");

    generalNode(in->children[0]);   //condition

    long tempLabel = labelCounter++;
    long tempLabelElse = labelCounter++;

    fprintf(outFile, "movq\t%s, %%r15\n", getReg(in, in->children[0]));  //movq handles both floats and clearing short ints
    fprintf(outFile, "cmpq\t$0, %%r15\n");
    fprintf(outFile, "jz\tL%ld\n", tempLabel);

    generalNode(in->children[1]);   //if body

    fprintf(outFile, "jmp\tL%ld\nL%ld:\n", tempLabelElse, tempLabel);

    generalNode(in->children[2]);   //else body

    fprintf(outFile, "L%ld:\n", tempLabelElse);

}

void derefNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling Deref Node\n");

    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movq\t%s, %%r15\n", getSymbol(in->children[0]));  //move in symbol data
            fprintf(outFile, "movdqu\t(%%r15), %s\n", getReg(in, in));          //deref the symbol
        }else{
            generalNode(in->children[0]);
            fprintf(outFile, "movdqu\t(%s), %s\n", getReg(in, in->children[0]), getReg(in, in));    //deref the register
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movq\t%s, %%r15\n", getSymbol(in->children[0]));
                fprintf(outFile, "mov%c\t(%%r15), %s\n", getType(in), getReg(in, in));
            }else{
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c\t(%s), %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movq\t%s, %%r15\n", getSymbol(in->children[0]));
                fprintf(outFile, "movs%c\t(%%r15), %s\n", getType(in), getReg(in, in));
            }else{
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c\t(%s), %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

        }
    }
}

void equNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling Equ Node\n");

    if(scopeCounter < 1){  //handle scope zero set definitions
        fprintf(outFile, "\n.data\n");
        fprintf(outFile, "%s: .%s %s\n", 
        ((struct symbolEntry*)(in->children[0]->children[0]))->name,
        getTypeSizeName(in), 
        ((struct symbolEntry*)(in->children[1]->children[0]))->constValue);
        return;
    }

    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------

        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu\t%s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqu\t%s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(in->children[1]->type == SYMBOL_TYPE){
            fprintf(outFile, "pblendw\t%ld, %s, %s\n", createSizeMask(in), getSymbol(in->children[1]), getReg(in, in));
        }else{
            generalNode(in->children[1]);
            fprintf(outFile, "pblendw\t%ld, %s, %s\n", createSizeMask(in), getReg(in, in->children[1]), getReg(in, in));
        }

        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu\t%s, %s\n", getReg(in, in), getSymbol(in->children[0]));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqu\t%s, %s\n",  getReg(in, in), getReg(in, in->children[0]));
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }

            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getReg(in, in), getSymbol(in->children[0]));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getReg(in, in), getReg(in, in->children[0]));
            }

        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }

            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getReg(in, in), getSymbol(in->children[0]));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getReg(in, in), getReg(in, in->children[0]));
            }
        }
    }
}

void ternNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling Ternary Node\n");

    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------

        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu\t%s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa\t%s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        fprintf(outFile, "pxor\t%%xmm0, %%xmm0\n"); //zero xmm0
        
        if(isInt(in)){
            fprintf(outFile, "pcmpeq%c\t%%xmm0, %s\n", getType(in), getReg(in, in));
            fprintf(outFile, "pcmpeqb\t%%xmm0, %%xmm0\n");                                 //negative 1 xmm0
            fprintf(outFile, "pxor\t%%xmm0, %s\n", getReg(in, in));                     //integer != 0
        }else{
            fprintf(outFile, "cmpneqs%c\t%%xmm0, %s\n", getType(in), getReg(in, in));    //float != 0
        }

        //the four situations
        if(in->children[1]->type == SYMBOL_TYPE && in->children[2]->type == SYMBOL_TYPE){
            fprintf(outFile, "pcmpeqb\t%%xmm0, %%xmm0\n");
            fprintf(outFile, "pxor\t%s, %%xmm0\n", getReg(in, in));

            fprintf(outFile, "pand\t%s, %s\n", getSymbol(in->children[1]), getReg(in, in));
            fprintf(outFile, "pand\t%s, %%xmm0\n", getSymbol(in->children[2]));
            fprintf(outFile, "por\t%%xmm0, %s\n", getReg(in, in));
        }else
        if(in->children[1]->type == SYMBOL_TYPE && in->children[2]->type != SYMBOL_TYPE){
            generalNode(in->children[2]);
            fprintf(outFile, "pcmpeqb\t%%xmm0, %%xmm0\n");
            fprintf(outFile, "pxor\t%s, %%xmm0\n", getReg(in, in));

            fprintf(outFile, "pand\t%s, %s\n", getSymbol(in->children[1]), getReg(in, in));
            fprintf(outFile, "pand\t%s, %%xmm0\n", getReg(in, in->children[2]));
            fprintf(outFile, "por\t%%xmm0, %s\n", getReg(in, in));
        }else
        if(in->children[1]->type != SYMBOL_TYPE && in->children[2]->type == SYMBOL_TYPE){
            generalNode(in->children[1]);
            fprintf(outFile, "pcmpeqb\t%%xmm0, %%xmm0\n");
            fprintf(outFile, "pxor\t%s, %%xmm0\n", getReg(in, in));

            fprintf(outFile, "pand\t%s, %s\n", getReg(in, in->children[1]), getReg(in, in));
            fprintf(outFile, "pand\t%s, %%xmm0\n", getSymbol(in->children[2]));
            fprintf(outFile, "por\t%%xmm0, %s\n", getReg(in, in));
        }else
        if(in->children[1]->type != SYMBOL_TYPE && in->children[2]->type != SYMBOL_TYPE){
            generalNode(in->children[1]);
            generalNode(in->children[2]);
            fprintf(outFile, "pcmpeqb\t%%xmm0, %%xmm0\n");
            fprintf(outFile, "pxor\t%s, %%xmm0\n", getReg(in, in));

            fprintf(outFile, "pand\t%s, %s\n", getReg(in, in->children[1]), getReg(in, in));
            fprintf(outFile, "pand\t%s, %%xmm0\n", getReg(in, in->children[2]));
            fprintf(outFile, "por\t%%xmm0, %s\n", getReg(in, in));
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        lastWasVec = 0;
        
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE && in->children[2]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmp%c\t$0, %s\n",  getType(in), getReg(in, in));

                fprintf(outFile, "cmovne%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
                fprintf(outFile, "cmoveq%c\t%s, %s\n", getType(in), getSymbol(in->children[2]), getReg(in, in));
            }else
            if(in->children[1]->type == SYMBOL_TYPE && in->children[2]->type != SYMBOL_TYPE){
                generalNode(in->children[2]);
                fprintf(outFile, "cmp%c\t$0, %s\n",  getType(in), getReg(in, in));

                fprintf(outFile, "cmovne%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
                fprintf(outFile, "cmoveq%c\t%s, %s\n", getType(in), getReg(in, in->children[2]), getReg(in, in));
            }else
            if(in->children[1]->type != SYMBOL_TYPE && in->children[2]->type == SYMBOL_TYPE){
                generalNode(in->children[1]);
                fprintf(outFile, "cmp%c\t$0, %s\n",  getType(in), getReg(in, in));

                fprintf(outFile, "cmovne%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "cmoveq%c\t%s, %s\n", getType(in), getSymbol(in->children[2]), getReg(in, in));
            }else
            if(in->children[1]->type != SYMBOL_TYPE && in->children[2]->type != SYMBOL_TYPE){
                generalNode(in->children[1]);
                generalNode(in->children[2]);
                fprintf(outFile, "cmp%c\t$0, %s\n",  getType(in), getReg(in, in));

                fprintf(outFile, "cmovne%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "cmoveq%c\t%s, %s\n", getType(in), getReg(in, in->children[2]), getReg(in, in));
            }

        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            fprintf(outFile, "pxor\t%%xmm0, %%xmm0\n");
            fprintf(outFile, "cmpneqs%c\t%%xmm0, %s\n", getType(in), getReg(in, in));

            if(in->children[1]->type == SYMBOL_TYPE && in->children[2]->type == SYMBOL_TYPE){
                fprintf(outFile, "pcmpeqb\t%%xmm0, %%xmm0\n");
                fprintf(outFile, "pxor\t%s, %%xmm0\n", getReg(in, in)); //xmm0 is the logical inverse of reg

                fprintf(outFile, "pand\t%s, %s\n", getSymbol(in->children[1]), getReg(in, in));
                fprintf(outFile, "pand\t%s, %%xmm0\n", getSymbol(in->children[2]));
                fprintf(outFile, "por\t%%xmm0, %s\n", getReg(in, in));
            }else
            if(in->children[1]->type == SYMBOL_TYPE && in->children[2]->type != SYMBOL_TYPE){
                generalNode(in->children[2]);
                fprintf(outFile, "pcmpeqb\t%%xmm0, %%xmm0\n");
                fprintf(outFile, "pxor\t%s, %%xmm0\n", getReg(in, in));

                fprintf(outFile, "pand\t%s, %s\n", getSymbol(in->children[1]), getReg(in, in));
                fprintf(outFile, "pand\t%s, %%xmm0\n", getReg(in, in->children[2]));
                fprintf(outFile, "por\t%%xmm0, %s\n", getReg(in, in));
            }else
            if(in->children[1]->type != SYMBOL_TYPE && in->children[2]->type == SYMBOL_TYPE){
                generalNode(in->children[1]);
                fprintf(outFile, "pcmpeqb\t%%xmm0, %%xmm0\n");
                fprintf(outFile, "pxor\t%s, %%xmm0\n", getReg(in, in));

                fprintf(outFile, "pand\t%s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pand\t%s, %%xmm0\n", getSymbol(in->children[2]));
                fprintf(outFile, "por\t%%xmm0, %s\n", getReg(in, in));
            }else
            if(in->children[1]->type != SYMBOL_TYPE && in->children[2]->type != SYMBOL_TYPE){
                generalNode(in->children[1]);
                generalNode(in->children[2]);
                fprintf(outFile, "pcmpeqb\t%%xmm0, %%xmm0\n");
                fprintf(outFile, "pxor\t%s, %%xmm0\n", getReg(in, in));

                fprintf(outFile, "pand\t%s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pand\t%s, %%xmm0\n", getReg(in, in->children[2]));
                fprintf(outFile, "por\t%%xmm0, %s\n", getReg(in, in));
            }
        }
    }
}

void logicOrNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling LogicalOr Node\n");

    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu\t%s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa\t%s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "por\t%s, %s\n", getSymbol(in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor\t%%xmm0, %%xmm0\n");
                fprintf(outFile, "pcmpeq%c\t%%xmm0, %s\n", getType(in), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "por\t%s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor\t%%xmm0, %%xmm0\n");
                fprintf(outFile, "pcmpeq%c\t%%xmm0, %s\n", getType(in), getReg(in, in));
            }

            fprintf(outFile, "pcmpeqb\t%%xmm0, %%xmm0\n");
            fprintf(outFile, "pxor\t%%xmm0, %s\n", getReg(in, in));

        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "por\t%s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor\t%%xmm0, %%xmm0\n");
                fprintf(outFile, "cmpneqp%c\t%%xmm0, %s\n", getType(in), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "por\t%s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor\t%%xmm0, %%xmm0\n");
                fprintf(outFile, "cmpneqp%c\t%%xmm0, %s\n", getType(in), getReg(in, in));
            }
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "or%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
                fprintf(outFile, "cmp%c\t$0, %s\nsetne\t%s\n", getType(in), getReg(in, in), getByteReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "or%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "cmp%c\t$0, %s\nsetne\t%s\n", getType(in), getReg(in, in), getByteReg(in, in));
            }
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "por\t%s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor\t%%xmm0, %%xmm0\n");
                fprintf(outFile, "cmpneqs%c\t%%xmm0, %s\n", getType(in), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "por\t%s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor\t%%xmm0, %%xmm0\n");
                fprintf(outFile, "cmpneqs%c\t%%xmm0, %s\n", getType(in), getReg(in, in));
            }
        }
    }
}

void logicAndNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling LogicalAnd Node\n");

    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu\t%s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa\t%s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pand\t%s, %s\n", getSymbol(in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor\t%%xmm0, %%xmm0\n");
                fprintf(outFile, "pcmpeq%c\t%%xmm0, %s\n", getType(in), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pand\t%s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor\t%%xmm0, %%xmm0\n");
                fprintf(outFile, "pcmpeq%c\t%%xmm0, %s\n", getType(in), getReg(in, in));
            }

            fprintf(outFile, "pcmpeqb\t%%xmm0, %%xmm0\n");
            fprintf(outFile, "pxor\t%%xmm0, %s\n", getReg(in, in));

        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "andps\t%s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor\t%%xmm0, %%xmm0\n");
                fprintf(outFile, "cmpneqp%c\t%%xmm0, %s\n", getType(in), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "andps\t%s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor\t%%xmm0, %%xmm0\n");
                fprintf(outFile, "cmpneqp%c\t%%xmm0, %s\n", getType(in), getReg(in, in));
            }
        }


    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "and%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
                fprintf(outFile, "cmp%c\t$0, %s\nsetne\t%s\n", getType(in), getReg(in, in), getByteReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "and%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "cmp%c\t$0, %s\nsetne\t%s\n", getType(in), getReg(in, in), getByteReg(in, in));
            }
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "andps\t%s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor\t%%xmm0, %%xmm0\n");
                fprintf(outFile, "cmpneqs%c\t%%xmm0, %s\n", getType(in), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "andps\t%s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor\t%%xmm0, %%xmm0\n");
                fprintf(outFile, "cmpneqs%c\t%%xmm0, %s\n", getType(in), getReg(in, in));
            }
        }
    }
}

void logicalNotNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling LogicalNot Node\n");

    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu\t%s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa\t%s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            fprintf(outFile, "pxor\t%%xmm0, %%xmm0\n");
            fprintf(outFile, "pcmpeq%c\t%%xmm0, %s\n", getType(in), getReg(in, in));
            fprintf(outFile, "pcmpeqb\t%%xmm0, %%xmm0\n");
            fprintf(outFile, "pxor\t%%xmm0, %s\n", getReg(in, in));

        }else{ // ------------------ FLOATING --------------------------------------------------------------
            fprintf(outFile, "pxor\t%%xmm0, %%xmm0\n");
            fprintf(outFile, "cmpneqp%c\t%%xmm0, %s\n", getType(in), getReg(in, in));
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        lastWasVec = 0;
        
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            fprintf(outFile, "cmp%c\t$0, %s\nsetne\t%s\n", getType(in), getReg(in, in), getByteReg(in, in));
            
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c\t%s,\t%s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c\t%s,\t%s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            fprintf(outFile, "cmpgtss\t%%xmm0, %%xmm0\n");
            fprintf(outFile, "cmpneqs%c\t%%xmm0, %s\n", getType(in), getReg(in, in));
        }
    }
}

void notEquNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling NotEqu Node\n");

    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu\t%s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa\t%s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }
        
        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pcmpeq%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pcmpeq%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }

            fprintf(outFile, "pcmpeqb\t%%xmm0, %%xmm0\n");
            fprintf(outFile, "pxor\t%%xmm0, %s\n", getReg(in, in));

        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmpneqp%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmpneqp%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmp%c\t%s, %s\nsetne\t%s\n", getType(in), getSymbol(in->children[1]), getReg(in, in), getByteReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmp%c\t%s, %s\nsetne\t%s\n", getType(in), getReg(in, in->children[1]), getReg(in, in), getByteReg(in, in));
            }
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmpneqs%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmpneqs%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
    }
}

void gtEquNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling GtEqu Node\n");

    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu\t%s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa\t%s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------            
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqa\t%s, %%xmm0\n", getReg(in, in));
                fprintf(outFile, "pcmpgt%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
                fprintf(outFile, "pcmpeq%c\t%s, %%xmm0\n", getType(in), getSymbol(in->children[1]));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "movdqa\t%s, %%xmm0\n", getReg(in, in));
                fprintf(outFile, "pcmpgt%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pcmpeq%c\t%s, %%xmm0\n", getType(in), getReg(in, in->children[1]));
            }

            fprintf(outFile, "por\t%%xmm0, %s\n", getReg(in, in));

        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmpnltp%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmpnltp%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmp%c\t%s, %s\nsetge\t%s\n", getType(in), getSymbol(in->children[1]), getReg(in, in), getByteReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmp%c\t%s, %s\nsetge\t%s\n", getType(in), getReg(in, in->children[1]), getReg(in, in), getByteReg(in, in));
            }
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmpnlts%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmpnlts%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
    }
}

void ltNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling Lt Node\n");

    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu\t%s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa\t%s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqa\t%s, %%xmm0\n", getReg(in, in));
                fprintf(outFile, "pcmpgt%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
                fprintf(outFile, "pcmpeq%c\t%s, %%xmm0\n", getType(in), getSymbol(in->children[1]));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "movdqa\t%s, %%xmm0\n", getReg(in, in));
                fprintf(outFile, "pcmpgt%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pcmpeq%c\t%s, %%xmm0\n", getType(in), getReg(in, in->children[1]));
            }

            fprintf(outFile, "por\t%%xmm0, %s\n", getReg(in, in));
            fprintf(outFile, "pcmpgeb\t%%xmm0, %%xmm0\n");
            fprintf(outFile, "pxor\t%%xmm0, %s\n", getReg(in, in));

        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmpltp%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmpltp%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmp%c\t%s, %s\nsetlt\t%s\n", getType(in), getSymbol(in->children[1]), getReg(in, in), getByteReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmp%c\t%s, %s\nsetlt\t%s\n", getType(in), getReg(in, in->children[1]), getReg(in, in), getByteReg(in, in));
            }
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmplts%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmplts%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
    }
}

void ltEquNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling LtEqu Node\n");

    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu\t%s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa\t%s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pcmpgt%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pcmpgt%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }

            fprintf(outFile, "pcmpeqb\t%%xmm0, %%xmm0\n");
            fprintf(outFile, "pxor\t%%xmm0, %s\n", getReg(in, in));

        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmplep%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmplep%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmp%c\t%s, %s\nsetle\t%s\n", getType(in), getSymbol(in->children[1]), getReg(in, in), getByteReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmp%c\t%s, %s\nsetle\t%s\n", getType(in), getReg(in, in->children[1]), getReg(in, in), getByteReg(in, in));
            }
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmples%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmples%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
    }
}

void equEquNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling EquEqu Node\n");

    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu\t%s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa\t%s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pcmpeq%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pcmpeq%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        
        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmpeqp%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmpeqp%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmp%c\t%s, %s\nseteq\t%s\n", getType(in), getSymbol(in->children[1]), getReg(in, in), getByteReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmp%c\t%s, %s\nseteq\t%s\n", getType(in), getReg(in, in->children[1]), getReg(in, in), getByteReg(in, in));
            }
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmpeqs%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmpeqs%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
    }
}

void gtNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling GtEqu Node\n");
    
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu\t%s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa\t%s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pcmpgt%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pcmpgt%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }

        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmpnlep%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmpnlep%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            long tempLabel = labelCounter++;

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmp%c\t%s, %s\nsetgt   %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in), getByteReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmp%c\t%s, %s\nsetgt   %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in), getByteReg(in, in));
            }
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmpnles%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmpnles%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
    }
}

void orNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling Or Node\n");

    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu\t%s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa\t%s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "por\t%s, %s\n", getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "por\t%s, %s\n", getReg(in, in->children[1]), getReg(in, in));
            }
        }
        // no floating point and

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "or%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "or%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
        // no floating point or
    }
}

void eorNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling Eor Node\n");

    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu\t%s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa\t%s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pxor\t%s, %s\n", getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pxor\t%s, %s\n", getReg(in, in->children[1]), getReg(in, in));
            }
        }
        // no floating point eor

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "xor%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "xor%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
        // no floating point eor
    }
}

void andNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling And Node\n");

    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu\t%s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa\t%s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pand\t%s, %s\n", getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pand\t%s, %s\n", getReg(in, in->children[1]), getReg(in, in));
            }
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "and%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "and%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
        // no floating point and
    }
}

void notNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling Not Node\n");

    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            fprintf(outFile, "pcmpeq%c\t%s, %s\n", getType(in), getReg(in, in), getReg(in, in));
            
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "pxor\t%s, %s\n", getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "pxor\t%s, %s\n", getReg(in, in->children[0]), getReg(in, in));
            }
        }

        // no floating point not

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }
            
            fprintf(outFile, "not%c\t%s\n", getType(in), getReg(in, in));
            
        }

        // no floating point not

    }
}

void negNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling Negate Node\n");

    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            fprintf(outFile, "pxor%c\t%s, %s\n", getType(in), getReg(in, in), getReg(in, in));
            
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "psub%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "psub%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }
        }else{ // ------------------ FLOATING --------------------------------------------------------------
            fprintf(outFile, "subp%c\t%s, %s\n", getType(in), getReg(in, in), getReg(in, in));
            
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "subp%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "subp%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }
            
            fprintf(outFile, "neg%c\t%s\n", getType(in), getReg(in, in));
            
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            fprintf(outFile, "subs%c\t%s, %s\n", getType(in), getReg(in, in), getReg(in, in));
            
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "subs%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "subs%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }
        }
    }
}

void lshNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling Lsh Node\n");

    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        if(elementCount(in) == 16){
            fprintf(stderr, "ERR: Sorry, but a packed byte shift has not been implemented yet");
            exit(1);
        }

        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu\t%s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa\t%s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "psll%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "psll%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
        //no floating point right shift

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "sll%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "sll%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

        //no floating point right shift

    }
}

void rshNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling Rsh Node\n");

    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        if(elementCount(in) == 16){
            fprintf(stderr, "ERR: Sorry, but a packed byte shift has not been implemented yet");
            exit(1);
        }

        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu\t%s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa\t%s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "psra%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "psra%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
        //no floating point right shift

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "sra%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "sra%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

        //no floating point right shift

    }
}

void subNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling Sub Node\n");

    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu\t%s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa\t%s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "psub%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "psub%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "subp%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "subp%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "sub%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "sub%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "subs%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "subs%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
    }
}

void addNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling Add Node\n");

    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu\t%s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa\t%s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }
        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "padd%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "padd%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "addp%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "addp%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "add%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "add%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "adds%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "adds%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
    }
}

void modNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling Rem Node\n");

    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------

            fprintf(stderr, "ERR: Sorry, but packed integer remainder hasn't been implemented yet\n");
            exit(1);

        }
        // there is no floating point mod

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pushq\t%%rax\npushq\t%%rdx\nmovq\t%s, %%rax\nxorq\t%%rdx, %%rdx\n", getReg(in, in));
                fprintf(outFile, "div%c\t%s\n", getType(in), getSymbol(in->children[1]));
                fprintf(outFile, "movq\t%%rdx, %s\npopq\t%%rdx\npopq\t%%rax\n", getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pushq\t%%rax\npushq\t%%rdx\nmovq\t%s, %%rax\nxorq\t%%rdx, %%rdx\n", getReg(in, in));
                fprintf(outFile, "div%c\t%s\n", getType(in), getReg(in, in->children[1]));
                fprintf(outFile, "movq\t%%rdx, %s\npopq\t%%rdx\npopq\t%%rax\n", getReg(in, in));
            }
        }

        // there is no floating point mod

    }
}

void divNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling Div Node\n");

    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------

        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu\t%s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa\t%s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------

            fprintf(stderr, "ERR: Sorry, but packed integer divide hasn't been implemented yet\n");
            exit(1);

        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "divp%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "divp%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pushq\t%%rax\npushq\t%%rdx\nmovq\t%s, %%rax\nxorq\t%%rdx, %%rdx\n", getReg(in, in));
                fprintf(outFile, "div%c\t%s\n", getType(in), getSymbol(in->children[1]));
                fprintf(outFile, "movq\t%%rax, %s\npopq\t%%rdx\npopq\t%%rax\n", getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pushq\t%%rax\npushq\t%%rdx\nmovq\t%s, %%rax\nxorq\t%%rdx, %%rdx\n", getReg(in, in));
                fprintf(outFile, "div%c\t%s\n", getType(in), getReg(in, in->children[1]));
                fprintf(outFile, "movq\t%%rax, %s\npopq\t%%rdx\npopq\t%%rax\n", getReg(in, in));
            }
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "divs%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "divs%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "divs%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "divs%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
    }
}

void mulNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling Mul Node\n");

    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        if(elementCount(in) == 16){
            fprintf(stderr, "ERR: Sorry, but a packed byte multiply has not been implemented yet");
            exit(1);
        }

        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu\t%s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa\t%s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pmull%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pmull%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "mulp%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "mulp%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "imul%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "imul%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c\t%s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "muls%c\t%s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "muls%c\t%s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
    }
}

void vecIndexNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling VecIndex Node\n");

    if(in->children[0]->type == SYMBOL_TYPE){
        fprintf(outFile, "pextr%c\t%s, %s, %s\n", getTypeInt(in), getInnerNumber(in->children[1]), getSymbol(in->children[0]), getReg(in, in));
    }else{
        generalNode(in->children[0]);
        fprintf(outFile, "pextr%c %s, %s, %s\n", getTypeInt(in), getInnerNumber(in->children[1]), getReg(in, in->children[0]), getReg(in, in));
    }
}

void preCallPush(){
    fprintf(outFile, "\
pushq\t%%rdi\n\
pushq\t%%rsi\n\
pushq\t%%rdx\n\
pushq\t%%rcx\n\
pushq\t%%r8\n\
pushq\t%%r9\n\
pushq\t%%r10\n\
pushq\t%%r11\n\
subq\t$16, %%rsp\n\
movdqu\t%%xmm1, (%%rsp)\n\
subq\t$16, %%rsp\n\
movdqu\t%%xmm2, (%%rsp)\n\
subq\t$16, %%rsp\n\
movdqu\t%%xmm3, (%%rsp)\n\
subq\t$16, %%rsp\n\
movdqu\t%%xmm4, (%%rsp)\n\
subq\t$16, %%rsp\n\
movdqu\t%%xmm5, (%%rsp)\n\
subq\t$16, %%rsp\n\
movdqu\t%%xmm6, (%%rsp)\n\
subq\t$16, %%rsp\n\
movdqu\t%%xmm7, (%%rsp)\n\
subq\t$16, %%rsp\n\
movdqu\t%%xmm8, (%%rsp)\n\
");
}

void postCallPop(){
    fprintf(outFile, "\
movdqu\t(%%rsp), %%xmm8\n\
addq\t$16, %%rsp\n\
movdqu\t(%%rsp), %%xmm7\n\
addq\t$16, %%rsp\n\
movdqu\t(%%rsp), %%xmm6\n\
addq\t$16, %%rsp\n\
movdqu\t(%%rsp), %%xmm5\n\
addq\t$16, %%rsp\n\
movdqu\t(%%rsp), %%xmm4\n\
addq\t$16, %%rsp\n\
movdqu\t(%%rsp), %%xmm3\n\
addq\t$16, %%rsp\n\
movdqu\t(%%rsp), %%xmm2\n\
addq\t$16, %%rsp\n\
movdqu\t(%%rsp), %%xmm1\n\
addq\t$16, %%rsp\n\
popq\t%%r11\n\
popq\t%%r10\n\
popq\t%%r9\n\
popq\t%%r8\n\
popq\t%%rcx\n\
popq\t%%rdx\n\
popq\t%%rsi\n\
popq\t%%rdi\n\
");
}

void callNode(struct genericNode* in){ 
    fprintf(outFile, "#\tHandling Call Node\n");

    long vectorSize = isVec(in);
    if(!isVoid(in))
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        fprintf(outFile, "pushq\t%%rax\n");     
    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            fprintf(outFile, "subq\t$16, %%rsp\nmovdqu\t%%xmm0, (%%rsp)\n"); 
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            fprintf(outFile, "pushq\t%%rax\n");
        }
    }

    preCallPush();   
    if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "call\t%s\n", getSymbol(in->children[0]));
        }else{
            generalNode(in->children[0]);
            fprintf(outFile, "call\t%s\n", getReg(in, in->children[0]));
    }
    postCallPop();

    if(!isVoid(in))
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        fprintf(outFile, "movdqa\t%%xmm0, %s\n", getReg(in, in));
        fprintf(outFile, "popq\t%%rax\n");
    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getRaxSize(in), getReg(in, in));
            fprintf(outFile, "movdqu\t(%%rsp), %%xmm0\naddq\t$16, %%rsp\n"); 
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            fprintf(outFile, "movdqa\t%%xmm0, %s\n", getReg(in, in));
            fprintf(outFile, "popq\t%%rax\n");
        }
    }
}

void permuteNode(struct genericNode* in){

}

void callParamNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling CallParam Node\n");

    generalNode(in->children[0]);

    for(int i = 0; i < in->children[1]->childCount; i++){
        if(in->children[1]->children[i]->type != SYMBOL_TYPE){
            generalNode(in->children[1]->children[i]);
        }
    }

    if(in->childCount > 6){
        fprintf(stderr, "ERR: currently the max parameters for a function is 6");
        exit(1);
    }
    
    char paramRegisters[5][6][7] = {
        {"%dil", "%sil", "%dl", "%cl", "%r8b", "%r9b"},
        {"%di", "%si", "%dx", "%cx", "%r8w", "%r9w"},
        {"%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"},
        {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"},
        {"%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5"}
    };

    for(int i = 0; i < in->children[1]->childCount; i++){    
        if(enumType(in->children[1]->children[i]) == 5)
            fprintf(outFile, "subq\t$16, %%rsp\nmovdqu\t%s, (%%rsp)\n", 
            paramRegisters[i][enumType(in->children[1]->children[i])]);
        else
            fprintf(outFile, "push%c\t%s\n", getType(in->children[1]->children[i]), 
            paramRegisters[i][enumType(in->children[1]->children[i])]);


        if(in->children[1]->children[i]->type == SYMBOL_TYPE){
            fprintf(outFile, "mov%c\t%s, %s\n", 
            getType(in->children[1]->children[i]), 
            getSymbol(in->children[1]->children[i]), 
            paramRegisters[i][enumType(in->children[1]->children[i])]);
        }else{
            fprintf(outFile, "mov%c\t%s, %s\n", 
            getType(in->children[1]->children[i]), 
            getReg(in, in->children[1]->children[i]), 
            paramRegisters[i][enumType(in->children[1]->children[i])]);
        }    
    }

    long vectorSize = isVec(in);
    if(!isVoid(in))
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        fprintf(outFile, "pushq\t%%rax\n");     
    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            fprintf(outFile, "subq\t$16, %%rsp\nmovdqu\t%%xmm0, (%%rsp)\n"); 
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            fprintf(outFile, "pushq\t%%rax\n");
        }
    }

    preCallPush();   
    if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "call\t%s\n", getSymbol(in->children[0]));
        }else{
            generalNode(in->children[0]);
            fprintf(outFile, "call\t%s\n", getReg(in, in->children[0]));
    }
    postCallPop();

    if(!isVoid(in))
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        fprintf(outFile, "movdqa\t%%xmm0, %s\n", getReg(in, in));
        fprintf(outFile, "popq\t%%rax\n");
    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            fprintf(outFile, "mov%c\t%s, %s\n", getType(in), getRaxSize(in), getReg(in, in));
            fprintf(outFile, "movdqu\t(%%rsp), %%xmm0\naddq\t$16, %%rsp\n"); 
        }else{ // ------------------ FLOATING -------------------------------------------------------------
            fprintf(outFile, "movdqa\t%%xmm0, %s\n", getReg(in, in));
            fprintf(outFile, "popq\t%%rax\n");
        }
    }

    for(int i = 0; i < in->childCount; i++){
        if(enumType(in->children[0]) == 5)
            fprintf(outFile, "movdqu\t(%%rsp), %s\naddq\t$16, %%rsp\n", paramRegisters[i][enumType(in->children[0])]);
        else
            fprintf(outFile, "pop%c\t%s\n", getType(in->children[0]), paramRegisters[i][enumType(in->children[0])]);
    }
}


void returnNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling Return Node\n");
    fprintf(outFile, "movq\t%%rbp, %%rsp\npopq\t%%rbp\npopq\t%%rbx\npopq\t%%r12\npopq\t%%r13\npopq\t%%r14\npopq\t%%r15\n");
}


void returnExpNode(struct genericNode* in){
    fprintf(outFile, "#\tHandling RetExp Node\n");

    if(in->children[0]->type == SYMBOL_TYPE){
        fprintf(outFile, "mov%c\t%s, %s\n", getType(in->children[0]), getSymbol(in->children[0]), getRaxSize(in->children[0]));
    }else{
        generalNode(in->children[0]);
        fprintf(outFile, "mov%c\t%s, %s\n", getType(in->children[0]), getReg(in, in->children[0]), getRaxSize(in->children[0]));
    }
    
    fprintf(outFile, "movq\t%%rbp, %%rsp\npopq\t%%rbp\npopq\t%%rbx\npopq\t%%r12\npopq\t%%r13\npopq\t%%r14\npopq\t%%r15\n");
    
}


void generalNode(struct genericNode* in){

    if(in == NULL)
        return;

    printf("node %ld\n", in->type);

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
	case(DEREF_TYPE):               derefNode(in); break;
	case(NEG_TYPE):                 negNode(in); break;
	case(LOGICAL_NOT_TYPE):         logicalNotNode(in); break;
	case(NOT_TYPE):                 notNode(in); break;
	case(PERMUTE_TYPE):             permuteNode(in); break;
	case(VEC_INDEX_TYPE):           vecIndexNode(in); break;
	case(CALL_TYPE):                callNode(in); break;
	case(CALL_PARAM_TYPE):          callParamNode(in); break;
    default:
        fprintf(stderr, "ERR UNKNOWN NODE TYPE %ld\n", in->type);
        exit(1);
	}

}



int backEnd(char* outFileName, struct genericNode* DAGinin[]){
	
    outFile = fopen(outFileName, "w");

	DAGin = DAGinin;

    printf("Starting work\n");
    scopeCounter = -1;
	generalNode(DAGin[0]);

    fclose(outFile);

	return 0;
}
