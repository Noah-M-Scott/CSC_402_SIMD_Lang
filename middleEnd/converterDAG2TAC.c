
#include <stdlib.h>
#include <stdio.h>
#include "frontEndDefs.h"

struct genericNode** DAGin;

struct tacOp {
	long operation;
	long a;
	long b;
	long c;
	struct tacOp *next;
}




void dagPrint(int in){

	printf("%lx : ", DAGin[in]);

	switch(DAGin[in]->type){
	case(SYMBOL_TYPE):				printf("SYMBOL_TYPE"); break;
	case(SCOPE_TYPE):				printf("SCOPE_TYPE"); break;
	case(INIT_LIST_TYPE):			printf("INITLIST_TYPE"); break;
	case(ARGUMENT_LIST_TYPE):		printf("ARGUMENTLIST_TYPE"); break;
	case(PERMUTE_LIST_TYPE):		printf("PERMUTE_TYPE"); break;
	case(FOR_TYPE):					printf("FOR_TYPE"); break;
	case(GOTO_TYPE):				printf("GOTO_TYPE"); break;
	case(LABEL_TYPE):				printf("LABEL_TYPE"); break;
	case(RETURN_TYPE):				printf("RETURN_TYPE"); break;
	case(RETURN_EXP_TYPE):			printf("RETURN EXP_TYPE"); break;
	case(CONTINUE_TYPE):			printf("COUNTINUE_TYPE"); break;
	case(BREAK_TYPE):				printf("BREAK_TYPE"); break;
	case(DO_WHILE_TYPE):			printf("DOWHILE_TYPE"); break;
	case(WHILE_TYPE):				printf("WHILE_TYPE"); break;
	case(IF_TYPE):					printf("IF_TYPE"); break;
	case(IF_ELSE_TYPE):				printf("IFELSE_TYPE"); break;
	case(COMMA_TYPE):				printf("COMMA_TYPE"); break;
	case(EQU_TYPE):					printf("EQU_TYPE"); break;
	case(TERN_TYPE):				printf("TERN_TYPE"); break;
	case(LOGIC_OR_TYPE):			printf("LOGICOR_TYPE"); break;
	case(LOGIC_AND_TYPE):			printf("LOGICAND_TYPE"); break;
	case(MESH_TYPE):				printf("MESH_TYPE"); break;
	case(OR_TYPE):					printf("OR_TYPE"); break;
	case(EOR_TYPE):					printf("EOR_TYPE"); break;
	case(AND_TYPE):					printf("AND_TYPE"); break;
	case(EQU_EQU_TYPE):				printf("EQUEQU_TYPE"); break;
	case(NOT_EQU_TYPE):				printf("NOTEQU_TYPE"); break;
	case(LT_TYPE):					printf("LT_TYPE"); break;
	case(GT_TYPE):					printf("GT_TYPE"); break;
	case(LT_EQU_TYPE):				printf("LTEQU_TYPE"); break;
	case(GT_EQU_TYPE):				printf("GTEQU_TYPE"); break;
	case(LSH_TYPE):					printf("LSH_TYPE"); break;
	case(RSH_TYPE):					printf("RSH_TYPE"); break;
	case(SUB_TYPE):					printf("SUB_TYPE"); break;
	case(ADD_TYPE):					printf("ADD_TYPE"); break;
	case(MOD_TYPE):					printf("MOD_TYPE"); break;
	case(DIV_TYPE):					printf("DIV_TYPE"); break;
	case(MUL_TYPE):					printf("MUL_TYPE"); break;
	case(PUNN_TYPE):				printf("PUNN_TYPE"); break;
	case(RUN_SUM_TYPE):				printf("RUNSUM_TYPE"); break;
	case(RUN_DIF_TYPE):				printf("RUNDIF_TYPE"); break;
	case(DEREF_TYPE):				printf("DEREF_TYPE"); break;
	case(NEG_TYPE):					printf("NEG_TYPE"); break;
	case(ABS_TYPE):					printf("ABS_TYPE"); break;
	case(LOGICAL_NOT_TYPE):			printf("LOGICNOT_TYPE"); break;
	case(NOT_TYPE):					printf("NOT_TYPE"); break;
	case(PERMUTE_TYPE):				printf("PERMUTE_TYPE"); break;
	case(VEC_INDEX_TYPE):			printf("VECINDEX_TYPE"); break;
	case(CALL_TYPE):				printf("CALL_TYPE"); break;
	case(CALL_PARAM_TYPE):			printf("CALLPARAM_TYPE"); break;	
	case(INDEX_TYPE):				printf("INDEX_TYPE"); break;
	}

	printf("	| ");

	for(int i = 0; i < DAGin[in]->childCount; i++)
		printf("%lx : ", DAGin[in]->children[i]);


	printf("\n");
}



int middleEnd(struct genericNode* DAGinin[]){
	
	DAGin = DAGinin;

	for(int i = 0; DAGin[i] != NULL; i++)
		dagPrint(i);


	return 0;
}
