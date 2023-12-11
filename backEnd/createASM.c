#include <stdlib.h>
#include <stdio.h>
#include "frontEndDefs.h"

struct genericNode** DAGin;

FILE* outFile;

//reserve r15 as a helper register
//reserve xmm0 as a helper register

//keeps a list of registers for use, what address a currently using them
char* getReg(struct genericNode* me, struct genericNode* mine){
    return "%%reg";
}

char* getByteReg(struct genericNode* me, struct genericNode* mine){
    return "%%reg";
}

void resetRegs(){

}

char* getRaxSize(struct genericNode* me){
    return "%%rax";
}

//turns local variables into offset(%rbp), or if it's a global variable name(%rip)
char* getSymbol(struct genericNode* in){
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
            fprintf(outFile, "\n.globl\t%s\n.p2align 4\n%s:\n", ((struct symbolEntry*)(in->children[0]))->name, ((struct symbolEntry*)(in->children[0]))->name);
            fprintf(outFile, "pushq\t%%r15\npushq\t%%r14\npushq\t%%r13\npushq\t%%r12\npushq\t%%rbx\npushq\t%%rbp\nmovq\t%%rsp, %%rbp\n");
            resetRegs();
            generalNode(((struct symbolEntry*)(in->children[0]))->innerScope);
        }else
            return; //don't write function prototypes
    }else   //print base variable
        fprintf(outFile, "%s: .%s\n", ((struct symbolEntry*)(in->children[0]))->name, getTypeSizeName(in));

}

void labelNode(struct genericNode* in){
    fprintf(outFile, "%s:\n", ((struct symbolEntry*)(in->children[0]->children[0]))->name);
    generalNode(in->children[1]);
}

void gotoNode(struct genericNode* in){
    fprintf(outFile, "jmp\t%s\n", ((struct symbolEntry*)(in->children[0]->children[0]))->name);
}

void scopeNode(struct genericNode* in){
    scopeCounter++;
    for(int i = 0; i < in->childCount; i++)
        generalNode(in->children[i]);

    scopeCounter--;
}

void forNode(struct genericNode* in){

    generalNode(in->children[0]);

    long tempLabel = labelCounter++;
    long exitLabel = labelCounter++;

    loopLabelStackTop[++loopLabelPointerTop] = tempLabel;
    loopLabelStackExit[++loopLabelPointerExit] = exitLabel;

    fprintf(outFile, "L%ld:\n", tempLabel);

    generalNode(in->children[1]);

    fprintf(outFile, "movq\t%s, %%r15\n", getReg(in, in->children[1]));  //movq handles both floats and clearing short ints
    fprintf(outFile, "cmpq\t$0, %%r15\n");
    fprintf(outFile, "jz\tL%ld\n", exitLabel);

    generalNode(in->children[2]);

    fprintf(outFile, "jmp\tL%ld\n", tempLabel);
    fprintf(outFile, "L%ld:\n", exitLabel);

    loopLabelPointerTop--;
    loopLabelPointerExit--;

}

void whileNode(struct genericNode* in){
    
    long tempLabel = labelCounter++;
    long exitLabel = labelCounter++;

    loopLabelStackTop[++loopLabelPointerTop] = tempLabel;
    loopLabelStackExit[++loopLabelPointerExit] = exitLabel;

    fprintf(outFile, "L%ld:\n", tempLabel);

    generalNode(in->children[0]);

    fprintf(outFile, "movq   %s, %%r15\n", getReg(in, in->children[0]));  //movq handles both floats and clearing short ints
    fprintf(outFile, "cmpq   $0, %%r15\n");
    fprintf(outFile, "jz    L%ld\n", exitLabel);

    generalNode(in->children[1]);

    fprintf(outFile, "jmp   L%ld\n", tempLabel);
    fprintf(outFile, "L%ld:\n", exitLabel);

    loopLabelPointerTop--;
    loopLabelPointerExit--;

}

void continueNode(struct genericNode* in){
    fprintf(outFile, "jmp   L%ld\n", loopLabelStackTop[loopLabelPointerTop]);
}

void breakNode(struct genericNode* in){
    fprintf(outFile, "jmp   L%ld\n", loopLabelStackExit[loopLabelPointerExit]);
}

void ifNode(struct genericNode* in){

    generalNode(in->children[0]);

    long tempLabel = labelCounter++;

    fprintf(outFile, "movq   %s, %%r15\n", getReg(in, in->children[0]));  //movq handles both floats and clearing short ints
    fprintf(outFile, "cmpq   $0, %%r15\n");
    fprintf(outFile, "jz    L%ld\n", tempLabel);

    generalNode(in->children[1]);

    fprintf(outFile, "L%ld:\n", tempLabel);
}

void ifElseNode(struct genericNode* in){
    generalNode(in->children[0]);

    long tempLabel = labelCounter++;
    long tempLabelElse = labelCounter++;

    fprintf(outFile, "movq   %%%s, %%r15\n", getReg(in, in->children[0]));  //movq handles both floats and clearing short ints
    fprintf(outFile, "cmpq   $0, %%r15\n");
    fprintf(outFile, "jz    L%ld\n", tempLabel);

    generalNode(in->children[1]);

    fprintf(outFile, "jmp   %ld\nL%ld:\n", tempLabelElse, tempLabel);

    generalNode(in->children[2]);

    fprintf(outFile, "L%ld:\n", tempLabelElse);

}

void derefNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movq\t%s, %%r15\n", getSymbol(in->children[0]));
            fprintf(outFile, "movdqu\t(%%r15), %s\n", getReg(in, in));
        }else{
            generalNode(in->children[0]);
            fprintf(outFile, "movdqu\t(%s), %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movq\t%s, %%r15\n", getSymbol(in->children[0]));
                fprintf(outFile, "mov%c\t(%%r15), %s\n", getType(in), getReg(in, in));
            }else{
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c (%s), %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movq\t%s, %%r15\n", getSymbol(in->children[0]));
                fprintf(outFile, "movs%c\t(%%r15), %s\n", getType(in), getReg(in, in));
            }else{
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c (%s), %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

        }
    }
}

void equNode(struct genericNode* in){

    if(scopeCounter == 0){  //handle scope zero set definitions
        fprintf(outFile, "%s: .%s %s\n", 
        ((struct symbolEntry*)(in->children[0]->children[0]))->name,
        getTypeSizeName(in), 
        ((struct symbolEntry*)(in->children[1]->children[0]))->constValue);
        return;
    }

    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------

        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqu %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(in->children[1]->type == SYMBOL_TYPE){
            fprintf(outFile, "pblendw %s, %s, %ld\n", getSymbol(in->children[1]), getReg(in, in), createSizeMask(in));
        }else{
            generalNode(in->children[1]);
            fprintf(outFile, "pblendw %s, %s, %ld\n", getReg(in, in->children[1]), getReg(in, in), createSizeMask(in));
        }

        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu %s, %s\n", getReg(in, in), getSymbol(in->children[0]));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqu %s, %s\n",  getReg(in, in), getReg(in, in->children[0]));
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }

            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in), getSymbol(in->children[0]));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in), getReg(in, in->children[0]));
            }

        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }

            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getReg(in, in), getSymbol(in->children[0]));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getReg(in, in), getReg(in, in->children[0]));
            }
        }
    }
}

void ternNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------

        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        fprintf(outFile, "pxor %%xmm0, %%xmm0");
        
        if(isInt(in))
            fprintf(outFile, "cmpneqs%c %%xmm0, %s\n", getType(in), getReg(in, in));
        else{
            fprintf(outFile, "pcmpeq%c %%xmm0, %s\n", getType(in), getReg(in, in));
            fprintf(outFile, "pcmpeqb %%xmm0, %%xmm0");
            fprintf(outFile, "pxor  %%xmm0, %s\n", getReg(in, in));
        }

        if(in->children[1]->type == SYMBOL_TYPE && in->children[2]->type == SYMBOL_TYPE){
            fprintf(outFile, "pcmpeqb %%xmm0, %%xmm0");
            fprintf(outFile, "pxor  %s, %%xmm0\n", getReg(in, in));

            fprintf(outFile, "pand  %s, %s\n", getSymbol(in->children[1]), getReg(in, in));
            fprintf(outFile, "pand  %s, %%xmm0\n", getSymbol(in->children[2]));
            fprintf(outFile, "por  %%xmm0, %s\n", getReg(in, in));
        }else
        if(in->children[1]->type == SYMBOL_TYPE && in->children[2]->type != SYMBOL_TYPE){
            generalNode(in->children[2]);
            fprintf(outFile, "pcmpeqb %%xmm0, %%xmm0");
            fprintf(outFile, "pxor  %s, %%xmm0\n", getReg(in, in));

            fprintf(outFile, "pand  %s, %s\n", getSymbol(in->children[1]), getReg(in, in));
            fprintf(outFile, "pand  %s, %%xmm0\n", getReg(in, in->children[2]));
            fprintf(outFile, "por  %%xmm0, %s\n", getReg(in, in));
        }else
        if(in->children[1]->type != SYMBOL_TYPE && in->children[2]->type == SYMBOL_TYPE){
            generalNode(in->children[1]);
            fprintf(outFile, "pcmpeqb %%xmm0, %%xmm0");
            fprintf(outFile, "pxor  %s, %%xmm0\n", getReg(in, in));

            fprintf(outFile, "pand  %s, %s\n", getReg(in, in->children[1]), getReg(in, in));
            fprintf(outFile, "pand  %s, %%xmm0\n", getSymbol(in->children[2]));
            fprintf(outFile, "por  %%xmm0, %s\n", getReg(in, in));
        }else
        if(in->children[1]->type != SYMBOL_TYPE && in->children[2]->type != SYMBOL_TYPE){
            generalNode(in->children[1]);
            generalNode(in->children[2]);
            fprintf(outFile, "pcmpeqb %%xmm0, %%xmm0");
            fprintf(outFile, "pxor  %s, %%xmm0\n", getReg(in, in));

            fprintf(outFile, "pand  %s, %s\n", getReg(in, in->children[1]), getReg(in, in));
            fprintf(outFile, "pand  %s, %%xmm0\n", getReg(in, in->children[2]));
            fprintf(outFile, "por  %%xmm0, %s\n", getReg(in, in));
        }


    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        lastWasVec = 0;
        
        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE && in->children[2]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmp%c  $0, %s\n",  getType(in), getReg(in, in));

                fprintf(outFile, "cmoveq%c  %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
                fprintf(outFile, "cmovne%c  %s, %s\n", getType(in), getSymbol(in->children[2]), getReg(in, in));
            }else
            if(in->children[1]->type == SYMBOL_TYPE && in->children[2]->type != SYMBOL_TYPE){
                generalNode(in->children[2]);
                fprintf(outFile, "cmp%c  $0, %s\n",  getType(in), getReg(in, in));

                fprintf(outFile, "cmoveq%c  %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
                fprintf(outFile, "cmovne%c  %s, %s\n", getType(in), getReg(in, in->children[2]), getReg(in, in));
            }else
            if(in->children[1]->type != SYMBOL_TYPE && in->children[2]->type == SYMBOL_TYPE){
                generalNode(in->children[1]);
                fprintf(outFile, "cmp%c  $0, %s\n",  getType(in), getReg(in, in));

                fprintf(outFile, "cmoveq%c  %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "cmovne%c  %s, %s\n", getType(in), getSymbol(in->children[2]), getReg(in, in));
            }else
            if(in->children[1]->type != SYMBOL_TYPE && in->children[2]->type != SYMBOL_TYPE){
                generalNode(in->children[1]);
                generalNode(in->children[2]);
                fprintf(outFile, "cmp%c  $0, %s\n",  getType(in), getReg(in, in));

                fprintf(outFile, "cmoveq%c  %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "cmovne%c  %s, %s\n", getType(in), getReg(in, in->children[2]), getReg(in, in));
            }

        }else{ // ------------------ FLOATING -------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{
                generalNode(in->children[0]);
                fprintf(outFile, "movs%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            fprintf(outFile, "pxor %%xmm0, %%xmm0");
            fprintf(outFile, "cmpneqs%c %%xmm0, %s\n", getType(in), getReg(in, in));

            if(in->children[1]->type == SYMBOL_TYPE && in->children[2]->type == SYMBOL_TYPE){
                fprintf(outFile, "pcmpeqb %%xmm0, %%xmm0");
                fprintf(outFile, "pxor  %s, %%xmm0\n", getReg(in, in));

                fprintf(outFile, "pand  %s, %s\n", getSymbol(in->children[1]), getReg(in, in));
                fprintf(outFile, "pand  %s, %%xmm0\n", getSymbol(in->children[2]));
                fprintf(outFile, "por  %%xmm0, %s\n", getReg(in, in));
            }else
            if(in->children[1]->type == SYMBOL_TYPE && in->children[2]->type != SYMBOL_TYPE){
                generalNode(in->children[2]);
                fprintf(outFile, "pcmpeqb %%xmm0, %%xmm0");
                fprintf(outFile, "pxor  %s, %%xmm0\n", getReg(in, in));

                fprintf(outFile, "pand  %s, %s\n", getSymbol(in->children[1]), getReg(in, in));
                fprintf(outFile, "pand  %s, %%xmm0\n", getReg(in, in->children[2]));
                fprintf(outFile, "por  %%xmm0, %s\n", getReg(in, in));
            }else
            if(in->children[1]->type != SYMBOL_TYPE && in->children[2]->type == SYMBOL_TYPE){
                generalNode(in->children[1]);
                fprintf(outFile, "pcmpeqb %%xmm0, %%xmm0");
                fprintf(outFile, "pxor  %s, %%xmm0\n", getReg(in, in));

                fprintf(outFile, "pand  %s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pand  %s, %%xmm0\n", getSymbol(in->children[2]));
                fprintf(outFile, "por  %%xmm0, %s\n", getReg(in, in));
            }else
            if(in->children[1]->type != SYMBOL_TYPE && in->children[2]->type != SYMBOL_TYPE){
                generalNode(in->children[1]);
                generalNode(in->children[2]);
                fprintf(outFile, "pcmpeqb %%xmm0, %%xmm0");
                fprintf(outFile, "pxor  %s, %%xmm0\n", getReg(in, in));

                fprintf(outFile, "pand  %s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pand  %s, %%xmm0\n", getReg(in, in->children[2]));
                fprintf(outFile, "por  %%xmm0, %s\n", getReg(in, in));
            }
        }
    }
}

void logicOrNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "por %s, %s\n", getSymbol(in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor %%xmm0, %%xmm0\n");
                fprintf(outFile, "pcmpeq%c %%xmm0, %s\n", getType(in), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "por %s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor %%xmm0, %%xmm0\n");
                fprintf(outFile, "pcmpeq%c %%xmm0, %s\n", getType(in), getReg(in, in));
            }

            fprintf(outFile, "pcmpeqb    %%xmm0, %%xmm0\n");
            fprintf(outFile, "pxor %%xmm0, %s\n", getReg(in, in));

        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "por %s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor %%xmm0, %%xmm0\n");
                fprintf(outFile, "cmpneqp%c %%xmm0, %s\n", getType(in), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "por %s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor %%xmm0, %%xmm0\n");
                fprintf(outFile, "cmpneqp%c %%xmm0, %s\n", getType(in), getReg(in, in));
            }
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

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
                fprintf(outFile, "pxor %%xmm0, %%xmm0\n");
                fprintf(outFile, "cmpneqs%c %%xmm0, %s\n", getType(in), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "por %s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor %%xmm0, %%xmm0\n");
                fprintf(outFile, "cmpneqs%c %%xmm0, %s\n", getType(in), getReg(in, in));
            }
        }
    }
}

void logicAndNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pand %s, %s\n", getSymbol(in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor %%xmm0, %%xmm0\n");
                fprintf(outFile, "pcmpeq%c %%xmm0, %s\n", getType(in), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pand %s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor %%xmm0, %%xmm0\n");
                fprintf(outFile, "pcmpeq%c %%xmm0, %s\n", getType(in), getReg(in, in));
            }

            fprintf(outFile, "pcmpeqb    %%xmm0, %%xmm0\n");
            fprintf(outFile, "pxor %%xmm0, %s\n", getReg(in, in));

        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "andps %s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor %%xmm0, %%xmm0\n");
                fprintf(outFile, "cmpneqp%c %%xmm0, %s\n", getType(in), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "andps %s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor %%xmm0, %%xmm0\n");
                fprintf(outFile, "cmpneqp%c %%xmm0, %s\n", getType(in), getReg(in, in));
            }
        }


    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

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
                fprintf(outFile, "pxor %%xmm0, %%xmm0\n");
                fprintf(outFile, "cmpneqs%c %%xmm0, %s\n", getType(in), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "andps %s, %s\n", getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pxor %%xmm0, %%xmm0\n");
                fprintf(outFile, "cmpneqs%c %%xmm0, %s\n", getType(in), getReg(in, in));
            }
        }
    }
}

void logicalNotNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            fprintf(outFile, "pxor %%xmm0, %%xmm0\n");
            fprintf(outFile, "pcmpeq%c %%xmm0, %s\n", getType(in), getReg(in, in));
            fprintf(outFile, "pcmpeqb    %%xmm0, %%xmm0\n");
            fprintf(outFile, "pxor %%xmm0, %s\n", getReg(in, in));

        }else{ // ------------------ FLOATING --------------------------------------------------------------
            fprintf(outFile, "pxor %%xmm0, %%xmm0\n");
            fprintf(outFile, "cmpneqp%c %%xmm0, %s\n", getType(in), getReg(in, in));
        }

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

            fprintf(outFile, "pxor %%xmm0, %%xmm0\n");
            fprintf(outFile, "cmpneqs%c %%xmm0, %s\n", getType(in), getReg(in, in));
        }
    }
}

void notEquNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }
        
        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pcmpeq%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pcmpeq%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }

            fprintf(outFile, "pcmpeqb    %%xmm0, %%xmm0\n");
            fprintf(outFile, "pxor %%xmm0, %s\n", getReg(in, in));

        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmpneqp%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmpneqp%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
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
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------            
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqa %s, %%xmm0\n", getReg(in, in));
                fprintf(outFile, "pcmpgt%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
                fprintf(outFile, "pcmpeq%c %s, %%xmm0\n", getType(in), getSymbol(in->children[1]));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "movdqa %s, %%xmm0\n", getReg(in, in));
                fprintf(outFile, "pcmpgt%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pcmpeq%c %s, %%xmm0\n", getType(in), getReg(in, in->children[1]));
            }

            fprintf(outFile, "por %%xmm0, %s\n", getReg(in, in));

        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmpnltp%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmpnltp%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
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
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "movdqa %s, %%xmm0\n", getReg(in, in));
                fprintf(outFile, "pcmpgt%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
                fprintf(outFile, "pcmpeq%c %s, %%xmm0\n", getType(in), getSymbol(in->children[1]));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "movdqa %s, %%xmm0\n", getReg(in, in));
                fprintf(outFile, "pcmpgt%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
                fprintf(outFile, "pcmpeq%c %s, %%xmm0\n", getType(in), getReg(in, in->children[1]));
            }

            fprintf(outFile, "por %%xmm0, %s\n", getReg(in, in));
            fprintf(outFile, "pcmpgeb %%xmm0, %%xmm0\n");
            fprintf(outFile, "pxor %%xmm0, %s\n", getReg(in, in));

        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmpltp%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmpltp%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

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
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pcmpgt%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pcmpgt%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }

            fprintf(outFile, "pcmpeqb    %%xmm0, %%xmm0\n");
            fprintf(outFile, "pxor %%xmm0, %s\n", getReg(in, in));

        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmplep%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmplep%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
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
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pcmpeq%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pcmpeq%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        
        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmpeqp%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmpeqp%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

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
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pcmpgt%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pcmpgt%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }

        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "cmpnlep%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "cmpnlep%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

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
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "por %s, %s\n", getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "por %s, %s\n", getReg(in, in->children[1]), getReg(in, in));
            }
        }
        // no floating point and

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

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
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pxor %s, %s\n", getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pxor %s, %s\n", getReg(in, in->children[1]), getReg(in, in));
            }
        }
        // no floating point eor

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

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
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pand %s, %s\n", getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pand %s, %s\n", getReg(in, in->children[1]), getReg(in, in));
            }
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

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
        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            fprintf(outFile, "pcmpeq%c %s, %s\n", getType(in), getReg(in, in), getReg(in, in));
            
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "pxor %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "pxor %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
            }
        }

        // no floating point not

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

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

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

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

        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "psll%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "psll%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
        //no floating point right shift

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

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

        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "psra%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "psra%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }
        //no floating point right shift

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

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
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "psub%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "psub%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "subp%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "subp%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

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
        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }
        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "padd%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "padd%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "addp%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "addp%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------
        
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

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------

            fprintf(stderr, "ERR: Sorry, but packed integer remainder hasn't been implemented yet\n");
            exit(1);

        }
        // there is no floating point mod

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pushq %%rax\npushq    %%rdx\nmovq  %s, %%rax\nxorq %%rdx, %%rdx", getReg(in, in));
                fprintf(outFile, "div%c %s\n", getType(in), getSymbol(in->children[1]));
                fprintf(outFile, "movq  %%rdx, %s\npopq  %%rdx\npopq  %%rax\n", getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pushq %%rax\npushq    %%rdx\nmovq  %s, %%rax\nxorq %%rdx, %%rdx", getReg(in, in));
                fprintf(outFile, "div%c %s\n", getType(in), getReg(in, in->children[1]));
                fprintf(outFile, "movq  %%rdx, %s\npopq  %%rdx\npopq  %%rax\n", getReg(in, in));
            }
        }

        // there is no floating point mod

    }
}

void divNode(struct genericNode* in){
    long vectorSize = isVec(in);
    if(vectorSize){ //-------------------- VECTOR CODE ----------------------------------------------

        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------

            fprintf(stderr, "ERR: Sorry, but packed integer divide hasn't been implemented yet\n");
            exit(1);

        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "divp%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "divp%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

        if(isInt(in)){ // --------------------- INTEGER ---------------------------------------
            if(in->children[0]->type == SYMBOL_TYPE){
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getSymbol(in->children[0]), getReg(in, in));
            }else{    
                generalNode(in->children[0]);
                fprintf(outFile, "mov%c %s, %s\n", getType(in), getReg(in, in->children[0]), getReg(in, in));
            }

            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pushq %%rax\npushq    %%rdx\nmovq  %s, %%rax\nxorq %%rdx, %%rdx", getReg(in, in));
                fprintf(outFile, "div%c %s\n", getType(in), getSymbol(in->children[1]));
                fprintf(outFile, "movq  %%rax, %s\npopq  %%rdx\npopq  %%rax\n", getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pushq %%rax\npushq    %%rdx\nmovq  %s, %%rax\nxorq %%rdx, %%rdx", getReg(in, in));
                fprintf(outFile, "div%c %s\n", getType(in), getReg(in, in->children[1]));
                fprintf(outFile, "movq  %%rax, %s\npopq  %%rdx\npopq  %%rax\n", getReg(in, in));
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

        if(in->children[0]->type == SYMBOL_TYPE){
            fprintf(outFile, "movdqu %s, %s\n", getSymbol(in->children[0]), getReg(in, in));
        }else{    
            generalNode(in->children[0]);
            fprintf(outFile, "movdqa %s, %s\n", getReg(in, in->children[0]), getReg(in, in));
        }

        if(isInt(in)){ // --------------------- INTEGER --------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "pmull%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "pmull%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }else{ // ------------------ FLOATING --------------------------------------------------------------
            if(in->children[1]->type == SYMBOL_TYPE){
                fprintf(outFile, "mulp%c %s, %s\n", getType(in), getSymbol(in->children[1]), getReg(in, in));
            }else{
                generalNode(in->children[1]);
                fprintf(outFile, "mulp%c %s, %s\n", getType(in), getReg(in, in->children[1]), getReg(in, in));
            }
        }

    }else{ //----------------- SCALAR CODE ---------------------------------------------------------------

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

void vecIndexNode(struct genericNode* in){
    if(in->children[0]->type == SYMBOL_TYPE){
        fprintf(outFile, "pextr%c %s, %s, %s\n", getTypeInt(in), getSymbol(in->children[0]), getReg(in, in), getInnerNumber(in->children[1]));
    }else{
        generalNode(in->children[0]);
        fprintf(outFile, "pextr%c %s, %s, %s\n", getTypeInt(in), getReg(in, in->children[0]), getReg(in, in), getInnerNumber(in->children[1]));
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
addq\t$16, %%rsp\n\
movdqu\t%%xmm1, (%%rsp)\n\
addq\t$16, %%rsp\n\
movdqu\t%%xmm2, (%%rsp)\n\
addq\t$16, %%rsp\n\
movdqu\t%%xmm3, (%%rsp)\n\
addq\t$16, %%rsp\n\
movdqu\t%%xmm4, (%%rsp)\n\
addq\t$16, %%rsp\n\
movdqu\t%%xmm5, (%%rsp)\n\
addq\t$16, %%rsp\n\
movdqu\t%%xmm6, (%%rsp)\n\
addq\t$16, %%rsp\n\
movdqu\t%%xmm7, (%%rsp)\n\
addq\t$16, %%rsp\n\
movdqu\t%%xmm8, (%%rsp)\n\
");
}

void postCallPop(){
    fprintf(outFile, "\
movdqu\t(%%rsp), %%xmm8\n\
subq\t$16, %%rsp\n\
movdqu\t(%%rsp), %%xmm7\n\
subq\t$16, %%rsp\n\
movdqu\t(%%rsp), %%xmm6\n\
subq\t$16, %%rsp\n\
movdqu\t(%%rsp), %%xmm5\n\
subq\t$16, %%rsp\n\
movdqu\t(%%rsp), %%xmm4\n\
subq\t$16, %%rsp\n\
movdqu\t(%%rsp), %%xmm3\n\
subq\t$16, %%rsp\n\
movdqu\t(%%rsp), %%xmm2\n\
subq\t$16, %%rsp\n\
movdqu\t(%%rsp), %%xmm1\n\
subq\t$16, %%rsp\n\
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
        {"%%dil", "%%sil", "%%dl", "%%cl", "%%r8b", "%%r9b"},
        {"%%di", "%%si", "%%dx", "%%cx", "%%r8w", "%%r9w"},
        {"%%edi", "%%esi", "%%edx", "%%ecx", "%%r8d", "%%r9d"},
        {"%%rdi", "%%rsi", "%%rdx", "%%rcx", "%%r8", "%%r9"},
        {"%%xmm0", "%%xmm1", "%%xmm2", "%%xmm3", "%%xmm4", "%%xmm5"}
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
            fprintf(outFile, "addq\t$16, %%rsp\nmovdqu\t%%xmm0, (%%rsp)\n"); 
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
            fprintf(outFile, "movdqu\t(%%rsp), %%xmm0\nsubq\t$16, %%rsp\n"); 
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
    fprintf(outFile, "movq\t%%rbp, %%rsp\npopq\t%%rbp\npopq\t%%rbx\npopq\t%%r12\npopq\t%%r13\npopq\t%%r14\npopq\t%%r15\n");
}


void returnExpNode(struct genericNode* in){

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
	generalNode(DAGin[0]);

    fclose(outFile);

	return 0;
}
