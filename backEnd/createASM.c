#include <stdlib.h>
#include <stdio.h>
#include "frontEndDefs.h"

struct genericNode** DAGin;

FILE outFile;

//reserve r15 as vecLoop counter and helper register

//used for typing the data literals
void lastType(struct genericNode* in){

}

//how many elements are in a xmm register of in's type
int  elementCount(struct genericNode* in){

}

//keeps a list of registers for use, what address a currently using them
char* getReg(struct genericNode* me, struct genericNode* mine){

}

//turns local variables into offset(%rsp), or if it's a global variable name(%rip)
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


void scopeNode(struct genericNode* in){
    scopeCounter++;
    for(int i = 0; i < in->childCount; i++)
        generalNode(in->children[i]);

    scopeCounter--;
    free(in);
}

void argumentListNode(struct genericNode* in){}
void permuteListNode(struct genericNode* in){}
void forNode(struct genericNode* in){}
void gotoNode(struct genericNode* in){}
void labelNode(struct genericNode* in){}
void returnNode(struct genericNode* in){}
void returnExpNode(struct genericNode* in){}
void continueNode(struct genericNode* in){}
void breakNode(struct genericNode* in){}
void whileNode(struct genericNode* in){}


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




void equNode(struct genericNode* in){}
void ternNode(struct genericNode* in){}
void logicOrNode(struct genericNode* in){}
void logicAndNode(struct genericNode* in){}
void orNode(struct genericNode* in){}
void eorNode(struct genericNode* in){}
void andNode(struct genericNode* in){}
void equEquNode(struct genericNode* in){}
void notEquNode(struct genericNode* in){}
void ltNode(struct genericNode* in){}
void gtNode(struct genericNode* in){}
void ltEquNode(struct genericNode* in){}
void gtEquNode(struct genericNode* in){}
void lshNode(struct genericNode* in){}
void rshNode(struct genericNode* in){}
void subNode(struct genericNode* in){}


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
            fprintf(outFile, "subq $%ld, \%r15\njns L%ld\n", 128 / elementCount(in), tempLabel);

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



void modNode(struct genericNode* in){}
void divNode(struct genericNode* in){}
void mulNode(struct genericNode* in){}
void runSumNode(struct genericNode* in){}
void runDifNode(struct genericNode* in){}
void derefNode(struct genericNode* in){}
void negNode(struct genericNode* in){}
void logicalNotNode(struct genericNode* in){}
void notNode(struct genericNode* in){}
void permuteNode(struct genericNode* in){}
void vecIndexNode(struct genericNode* in){}
void callNode(struct genericNode* in){}
void callParamNode(struct genericNode* in){}


void generalNode(struct generalNode* in){
    if(in == NULL)
        return;

	switch(in->type){
	case(SYMBOL_TYPE):              symbolNode(in); break;
	case(SCOPE_TYPE):               scopeNode(in); break;
	case(ARGUMENT_LIST_TYPE):       argumentListNode(in); break;
	case(PERMUTE_LIST_TYPE):        permuteListNode(in); break;
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
	case(RUN_DIF_TYPE):             runDifNode(in); break;
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
