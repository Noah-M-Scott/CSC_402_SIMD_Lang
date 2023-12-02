%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>

unsigned long GLOBAL_LINE_NUMBER;
unsigned long IN_COM = 0;
#include "frontEnd/nodeDAG.h"

int yylex(void);
void yyerror(char const *);

FILE *inFile;
FILE *outFile;
%}

%define api.value.type union
%token <char*> DECIMAL
%token <char*> HEX
%token <char*> BINARY
%token <char*> FLOAT
%token <char*> IDENT
%token <char*> STRING_LIT
%token LTLT_OP
%token GTGT_OP
%token EQUEQU_OP
%token UGT_OP
%token ULT_OP
%token UGTEQU_OP
%token ULTEQU_OP
%token GTEQU_OP
%token LTEQU_OP
%token NOTEQU_OP
%token ANDAND_OP
%token OROR_OP
%token DOTDOTDOT_OP

%token ADD_BAR_OP
%token SUB_BAR_OP

%token EXTERN_OP
%token GLOBAL_OP
%token VOID_OP
%token BYTE_OP
%token WORD_OP
%token LONG_OP
%token QUAD_OP
%token SINGLE_OP
%token DOUBLE_OP

%token CONST_OP
%token SHARED_OP

%token BLOCK_OP
%token SET_OP
%token IF_OP
%token ELSE_OP
%token WHILE_OP
%token DO_OP
%token FOR_OP
%token BREAK_OP
%token CONTINUE_OP
%token SWITCH_OP
%token CASE_OP
%token DEFAULT_OP
%token RETURN_OP
%token GOTO_OP

%token NULL_OP

%token INCLUDE_OP
%token IFDEF_OP
%token IFNDEF_OP
%token THEN_OP
%token DEFINE_OP
%token UNDEF_OP
%token NOTHING_OP


%nterm <struct genericNode*> constant
%nterm <struct genericNode*> initial_expression
%nterm <char*> initializer_list
%nterm <struct genericNode*> postfix_operation
%nterm <struct genericNode*> permute_list
%nterm <struct genericNode*> argument_list
%nterm <struct genericNode*> prefix_operation
%nterm <struct genericNode*> multdiv_operation
%nterm <struct genericNode*> addsub_operation
%nterm <struct genericNode*> shift_operation
%nterm <struct genericNode*> relation_operation
%nterm <struct genericNode*> equality_operation
%nterm <struct genericNode*> bitwise_and_operation
%nterm <struct genericNode*> bitwise_eor_operation
%nterm <struct genericNode*> bitwise_or_operation
%nterm <struct genericNode*> logical_and_operation
%nterm <struct genericNode*> logical_or_operation
%nterm <struct genericNode*> ternary_operation
%nterm <struct genericNode*> expression
%nterm <struct genericNode*> declaration
%nterm <struct genericNode*> variable_declaration
%nterm <struct genericNode*> function_declaration
%nterm <struct genericNode*> declaration_init_list
%nterm <char> hint_modifier
%nterm <char*> base_type
%nterm <char*> base_type_postfix
%nterm <char*> pointer_modifier
%nterm <char*> function_modbase
%nterm <char*> function_modifier
%nterm <char*> type_name
%nterm <char*> parameter_list
%nterm <struct genericNode*> statement
%nterm <struct genericNode*> scope

%%

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
input:
	  %empty
	| input statement
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
constant:
	  DECIMAL			{ $$ = registerNode(registerSymbol(createImmediate($1,  0)));  }
	| FLOAT				{ $$ = registerNode(registerSymbol(createImmediate($1,  3)));  }
	| BINARY			{ $$ = registerNode(registerSymbol(createImmediate($1,  2)));  }
	| HEX				{ $$ = registerNode(registerSymbol(createImmediate($1,  1)));  }
	| BYTE_OP			{ $$ = registerNode(registerSymbol(createImmediate("1", 0)));  } //these replace sizeof
	| WORD_OP			{ $$ = registerNode(registerSymbol(createImmediate("2", 0)));  }
	| LONG_OP			{ $$ = registerNode(registerSymbol(createImmediate("4", 0)));  }
	| QUAD_OP			{ $$ = registerNode(registerSymbol(createImmediate("8", 0)));  }
	| SINGLE_OP			{ $$ = registerNode(registerSymbol(createImmediate("4", 0)));  }
	| DOUBLE_OP			{ $$ = registerNode(registerSymbol(createImmediate("8", 0)));  }
	| NULL_OP			{ $$ = registerNode(registerSymbol(createImmediate("0", 0)));  }
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
initial_expression: 
	  IDENT				{ $$ = registerNode(registerSymbol(createRef($1)));          } //create referance
	| constant			{ $$ = $1;                                                   } //passthrough
	| STRING_LIT			{ $$ = registerNode(registerSymbol(createImmediate($1, 4))); } //string immediate (char*)
	| initializer_list '\\'		{ $$ = registerNode(registerSymbol(createImmediate($1, 6))); } //like string lit, but of const type
	| DOTDOTDOT_OP			{ struct genericNode* temp = malloc(sizeof(struct genericNode)); 
					  temp->childCount = 0; 
					  temp->type = DOTDOTDOT_TYPE;			// '...' is a void pointer to stack base + offset
					  memset(temp->modString, NONE_MOD, 32);
					  temp->modString[0] = VOID_BASE;
					  temp->modString[1] = POINTER_POSTFIX;	//create a void pointer
					  $$ = registerNode(temp); 
					}
	| '(' expression ')'		{ $$ = $2; }	//pass
	| '&' IDENT			{ $$ = registerNode(registerSymbol(createImmediate($2, 5))); } //get data label
	| '[' expression ']'		{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
					  temp->type = DEREF_TYPE;
					  memcpy(temp->modString, $2->modString, 32);
					  temp->childCount = 1;
					  temp->children[0] = $2;
					  enforcePointer($2);			//require a pointer
					  $$ = registerNode(fetchMod(temp));	//undo the pointer
			 	       	}
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
initializer_list:
	  '\\' DECIMAL			{ dataLitIndex = strlen($2);  dataLitType = 0; $$ = malloc(dataLitIndex + 1); strcpy($$, $2);  }
	| '\\' FLOAT			{ dataLitIndex = strlen($2);  dataLitType = 1; $$ = malloc(dataLitIndex + 1); strcpy($$, $2);  }
	| '\\' BINARY			{ dataLitIndex = strlen($2);  dataLitType = 0; $$ = malloc(dataLitIndex + 1); strcpy($$, $2);  }
	| '\\' HEX			{ dataLitIndex = strlen($2);  dataLitType = 0; $$ = malloc(dataLitIndex + 1); strcpy($$, $2);  }

	| '\\' BYTE_OP			{ dataLitIndex = strlen("1"); dataLitType = 0; $$ = malloc(dataLitIndex + 1); strcpy($$, "1"); }
	| '\\' WORD_OP			{ dataLitIndex = strlen("2"); dataLitType = 0; $$ = malloc(dataLitIndex + 1); strcpy($$, "2"); }
	| '\\' LONG_OP			{ dataLitIndex = strlen("4"); dataLitType = 0; $$ = malloc(dataLitIndex + 1); strcpy($$, "4"); }
	| '\\' QUAD_OP			{ dataLitIndex = strlen("8"); dataLitType = 0; $$ = malloc(dataLitIndex + 1); strcpy($$, "8"); }
	| '\\' SINGLE_OP		{ dataLitIndex = strlen("4"); dataLitType = 0; $$ = malloc(dataLitIndex + 1); strcpy($$, "4"); }
	| '\\' DOUBLE_OP		{ dataLitIndex = strlen("8"); dataLitType = 0; $$ = malloc(dataLitIndex + 1); strcpy($$, "8"); }
	| '\\' NULL_OP			{ dataLitIndex = strlen("0"); dataLitType = 0; $$ = malloc(dataLitIndex + 1); strcpy($$, "0"); }

	| initializer_list ',' DECIMAL	{ checkDataLitType(0); $$ = realloc($1, dataLitIndex+strlen($3)+1); strcpy(&$$[dataLitIndex], $3); dataLitIndex += strlen($3); }
	| initializer_list ',' FLOAT	{ checkDataLitType(1); $$ = realloc($1, dataLitIndex+strlen($3)+1); strcpy(&$$[dataLitIndex], $3); dataLitIndex += strlen($3); }
	| initializer_list ',' BINARY	{ checkDataLitType(0); $$ = realloc($1, dataLitIndex+strlen($3)+1); strcpy(&$$[dataLitIndex], $3); dataLitIndex += strlen($3); }
	| initializer_list ',' HEX	{ checkDataLitType(0); $$ = realloc($1, dataLitIndex+strlen($3)+1); strcpy(&$$[dataLitIndex], $3); dataLitIndex += strlen($3); }

	| initializer_list ',' BYTE_OP	{ checkDataLitType(0); $$ = realloc($1, dataLitIndex+1+1); strcpy(&$$[dataLitIndex], "1"); dataLitIndex += strlen("1"); }
	| initializer_list ',' WORD_OP	{ checkDataLitType(0); $$ = realloc($1, dataLitIndex+1+1); strcpy(&$$[dataLitIndex], "2"); dataLitIndex += strlen("2"); }
	| initializer_list ',' LONG_OP	{ checkDataLitType(0); $$ = realloc($1, dataLitIndex+1+1); strcpy(&$$[dataLitIndex], "4"); dataLitIndex += strlen("4"); }
	| initializer_list ',' QUAD_OP	{ checkDataLitType(0); $$ = realloc($1, dataLitIndex+1+1); strcpy(&$$[dataLitIndex], "8"); dataLitIndex += strlen("8"); }
	| initializer_list ',' SINGLE_OP{ checkDataLitType(0); $$ = realloc($1, dataLitIndex+1+1); strcpy(&$$[dataLitIndex], "4"); dataLitIndex += strlen("4"); }
	| initializer_list ',' DOUBLE_OP{ checkDataLitType(0); $$ = realloc($1, dataLitIndex+1+1); strcpy(&$$[dataLitIndex], "8"); dataLitIndex += strlen("8"); }
	| initializer_list ',' NULL_OP	{ checkDataLitType(0); $$ = realloc($1, dataLitIndex+1+1); strcpy(&$$[dataLitIndex], "0"); dataLitIndex += strlen("0"); }
	;



//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
postfix_operation:
	  initial_expression				{ $$ = $1; }
	| postfix_operation '(' ')'			{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
							  temp->type = CALL_TYPE;
							  memcpy(temp->modString, $1->modString, 32);
							  temp->childCount = 1;
							  temp->children[0] = $1;
							  requireFunctions($1);
							  $$ = registerNode(fetchFunc(temp)); //this operates on function pointers not functions
							} 
	| postfix_operation argument_list ')'		{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*)*2);
							  temp->type = CALL_PARAM_TYPE;
							  memcpy(temp->modString, $1->modString, 32);
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $2;
							  requireFunctions($1);
							  checkArgumentTypes($1, $2);
							  $$ = registerNode(fetchFunc(temp)); //same here
							}
	| postfix_operation permute_list '>'	 	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = PERMUTE_TYPE;
							  memcpy(temp->modString, $1->modString, 32);
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $2;
							  checkLengths($1, $2);
							  requireVecs($1);
							  $$ = registerNode(temp);
							}
	| postfix_operation '<' constant '>'  		{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = VEC_INDEX_TYPE;
							  memcpy(temp->modString, $1->modString, 32);
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $3;
							  enforceScalerInts($3);
							  requireVecs($1);
							  $$ = registerNode(undoVector(temp));
						        }
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
permute_list:
	  '<' constant				{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
						  temp->type = PERMUTE_LIST_TYPE;
						  memset(temp->modString, NONE_MOD, 32);
						  temp->modString[0] = VECTOR_MOD;
						  temp->modString[1] = -1;		//start with a length of one
						  temp->modString[2] = QUAD_BASE;
						  temp->childCount = 1;
						  temp->children[0] = $2;
						  enforceScalerInts($2);	//no indexing with floats
						  $$ = registerNode(temp);	//create head, like initList
				 	      	}
	| permute_list ',' constant		{ enforceScalerInts($3);
						  $1->modString[1] -= 1;	//add one to the length
						  $$ = appendAChild($1, $3);	//add the constant to the list
				 	      	}
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
argument_list:
	  '(' ternary_operation			 { struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
						   temp->type = ARGUMENT_LIST_TYPE;
						   memset(temp->modString, NONE_MOD, 32);
						   temp->childCount = 1;
						   temp->children[0] = $2;
						   $$ = registerNode(temp);	//create the head, like above
				 	       	 }
	| argument_list ',' ternary_operation 	 { $$ = appendAChild($1, $3); /* add a argument to the list */ }
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
prefix_operation:
	  postfix_operation			{ $$ = $1; }
	| '~' prefix_operation			{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
						  temp->type = NOT_TYPE;
						  memcpy(temp->modString, $2->modString, 32);
						  temp->childCount = 1;
						  temp->children[0] = $2;
						  excludeFloats($2);		//no bitwise on floats
						  $$ = registerNode(temp);
			 		       	}
	| '!' prefix_operation			{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
						  temp->type = LOGICAL_NOT_TYPE;
						  memcpy(temp->modString, $2->modString, 32);
						  temp->childCount = 1;
						  temp->children[0] = $2;
						  excludeFloats($2);		//no logic on floats
						  $$ = registerNode(temp);
				 	       	}
	| '+' prefix_operation			{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
						  temp->type = ABS_TYPE;
						  memcpy(temp->modString, $2->modString, 32);
						  temp->childCount = 1;
						  temp->children[0] = $2;
						  $$ = registerNode(temp);
			 		       	}
	| '-' prefix_operation			{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
						  temp->type = NEG_TYPE;
						  memcpy(temp->modString, $2->modString, 32);
						  temp->childCount = 1;
						  temp->children[0] = $2;
						  $$ = registerNode(temp);
				 	       	}
	| ADD_BAR_OP prefix_operation		{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
						  temp->type = RUN_SUM_TYPE;
						  memcpy(temp->modString, $2->modString, 32);
						  temp->childCount = 1;
						  temp->children[0] = $2;
						  requireVecs($2);		//require vecs for running sum
						  $$ = registerNode(temp);
				 	       	}
	| SUB_BAR_OP prefix_operation		{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
						  temp->type = RUN_DIF_TYPE;
						  memcpy(temp->modString, $2->modString, 32);
						  temp->childCount = 1;
						  temp->children[0] = $2;
						  requireVecs($2);		//require vecs for running dif
						  $$ = registerNode(temp);
			 	      	 	}
	| '(' type_name ')' prefix_operation   	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
						  temp->type = PUNN_TYPE;
						  memcpy(temp->modString, $2, 32);
						  temp->childCount = 1;
						  temp->children[0] = $4;
						  $$ = registerNode(temp);	//for both of these, it's fine not to track the type_name; we don't need it
			 		       	}
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
multdiv_operation:
	  prefix_operation				{ $$ = $1; }
	| multdiv_operation '*' prefix_operation	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = MUL_TYPE;
							  memcpy(temp->modString, $1->modString, 32);
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $3;
							  compareTypes($1, $3);
							  $$ = registerNode(temp);
			 			       	}
	| multdiv_operation '/' prefix_operation	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = DIV_TYPE;
							  memcpy(temp->modString, $1->modString, 32);
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $3;
							  compareTypes($1, $3);
							  $$ = registerNode(temp);
			 			       	}
	| multdiv_operation '%' prefix_operation	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = MOD_TYPE;
							  memcpy(temp->modString, $1->modString, 32);
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $3;
							  compareTypes($1, $3);
							  excludeFloats($1);
							  $$ = registerNode(temp);
			 			       	}
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
addsub_operation:
	  multdiv_operation				{ $$ = $1; }
	| addsub_operation '+' multdiv_operation	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = ADD_TYPE;
							  memcpy(temp->modString, $1->modString, 32);
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $3;
							  compareTypes($1, $3);
							  $$ = registerNode(temp);
			 			       	}
	| addsub_operation '-' multdiv_operation	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = SUB_TYPE;
							  memcpy(temp->modString, $1->modString, 32);
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $3;
							  compareTypes($1, $3);
							  $$ = registerNode(temp);
			 			       	}

	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
shift_operation:
	  addsub_operation				{ $$ = $1; }
	| shift_operation GTGT_OP addsub_operation	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = RSH_TYPE;
							  memcpy(temp->modString, $1->modString, 32);
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $3;
							  compareTypes($1, $3);
							  excludeFloats($1);
							  $$ = registerNode(temp);
			 			       	}

	| shift_operation LTLT_OP addsub_operation	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = LSH_TYPE;
							  memcpy(temp->modString, $1->modString, 32);
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $3;
							  compareTypes($1, $3);
							  excludeFloats($1);
							  $$ = registerNode(temp);
			 			       	}
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
relation_operation:
	  shift_operation				{ $$ = $1; }

	| relation_operation '<' shift_operation	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = LT_TYPE;
							  memset(temp->modString, NONE_MOD, 32);
							  matchLength(temp->modString, $1->modString);
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $3;
							  compareTypes($1, $3);
							  $$ = registerNode(temp);
			 			       	}

	| relation_operation '>' shift_operation	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = GT_TYPE;
							  memset(temp->modString, NONE_MOD, 32);
							  matchLength(temp->modString, $1->modString);
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $3;
							  compareTypes($1, $3);
							  $$ = registerNode(temp);
			 			       	}

	| relation_operation LTEQU_OP shift_operation	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = LT_EQU_TYPE;
							  memset(temp->modString, NONE_MOD, 32);
							  matchLength(temp->modString, $1->modString);
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $3;
							  compareTypes($1, $3);
							  $$ = registerNode(temp);
			 			       	}

	| relation_operation GTEQU_OP shift_operation	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = GT_EQU_TYPE;
							  memset(temp->modString, NONE_MOD, 32);
							  matchLength(temp->modString, $1->modString);
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $3;
							  compareTypes($1, $3);
							  $$ = registerNode(temp);
			 			       	}

	| relation_operation '<' shift_operation ':' expression	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 3);
								  temp->type = LT_TERN_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
							  	  matchLength(temp->modString, $1->modString);
								  temp->childCount = 3;
								  temp->children[0] = $1;
								  temp->children[1] = $3;
								  temp->children[2] = $5;
								  compareTypes($1, $3);
								  compareTypes($1, $5);
								  $$ = registerNode(temp);
			 				       	}

	| relation_operation '>' shift_operation ':' expression	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 3);
								  temp->type = GT_TERN_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
							 	  matchLength(temp->modString, $1->modString);
								  temp->childCount = 3;
								  temp->children[0] = $1;
								  temp->children[1] = $3;
								  temp->children[2] = $5;
								  compareTypes($1, $3);
								  compareTypes($1, $5);
								  $$ = registerNode(temp);
			 				       	}

	| relation_operation LTEQU_OP shift_operation ':' expression	{ struct genericNode* temp = malloc(sizeof(struct genericNode)+sizeof(struct genericNode*)*3);
									  temp->type = LT_EQU_TERN_TYPE;
							 		  memset(temp->modString, NONE_MOD, 32);
							 		  matchLength(temp->modString, $1->modString);
									  temp->childCount = 3;
									  temp->children[0] = $1;
									  temp->children[1] = $3;
									  temp->children[2] = $5;
									  compareTypes($1, $3);
									  compareTypes($1, $5);
									  $$ = registerNode(temp);
			 				   	    	}

	| relation_operation GTEQU_OP shift_operation ':' expression	{ struct genericNode* temp = malloc(sizeof(struct genericNode)+sizeof(struct genericNode*)*3);
									  temp->type = GT_EQU_TERN_TYPE;
									  memset(temp->modString, NONE_MOD, 32);
									  matchLength(temp->modString, $1->modString);
									  temp->childCount = 3;
									  temp->children[0] = $1;
									  temp->children[1] = $3;
									  temp->children[2] = $5;
									  compareTypes($1, $3);
									  compareTypes($1, $5);
									  $$ = registerNode(temp);
			 					       	}
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
equality_operation:
	  relation_operation				{ $$ = $1; }

	| equality_operation EQUEQU_OP relation_operation	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
								  temp->type = EQU_EQU_TYPE;
								  memset(temp->modString, NONE_MOD, 32);
								  matchLength(temp->modString, $1->modString);
								  temp->childCount = 2;
								  temp->children[0] = $1;
								  temp->children[1] = $3;
								  compareTypes($1, $3);
								  $$ = registerNode(temp);
			 				       	}

	| equality_operation NOTEQU_OP relation_operation	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
								  temp->type = NOT_EQU_TYPE;
								  memset(temp->modString, NONE_MOD, 32);
								  matchLength(temp->modString, $1->modString);
								  temp->childCount = 2;
								  temp->children[0] = $1;
								  temp->children[1] = $3;
								  compareTypes($1, $3);
								  $$ = registerNode(temp);
			 				       	}

	| equality_operation EQUEQU_OP relation_operation ':' expression { struct genericNode* temp = malloc(sizeof(struct genericNode)+sizeof(struct genericNode*)*3);
									   temp->type = EQU_EQU_TERN_TYPE;
							 		   memset(temp->modString, NONE_MOD, 32);
									   matchLength(temp->modString, $1->modString);
									   temp->childCount = 3;
									   temp->children[0] = $1;
									   temp->children[1] = $3;
									   temp->children[2] = $5;
									   compareTypes($1, $3);
									   compareTypes($1, $5);
									   $$ = registerNode(temp);
			 					       	 }

	| equality_operation NOTEQU_OP relation_operation ':' expression { struct genericNode* temp = malloc(sizeof(struct genericNode)+sizeof(struct genericNode*)*3);
									   temp->type = NOT_EQU_TERN_TYPE;
									   memset(temp->modString, NONE_MOD, 32);
									   matchLength(temp->modString, $1->modString);
									   temp->childCount = 3;
									   temp->children[0] = $1;
									   temp->children[1] = $3;
									   temp->children[2] = $5;
									   compareTypes($1, $3);
									   compareTypes($1, $5);
									   $$ = registerNode(temp);
			 					       	 }

	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
bitwise_and_operation:
	  equality_operation				{ $$ = $1; }

	| bitwise_and_operation '&' equality_operation	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = AND_TYPE;
							  memcpy(temp->modString, $1->modString, 32);
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $3;
							  compareTypes($1, $3);
							  excludeFloats($1);
							  $$ = registerNode(temp);
			 			       	}
	;



//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
bitwise_eor_operation:
	  bitwise_and_operation				  { $$ = $1; }

	| bitwise_eor_operation '^' bitwise_and_operation { struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							    temp->type = EOR_TYPE;
							    memcpy(temp->modString, $1->modString, 32);
							    temp->childCount = 2;
							    temp->children[0] = $1;
							    temp->children[1] = $3;
							    compareTypes($1, $3);
							    excludeFloats($1);
							    $$ = registerNode(temp);
			 			       	  }
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
bitwise_or_operation:
	  bitwise_eor_operation				 { $$ = $1; }

	| bitwise_or_operation '|' bitwise_eor_operation { struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							   temp->type = OR_TYPE;
							   memcpy(temp->modString, $1->modString, 32);
							   temp->childCount = 2;
							   temp->children[0] = $1;
							   temp->children[1] = $3;
							   compareTypes($1, $3);
							   excludeFloats($1);
							   $$ = registerNode(temp);
			 			       	 }

	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
logical_and_operation:
	  bitwise_or_operation				  { $$ = $1; }

	| logical_and_operation ANDAND_OP bitwise_or_operation  
							  { struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							    temp->type = LOGIC_AND_TYPE;
							    memset(temp->modString, NONE_MOD, 32);
							    matchLength(temp->modString, $1->modString);
							    temp->childCount = 2;
							    temp->children[0] = $1;
							    temp->children[1] = $3;
							    compareTypes($1, $3);
							    excludeFloats($1);
							    $$ = registerNode(temp);
						       	  }
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
logical_or_operation:
	  logical_and_operation				{ $$ = $1; }

	| logical_or_operation OROR_OP logical_and_operation	  { struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
								    temp->type = LOGIC_OR_TYPE;
								    memset(temp->modString, NONE_MOD, 32);
								    matchLength(temp->modString, $1->modString);
								    temp->childCount = 2;
								    temp->children[0] = $1;
								    temp->children[1] = $3;
								    compareTypes($1, $3);
								    excludeFloats($1);
								    $$ = registerNode(temp);
							       	  }

	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
ternary_operation:
	  logical_or_operation				{ $$ = $1; }
	| logical_or_operation '?' expression ':' ternary_operation	 { struct genericNode* temp = malloc(sizeof(struct genericNode)+sizeof(struct genericNode*)*3);
									   temp->type = TERN_TYPE;
									   memcpy(temp->modString, $3->modString, 32);
									   temp->childCount = 3;
									   temp->children[0] = $1;
									   temp->children[1] = $3;
									   temp->children[2] = $5;
									   compareTypes($3, $5);
							 		   excludeFloats($1);
									   $$ = registerNode(temp);
			 					       	 }
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
expression:
	  ternary_operation	{ $$ = $1; }
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
variable_declaration:
	  type_name declaration_init_list ';' { $$ = $2; }
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
function_declaration:
	  type_name IDENT scope				    { struct symbolEntry* temp = malloc(sizeof(struct symbolEntry));
							      memcpy(temp->modString, $1, 32);
							      memcpy(temp->name, $2, 128);
							      temp->innerScope = $3;
							      requireFunctionsType($1);
							      enforceZeroScope();
							      closeFalseScope();			//close the false scope created by function mod
							      $$ = registerNodeFunction(registerSymbol(temp));
							    }
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
declaration:
	  variable_declaration		 { $$ = $1; }
	| function_declaration		 { $$ = $1; }
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
declaration_init_list:
	  IDENT '=' expression				{ closeFalseScope(); //close the previous false scope from typename
							  excludeFunctionsType(globalIPT);
							  createIPTSymbol($1); //create a symbol using the inprocess type
							  struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = EQU_TYPE;
							  memcpy(temp->modString, $3->modString, 32);
							  temp->childCount = 2;
							  temp->children[0] = registerNode(registerSymbol(createRef($1)));
							  temp->children[1] = $3;
							  compareTypes(temp->children[0], $3);
							  struct genericNode* temp2 = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
							  temp2->type = DEC_LIST_TYPE;	//create the dec list header
							  memset(temp->modString, NONE_MOD, 32);
							  temp2->childCount = 1;
							  temp2->children[0] = temp;
							  $$ = registerNode(temp2);	//create the head
						        }
	| IDENT						{ closeFalseScope(); //close the previous false scope from typename
							  excludeFunctionsType(globalIPT);
							  createIPTSymbol($1);	//in process symbol creation
							  struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
							  temp->type = DEC_LIST_TYPE;
							  memset(temp->modString, NONE_MOD, 32);
							  temp->childCount = 1;
							  temp->children[0] = registerNode(registerSymbol(createRef($1)));
							  $$ = registerNode(temp);	//create the head, like above
				 		       	}
	| declaration_init_list ',' IDENT '=' expression{ createIPTSymbol($3); //create a symbol using the inprocess type
							  struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = EQU_TYPE;
							  memcpy(temp->modString, $5->modString, 32);
							  temp->childCount = 2;
							  temp->children[0] = registerNode(registerSymbol(createRef($3)));
							  temp->children[1] = $5;
							  compareTypes(temp->children[0], $5);
							  $$ = appendAChild($1, temp);	//append equals
						        }
	| declaration_init_list ',' IDENT		{ createIPTSymbol($3); $$ = appendAChild($1, registerNode(registerSymbol(createRef($3)))); }	
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
hint_modifier:
	  %empty 	{ $$ = 0; }
	| EXTERN_OP	{ $$ = EXTERN_HINT; enforceZeroScope(); }	
	| GLOBAL_OP	{ $$ = GLOBAL_HINT; enforceZeroScope(); }
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
base_type:
	  hint_modifier VOID_OP		{ char *temp = malloc(32); globalTypeIndex = 1; int bump = $1; temp[0] = $1; temp[(bump?1:0)] = VOID_BASE;   $$ = temp; }
	| hint_modifier BYTE_OP		{ char *temp = malloc(32); globalTypeIndex = 1; int bump = $1; temp[0] = $1; temp[(bump?1:0)] = BYTE_BASE;   $$ = temp; }
	| hint_modifier WORD_OP		{ char *temp = malloc(32); globalTypeIndex = 1; int bump = $1; temp[0] = $1; temp[(bump?1:0)] = WORD_BASE;   $$ = temp; }
	| hint_modifier LONG_OP		{ char *temp = malloc(32); globalTypeIndex = 1; int bump = $1; temp[0] = $1; temp[(bump?1:0)] = LONG_BASE;   $$ = temp; }
	| hint_modifier QUAD_OP		{ char *temp = malloc(32); globalTypeIndex = 1; int bump = $1; temp[0] = $1; temp[(bump?1:0)] = QUAD_BASE;   $$ = temp; }
	| hint_modifier SINGLE_OP	{ char *temp = malloc(32); globalTypeIndex = 1; int bump = $1; temp[0] = $1; temp[(bump?1:0)] = SINGLE_BASE; $$ = temp; }
	| hint_modifier DOUBLE_OP	{ char *temp = malloc(32); globalTypeIndex = 1; int bump = $1; temp[0] = $1; temp[(bump?1:0)] = DOUBLE_BASE; $$ = temp; }
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
base_type_postfix:
	  base_type				{ $$ = $1; }
	| base_type '<' constant '>'		{ $1[2] = $1[0]; 
						  $1[0] = VECTOR_MOD; 
						  enforceScalerInts($3);
						  long temp;
						  sscanf( ((struct symbolEntry*)($3->children[0]))->constValue, "%ld", &temp);
						  if(temp > 128 || temp < 1){
							fprintf(stderr, "ERR: vector constant too large (not 1 to 128), %ld", temp);
							exit(1);
						  }
						  $1[1] = (char)(-temp);
						  globalTypeIndex += 2;
						  $$ = $1;
						}
	| SHARED_OP base_type			{ $2[1] = $2[0]; $2[0] = SHARED_MOD; globalTypeIndex += 1; $$ = $2; }
	| SHARED_OP base_type '<' constant '>'	{ $2[3] = $2[0]; 
						  $2[0] = SHARED_MOD; 
						  $2[1] = VECTOR_MOD; 
						  enforceScalerInts($4);
						  long temp;
						  sscanf( ((struct symbolEntry*)($4->children[0]))->constValue, "%ld", &temp);
						  if(temp > 128 || temp < 1){
							fprintf(stderr, "ERR: vector constant too large (not 1 to 128), %ld", temp);
							exit(1);
						  }
						  globalTypeIndex += 3;
						  $2[2] = (char)(-temp);
						  $$ = $2;
						}
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
pointer_modifier:
	  '[' base_type_postfix ']'		{ $1[globalTypeIndex++] = POINTER_POSTFIX; $$ = $1; }
	| '[' pointer_modifier  ']'		{ $1[globalTypeIndex++] = POINTER_POSTFIX; $$ = $1; }
	| '[' function_modifier ']'		{ $1[globalTypeIndex++] = POINTER_POSTFIX; $$ = $1; }
	| '[' SHARED_OP base_type_postfix ']'	{ $1[globalTypeIndex++] = SHARED_MOD; $1[globalTypeIndex++] = POINTER_POSTFIX; $$ = $1; }
	| '[' SHARED_OP pointer_modifier  ']'	{ $1[globalTypeIndex++] = SHARED_MOD; $1[globalTypeIndex++] = POINTER_POSTFIX; $$ = $1; }
	| '[' SHARED_OP function_modifier ']'	{ $1[globalTypeIndex++] = SHARED_MOD; $1[globalTypeIndex++] = POINTER_POSTFIX; $$ = $1; }
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
function_modbase:
	  base_type_postfix '(' { openFalseScope(); $1[globalTypeIndex++] = FUNCTION_POSTFIX; pushTypeIndex(); $$ = $1; }
 	| pointer_modifier  '(' { openFalseScope(); $1[globalTypeIndex++] = FUNCTION_POSTFIX; pushTypeIndex(); $$ = $1; }
	;


function_modifier:
	  function_modbase ')'			{ copyAndPopTypeIndex($1, NULL); $1[globalTypeIndex++] = CLOSE_FUNCTION_POSTFIX; $$ = $1; }
	| function_modbase parameter_list ')'	{ copyAndPopTypeIndex($1, $2); free($2); $1[globalTypeIndex++] = CLOSE_FUNCTION_POSTFIX; $$ = $1; }

	| function_modbase parameter_list ',' DOTDOTDOT_OP ')'	
						{ copyAndPopTypeIndex($1, $2); 
						  free($2); 
						  $1[globalTypeIndex++] = DOTDOTDOT_BASE; 
						  $1[globalTypeIndex++] = CLOSE_FUNCTION_POSTFIX; 
						  $$ = $1;
						}
	| function_modbase DOTDOTDOT_OP ')'	{$1[globalTypeIndex++] = DOTDOTDOT_BASE; $1[globalTypeIndex++] = CLOSE_FUNCTION_POSTFIX; $$ = $1; }
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
type_name:
	  base_type_postfix	{ memcpy(globalIPT, $1, 32); $$ = $1; }
	| function_modifier	{ memcpy(globalIPT, $1, 32); $$ = $1; }
	| pointer_modifier	{ memcpy(globalIPT, $1, 32); $$ = $1; }
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

pushTypeIndexR: %empty {pushTypeIndex();} 
	;

parameter_list:
	  type_name						{ $$ = $1; }
	| type_name IDENT					{ struct symbolEntry* temp = malloc(sizeof(struct symbolEntry));
							    	  memcpy(temp->modString, $1, 32);
							    	  memcpy(temp->name, $2, 128);
							    	  registerSymbol(temp);
								  $$ = $1;
 								}
	| parameter_list ',' pushTypeIndexR type_name		{ copyAndPopTypeIndex($1, $4); free($4); $$ = $1; }
	| parameter_list ',' pushTypeIndexR type_name IDENT	{ struct symbolEntry* temp = malloc(sizeof(struct symbolEntry));
							    	  memcpy(temp->modString, $4, 32);
							    	  memcpy(temp->name, $4, 128);
							    	  registerSymbol(temp);
								  copyAndPopTypeIndex($1, $4);
								  free($4);
								  $$ = $1;
								}
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
openScopeHelper: '(' {openScope(); } 
	;

statement:
	  ';'						  	{ $$ = NULL; }
	| expression ';'				  	{ $$ = $1; }
	| declaration					  	{ $$ = $1; }
	| scope '}'					  	{ closeScope(); $$ = $1; }


	| IDENT '=' expression ';'	  
								{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
								  temp->type = EQU_TYPE;
								  memcpy(temp->modString, $3->modString, 32);
								  temp->childCount = 2;
								  temp->children[0] = registerNode(registerSymbol(createRef($1)));
								  temp->children[1] = $6;
								  compareTypes($3, $6);
								  $$ = registerNode(temp);
							       	}
	| '[' expression ']' '=' expression ';'	  
								{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
								  temp->type = EQU_TYPE;
								  memcpy(temp->modString, $3->modString, 32);
								  temp->childCount = 2;
								  temp->children[0] = $3;
								  temp->children[1] = $6;
								  compareTypes($3, $6);
								  $$ = registerNode(temp);
							       	}


	| IF_OP openScopeHelper expression ')' DO_OP statement	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
								  temp->type = IF_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 2;
								  temp->children[0] = $3;
								  temp->children[1] = $6;
								  closeScope();
								  $$ = registerNode(temp);
								}

	| IF_OP openScopeHelper expression ')' THEN_OP statement ELSE_OP statement
								{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 3);
								  temp->type = IF_ELSE_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 3;
								  temp->children[0] = $3;
								  temp->children[1] = $6;
								  temp->children[2] = $8;
								  closeScope();
								  $$ = registerNode(temp);
								}
	| WHILE_OP openScopeHelper expression ')' statement  { struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
								  temp->type = WHILE_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 2;
								  temp->children[0] = $3;
								  temp->children[1] = $5;
								  closeScope();
								  $$ = registerNode(temp);
								}

	| DO_OP statement WHILE_OP openScopeHelper expression ')' ';'	
								{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
								  temp->type = IF_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 2;
								  temp->children[0] = $2;
								  temp->children[1] = $5;
								  closeScope();
								  $$ = registerNode(temp);
								}
	| BREAK_OP ';'						{ struct genericNode* temp = malloc(sizeof(struct genericNode));
								  temp->type = BREAK_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 0;
								  $$ = registerNode(temp);
								}
	| CONTINUE_OP ';'					{ struct genericNode* temp = malloc(sizeof(struct genericNode));
								  temp->type = CONTINUE_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 0;
								  $$ = registerNode(temp);
								}

	| SWITCH_OP openScopeHelper expression ')' statement { struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
								  temp->type = SWITCH_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 2;
								  temp->children[0] = $3;
								  temp->children[1] = $5;
								  closeScope();
								  $$ = registerNode(temp);
								}
	| CASE_OP constant ':' statement			{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
								  temp->type = CASE_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 2;
								  temp->children[0] = $2;
								  temp->children[1] = $4;
								  $$ = registerNode(temp);
								}
	| DEFAULT_OP ':' statement				{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 1);
								  temp->type = DEFAULT_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 1;
								  temp->children[0] = $3;
								  $$ = registerNode(temp);
								}
	| RETURN_OP ';'						{ struct genericNode* temp = malloc(sizeof(struct genericNode));
								  temp->type = RETURN_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 0;
								  $$ = registerNode(temp);
								}
	| RETURN_OP expression ';'				{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 1);
								  temp->type = RETURN_EXP_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 1;
								  temp->children[0] = $2;
								  $$ = registerNode(temp);
								}
	| IDENT ':' statement					{ $$ = appendAChild(registerNode(registerSymbol(createLabel($1))), $3); }
	| GOTO_OP IDENT ';'					{ $$ = registerNode(createLabelJump($2)); }

	| FOR_OP openScopeHelper statement            ';' statement ')' statement
								{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 4);
								  temp->type = FOR_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 4;
								  temp->children[0] = $3;
								  temp->children[1] = NULL;
								  temp->children[2] = $5;
								  temp->children[3] = $7;
								  $$ = registerNode(temp);
								}

	| FOR_OP openScopeHelper statement expression ';' statement ')' statement
								{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 4);
								  temp->type = FOR_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 4;
								  temp->children[0] = $3;
								  temp->children[1] = $4;
								  temp->children[2] = $6;
								  temp->children[3] = $8;
								  $$ = registerNode(temp);
								}
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
scope:
	  '{' statement			{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
					  temp->type = SCOPE_TYPE;
				 	  memset(temp->modString, NONE_MOD, 32);
					  temp->childCount = 1;
					  temp->children[0] = $2;
					  openScope();
					  $$ = registerNode(temp);
					}
	| scope statement		{ $$ = appendAChild($1, $2); }
	;


%%

#include "lex.yy.c"

void yyerror(char const *s){
	fprintf(stderr, "%s\n", s);
}

int main(int argc, char *argv[]){
	inFile = stdin;
	outFile = stdout;

	yyin = stdin;
	GLOBAL_LINE_NUMBER = 0;
	
	initNodes();

	yyparse();

	//while(/*files not empty*/){
		//look for delimiters
	//	char *cut;
	//
	//	struct yy_buffer_state *selection = yy_scan_string(cut);
	//	yyparse();
	//	yy_delete_buffer(selection);
	//}
	

	return 0;
}


