%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>

unsigned long GLOBAL_LINE_NUMBER;
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
%token ARROW_OP
%token ANDAND_OP
%token OROR_OP
%token SIZEOF_OP
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
%token STRUCT_OP

%token CONST_OP
%token SHARED_OP

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


%token INCLUDE_OP
%token IFDEF_OP
%token IFNDEF_OP
%token THEN_OP
%token DEFINE_OP
%token UNDEF_OP
%token NOTHING_OP


%nterm <struct genericNode*> constant
%nterm <struct genericNode*> initial_expression
%nterm <struct genericNode*> initializer
%nterm <struct genericNode*> initializer_list
%nterm <struct genericNode*> iota_vector
%nterm <struct genericNode*> postfix_operation
%nterm <struct genericNode*> permute_list
%nterm <struct genericNode*> block_permute
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
%nterm <struct genericNode*> mesh_operation
%nterm <struct genericNode*> logical_and_operation
%nterm <struct genericNode*> logical_or_operation
%nterm <struct genericNode*> ternary_operation
%nterm <struct genericNode*> assignment_operation
%nterm <struct genericNode*> comma_operation
%nterm <struct genericNode*> expression
%nterm <struct genericNode*> declaration
%nterm <struct genericNode*> declaration_init_list
%nterm <char> hint_modifier
%nterm <char*> base_type
%nterm <char*> base_type_postfix
%nterm <char*> array_modifier
%nterm <char*> pointer_modifier
%nterm <char*> function_modifier
%nterm <char*> type_name
%nterm <char*> parameter_list
%nterm <struct genericNode*> statement
%nterm <struct genericNode*> scope

%%

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
input:
	  %empty
	| input initial_expression
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
constant:
	  DECIMAL			{ $$ = registerNode(registerSymbol(createImmediate($1, 0)));  }
	| FLOAT				{ $$ = registerNode(registerSymbol(createImmediate($1, 3)));  }
	| BINARY			{ $$ = registerNode(registerSymbol(createImmediate($1, 2)));  }
	| HEX				{ $$ = registerNode(registerSymbol(createImmediate($1, 1)));  }
	| SIZEOF_OP '(' type_name ')'	{ $$ = registerNode(registerSymbol(createSizeOf($3, 0)));     }
	| SIZEOF_OP '(' IDENT ')'	{ $$ = registerNode(registerSymbol(createSizeOf($3, 1)));     }
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
initial_expression: 
	  IDENT				{ $$ = registerNode(registerSymbol(createRef($1)));          }
	| constant			{ $$ = $1;                                                   }
	| STRING_LIT			{ $$ = registerNode(registerSymbol(createImmediate($1, 4))); }
	| DOTDOTDOT_OP			{ struct genericNode* temp = malloc(sizeof(struct genericNode)); 
					  temp->childCount = 0; 
					  temp->type = DOTDOTDOT_TYPE;			// '...' is a void pointer to stack base + offset
					  memset(temp->modString, NONE_MOD, 32);
					  temp->modString[0] = VOID_BASE;
					  temp->modString[1] = POINTER_POSTFIX;	//create a void pointer
					  $$ = registerNode(); 
					}
	| '(' expression ')'		{ $$ = $2; }
	| initializer_list		{ $$ = $1; }
	| '&' IDENT			{ $$ = registerNode(registerSymbol(createImmediate($2, 5))); }
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
initializer:
	  constant			{ $$ = $1; }
	| '{' initializer_list '}'	{ $$ = $2; }
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
initializer_list:
	  initializer				{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
						  temp->type = INIT_LIST_TYPE;
						  memcpy(temp->modString, $1->modString, 32);
						  temp->childCount = 1;
						  temp->children[0] = $1;
						  $$ = registerNode(temp);	//create the center node of the list
						}
	| initializer_list ',' initializer	{ compareTypes($1, $3);	     //make sure it matches the initList type
						  $$ = appendAChild($1, $3); //add another branch to the head
						}
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
postfix_operation:
	  initial_expression				{ $$ = $1; }
	| postfix_operation '[' expression ']'		{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*)*2);
							  temp->type = INDEX_TYPE;
							  memcpy(temp->modString, $1->modString, 32);
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $3;
							  enforceScalerInts($3);
							  enforcePointer($1);
							  $$ = registerNode(fetchMod(temp));
							}			
	| '&' postfix_operation '[' expression ']'	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*)*2);
							  temp->type = AMP_INDEX_TYPE;
							  memcpy(temp->modString, $2->modString, 32);
							  temp->childCount = 2;
							  temp->children[0] = $2;
							  temp->children[1] = $4;
							  enforceScalerInts($4);
							  enforcePointer($2);
							  $$ = registerNode(temp);
							}
	| postfix_operation '(' ')'			{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
							  temp->type = CALL_TYPE;
							  memcpy(temp->modString, $1->modString, 32);
							  temp->childCount = 1;
							  temp->children[0] = $1;
							  requireFunctions($1);
							  $$ = registerNode(fetchFunc(temp));
							} 
	| postfix_operation '(' argument_list ')'	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*)*2);
							  temp->type = CALL_PARAM_TYPE;
							  memcpy(temp->modString, $1->modString, 32);
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $3;
							  requireFunctions($1);
							  checkArgumentTypes($1, $3);
							  $$ = registerNode(fetchFunc(temp));
							}
	| postfix_operation '<' permute_list '>' 	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = PERMUTE_TYPE;
							  memcpy(temp->modString, $1->modString, 32);
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $3;
							  checkLengths($1, $3);
							  $$ = registerNode(temp);
							}
	| postfix_operation '<' constant '>'  		{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = VEC_INDEX_TYPE;
							  memcpy(temp->modString, $1->modString, 32);
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $3;
							  enforceScalerInts($3);
							  $$ = registerNode(undoVector(temp));
						        }
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
permute_list:
	  constant				{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
						  temp->type = PERMUTE_LIST_TYPE;
						  memcpy(temp->modString, $1->modString, 32);
						  temp->childCount = 1;
						  temp->children[0] = $1;
						  enforceScalerInts($1);	//no indexing with floats
						  $$ = registerNode(temp);	//create head, like initList
				 	      	}
	| permute_list ',' initial_expression	{ enforceScalerInts($3);
						  $$ = appendAChild($1, $3);	//add the constant to the list
				 	      	}
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
argument_list:
	  assignment_operation			 { struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
						   temp->type = ARGUMENT_LIST_TYPE;
						   memset(temp->modString, NONE_MOD, 32);
						   temp->childCount = 1;
						   temp->children[0] = $1;
						   $$ = registerNode(temp);	//create the head, like above
				 	       	 }
	| argument_list ',' assignment_operation { $$ = appendAChild($1, $3); /* add a argument to the list */ }
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
	| '*' prefix_operation			{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
						  temp->type = DEREF_TYPE;
						  memcpy(temp->modString, $2->modString, 32);
						  temp->childCount = 1;
						  temp->children[0] = $2;
						  enforcePointer($2);			//require a pointer of array here
						  $$ = registerNode(fetchMod(temp));	//undo the pointer/array
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
	| '(' type_name ')' prefix_operation    { struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
						  temp->type = PUNN_TYPE;
						  memcpy(temp->modString, $2->modString, 32);
						  temp->childCount = 1;
						  temp->children[0] = $4;
						  $$ = registerNode(temp);	//for both of these, it's fine not to track the type_name; we don't need it
			 		       	}
	| type_name '(' prefix_operation ')'    { struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
						  temp->type = CONV_TYPE;
						  memcpy(temp->modString, $2->modString, 32);
						  temp->childCount = 1;
						  temp->children[0] = $4;
						  $$ = registerNode(temp);
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
							  temp->modString[0] = BYTE_TYPE;
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $3;
							  compareTypes($1, $3);
							  $$ = registerNode(temp);
			 			       	}

	| relation_operation '>' shift_operation	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = GT_TYPE;
							  memset(temp->modString, NONE_MOD, 32);
							  temp->modString[0] = BYTE_TYPE;
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $3;
							  compareTypes($1, $3);
							  $$ = registerNode(temp);
			 			       	}

	| relation_operation LTEQU_OP shift_operation	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = LT_EQU_TYPE;
							  memset(temp->modString, NONE_MOD, 32);
							  temp->modString[0] = BYTE_TYPE;
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $3;
							  compareTypes($1, $3);
							  $$ = registerNode(temp);
			 			       	}

	| relation_operation GTEQU_OP shift_operation	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = GT_EQU_TYPE;
							  memset(temp->modString, NONE_MOD, 32);
							  temp->modString[0] = BYTE_TYPE;
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $3;
							  compareTypes($1, $3);
							  $$ = registerNode(temp);
			 			       	}

	| relation_operation '<' shift_operation ':' expression	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 3);
								  temp->type = LT_TERN_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
							  	  temp->modString[0] = BYTE_TYPE;
								  temp->childCount = 3;
								  temp->children[0] = $1;
								  temp->children[1] = $3;
								  temp->children[2] = $5;
								  compareTypes($1, $3);
								  compareTypes($3, $5);
								  $$ = registerNode(temp);
			 				       	}

	| relation_operation '>' shift_operation ':' expression	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 3);
								  temp->type = GT_TERN_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
							 	  temp->modString[0] = BYTE_TYPE;
								  temp->childCount = 3;
								  temp->children[0] = $1;
								  temp->children[1] = $3;
								  temp->children[2] = $5;
								  compareTypes($1, $3);
								  compareTypes($3, $5);
								  $$ = registerNode(temp);
			 				       	}

	| relation_operation LTEQU_OP shift_operation ':' expression	{ struct genericNode* temp = malloc(sizeof(struct genericNode)+sizeof(struct genericNode*)*3);
									  temp->type = LT_EQU_TERN_TYPE;
							 		  memset(temp->modString, NONE_MOD, 32);
							 		  temp->modString[0] = BYTE_TYPE;
									  temp->childCount = 3;
									  temp->children[0] = $1;
									  temp->children[1] = $3;
									  temp->children[2] = $5;
									  compareTypes($1, $3);
									  compareTypes($3, $5);
									  $$ = registerNode(temp);
			 				   	    	}

	| relation_operation GTEQU_OP shift_operation ':' expression	{ struct genericNode* temp = malloc(sizeof(struct genericNode)+sizeof(struct genericNode*)*3);
									  temp->type = GT_EQU_TERN_TYPE;
									  memset(temp->modString, NONE_MOD, 32);
									  temp->modString[0] = BYTE_TYPE;
									  temp->childCount = 3;
									  temp->children[0] = $1;
									  temp->children[1] = $3;
									  temp->children[2] = $5;
									  compareTypes($1, $3);
									  compareTypes($3, $5);
									  $$ = registerNode(temp);
			 					       	}
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
equality_operation:
	  relation_operation				{ $$ = $1; }

	| equality_operation EQUEQU_OP relation_operation	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
								  temp->type = EQU_EQU_TYPE;
								  memset(temp->modString, NONE_MOD, 32);
								  temp->modString[0] = BYTE_TYPE;
								  temp->childCount = 2;
								  temp->children[0] = $1;
								  temp->children[1] = $3;
								  compareTypes($1, $3);
								  $$ = registerNode(temp);
			 				       	}

	| equality_operation NOTEQU_OP relation_operation	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
								  temp->type = NOT_EQU_TYPE;
								  memset(temp->modString, NONE_MOD, 32);
								  temp->modString[0] = BYTE_TYPE;
								  temp->childCount = 2;
								  temp->children[0] = $1;
								  temp->children[1] = $3;
								  compareTypes($1, $3);
								  $$ = registerNode(temp);
			 				       	}

	| equality_operation EQUEQU_OP relation_operation ':' expression { struct genericNode* temp = malloc(sizeof(struct genericNode)+sizeof(struct genericNode*)*3);
									   temp->type = EQU_EQU_TERN_TYPE;
							 		   memset(temp->modString, NONE_MOD, 32);
									   temp->modString[0] = BYTE_TYPE;
									   temp->childCount = 3;
									   temp->children[0] = $1;
									   temp->children[1] = $3;
									   temp->children[2] = $5;
									   compareTypes($1, $3);
									   compareTypes($3, $5);
									   $$ = registerNode(temp);
			 					       	 }

	| equality_operation NOTEQU_OP relation_operation ':' expression { struct genericNode* temp = malloc(sizeof(struct genericNode)+sizeof(struct genericNode*)*3);
									   temp->type = NOT_EQU_TERN_TYPE;
									   memset(temp->modString, NONE_MOD, 32);
									   temp->modString[0] = BYTE_TYPE;
									   temp->childCount = 3;
									   temp->children[0] = $1;
									   temp->children[1] = $3;
									   temp->children[2] = $5;
									   compareTypes($1, $3);
									   compareTypes($3, $5);
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

	| logical_and_operation ANDAND_OP mesh_operation  { struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							    temp->type = LOGIC_AND_TYPE;
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
logical_or_operation:
	  logical_and_operation				{ $$ = $1; }

	| logical_or_operation OROR_OP logical_and_operation	  { struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
								    temp->type = LOGIC_OR_TYPE;
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
assignment_operation:
	  ternary_operation				{ $$ = $1; }

	| {poisonRefBool = 1;} prefix_operation {poisonRefBool = 0;} '=' assignment_operation	  
							  { struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							    temp->type = EQU_TYPE;
							    memcpy(temp->modString, $1->modString, 32);
							    temp->childCount = 2;
							    temp->children[0] = $1;
							    temp->children[1] = $3;
							    compareTypes($1, $3);
							    $$ = registerNode(temp);
						       	  }
	;





//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
comma_operation:
	  assignment_operation				  { $$ = $1; }

	| comma_operation ',' assignment_operation  	  { struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							    temp->type = COMMA_TYPE;
							    memcpy(temp->modString, $3->modString, 32);
							    temp->childCount = 2;
							    temp->children[0] = $1;
							    temp->children[1] = $3;
							    $$ = registerNode(temp);
						       	  }
	;




//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
expression:
	  comma_operation				{ $$ = $1; }
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
declaration:
	  hint_modifier type_name { memcpy(&globalIPT[1], $2, 31); globalIPT[0] = $1; excludeFunctionsType($2); } declaration_init_list ';' { $$ = $3; }


	| hint_modifier type_name IDENT scope		    { struct genericNode* temp = malloc(sizeof(struct symbolNode));
							      memcpy(&temp->modString[1], $2, 31);
							      modString[0] = $1
							      memcpy(temp->name, $3);
							      temp->innerScope = $4;
							      temp->size = globalRunningSize;
							      requireFunctionsType($2);
							      enforceZeroScope();
							      closeFalseScope();
							      $$ = registerNodeFunction(registerSymbol(temp));
							    }

	| type_name { memcpy(globalIPT, $1, 32); excludeFunctionsType($1); } declaration_init_list ';' { $$ = $3; }

	| type_name IDENT scope				    { struct genericNode* temp = malloc(sizeof(struct symbolNode));
							      memcpy(temp->modString, $1, 32);
							      memcpy(temp->name, $2, 128);
							      temp->innerScope = $3;
							      temp->size = globalRunningSize;
							      requireFunctionsType($1);
							      enforceZeroScope();
							      closeFalseScope();			//close the false scope created by function mod
							      $$ = registerNodeFunction(registerSymbol(temp));
							    }

	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
declaration_init_list:
	  IDENT '=' expression				{ createIPTSymbol($1); //create a symbol using the inprocess type
							  struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = EQU_TYPE;
							  memcpy(temp->modString, $3->modString, 32);
							  temp->childCount = 2;
							  temp->children[0] = registerNode(registerSymbol(createRef($1)));
							  temp->children[1] = $3;
							  compareTypes($1, $3);
							  struct genericNode* temp2 = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
							  temp2->type = DEC_LIST_TYPE;	//create the dec list header
							  memset(temp->modString, NONE_MOD, 32);
							  temp2->childCount = 1;
							  temp2->children[0] = temp;
							  $$ = registerNode(temp2);	//create the head
						        }
	| IDENT						{ createIPTSymbol($1);	//in process symbol creation
							  struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
							  temp->type = DEC_LIST_TYPE;
							  memset(temp->modString, NONE_MOD, 32);
							  temp->childCount = 1;
							  temp->children[0] = registerNode(registerSymbol(createRef($1)));
							  $$ = registerNode(temp);	//create the head, like above
				 		       	}
	| declaration_init_list ',' IDENT '=' expression{ createIPTSymbol($1); //create a symbol using the inprocess type
							  struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = EQU_TYPE;
							  memcpy(temp->modString, $5->modString, 32);
							  temp->childCount = 2;
							  temp->children[0] = registerNode(registerSymbol(createRef($3)));
							  temp->children[1] = $5;
							  compareTypes($3, $5);
							  $$ = appendAChild($1, temp);	//append equals
						        }
	| declaration_init_list ',' IDENT		{ createIPTSymbol($3); $$ = appendAChild($1, registerNode(registerSymbol(createRef($3)))); }	
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
hint_modifier:
	  EXTERN_OP	{ $$ = EXTERN_HINT; }	
	| GLOBAL_OP	{ $$ = GLOBAL_HINT; }
	| CONST_OP	{ $$ = CONST_HINT;  }
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
base_type:
	  VOID_OP		{ char *temp = malloc(32); globalTypeIndex = 1; temp[0] = VOID_BASE; $$ = temp; }
	| BYTE_OP		{ char *temp = malloc(32); globalTypeIndex = 1; temp[0] = BYTE_BASE; $$ = temp; }
	| WORD_OP		{ char *temp = malloc(32); globalTypeIndex = 1; temp[0] = WORD_BASE; $$ = temp; }
	| LONG_OP		{ char *temp = malloc(32); globalTypeIndex = 1; temp[0] = LONG_BASE; $$ = temp; }
	| QUAD_OP		{ char *temp = malloc(32); globalTypeIndex = 1; temp[0] = QUAD_BASE; $$ = temp; }
	| SINGLE_OP		{ char *temp = malloc(32); globalTypeIndex = 1; temp[0] = SINGLE_BASE; $$ = temp; }
	| DOUBLE_OP		{ char *temp = malloc(32); globalTypeIndex = 1; temp[0] = DOUBLE_BASE; $$ = temp; }
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
base_type_postfix:
	  base_type				{ $$ = $1; }
	| base_type '<' constant '>'		{ $1[2] = $1[0]; 
						  $1[0] = VECTOR_MOD; 
						  enforceScalerInts($3);
						  long temp;
						  sscanf($3->children[0]->constValue, "%ld", &temp);
						  if(temp > 128 || temp < 1){
							fprintf(stderr, "ERR: vector constant too large (not 1 to 128), %ld", temp);
							exit(1);
						  }
						  $1[1] = (char)(-temp);
						  globalTypeIndex += 2;
						  $$ = $1;
						}
	| SHARED_OP base_type			{ $1[1] = $1[0]; $1[0] = SHARED_MOD; globalTypeIndex += 1; $$ = $1; }
	| SHARED_OP base_type '<' constant '>'	{ $2[3] = $2[0]; 
						  $2[0] = SHARED_MOD; 
						  $2[1] = VECTOR_MOD; 
						  enforceScalerInts($4);
						  long temp;
						  sscanf($4->children[0]->constValue, "%ld", &temp);
						  if(temp > 128 || temp < 1){
							fprintf(stderr, "ERR: vector constant too large (not 1 to 128), %ld", temp);
							exit(1);
						  }
						  globalTypeIndex += 3;
						  $2[2] = (char)(-temp);
						  $$ = $1;
						}
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
array_modifier:
	  base_type_postfix '[' constant ']'	{ $1[globalTypeIndex] = ARRAY_POSTFIX; 
						  globalTypeIndex++;
						  excludeFloats($3);
						  long temp;
						  sscanf($3->children[0]->constValue, "%ld", &temp);
						  globalRunningSize *= (char)(-temp);
						  $$ = $1;
						}
	| pointer_modifier '[' constant ']'	{ $1[globalTypeIndex] = ARRAY_POSTFIX; 
						  globalTypeIndex++;
						  excludeFloats($3);
						  long temp;
						  sscanf($3->children[0]->constValue, "%ld", &temp);
						  globalRunningSize *= (char)(-temp);
						  $$ = $1;
						}
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
pointer_modifier:
	  base_type_postfix '*'			{ $1[globalTypeIndex++] = POINTER_POSTFIX; $$ = $1; }
	| pointer_modifier  '*'			{ $1[globalTypeIndex++] = POINTER_POSTFIX; $$ = $1; }
	| array_modifier    '*'			{ $1[globalTypeIndex++] = POINTER_POSTFIX; $$ = $1; }
	| function_modifier '*'			{ $1[globalTypeIndex++] = POINTER_POSTFIX; $$ = $1; }
	| base_type_postfix SHARED_OP '*'	{ $1[globalTypeIndex++] = SHARED_MOD; $1[globalTypeIndex++] = POINTER_POSTFIX; $$ = $1; }
	| pointer_modifier  SHARED_OP '*'	{ $1[globalTypeIndex++] = SHARED_MOD; $1[globalTypeIndex++] = POINTER_POSTFIX; $$ = $1; }
	| array_modifier    SHARED_OP '*'	{ $1[globalTypeIndex++] = SHARED_MOD; $1[globalTypeIndex++] = POINTER_POSTFIX; $$ = $1; }
	| function_modifier SHARED_OP '*'	{ $1[globalTypeIndex++] = SHARED_MOD; $1[globalTypeIndex++] = POINTER_POSTFIX; $$ = $1; }
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
function_modifier:
	  base_type_postfix '(' { openFalseScope(); $1[globalTypeIndex++] = FUNCTION_POSTFIX; pushTypeIndex(); } parameter_list ')'			
				{ copyAndPopTypeIndex($1, $3); free($3); $1[globalTypeIndex++] = CLOSE_FUNCTION_POSTFIX; }

	| pointer_modifier  '(' { openFalseScope(); $1[globalTypeIndex++] = FUNCTION_POSTFIX; pushTypeIndex(); } parameter_list ')'			
				{ copyAndPopTypeIndex($1, $3); free($3); $1[globalTypeIndex++] = CLOSE_FUNCTION_POSTFIX; }

	| base_type_postfix '(' { openFalseScope(); $1[globalTypeIndex++] = FUNCTION_POSTFIX; pushTypeIndex(); } parameter_list ',' DOTDOTDOT_OP ')'	
				{ copyAndPopTypeIndex($1, $3); free($3); $1[globalTypeIndex++] = DOTDOTDOT_BASE; $1[globalTypeIndex++] = CLOSE_FUNCTION_POSTFIX; }

	| pointer_modifier  '(' { openFalseScope(); $1[globalTypeIndex++] = FUNCTION_POSTFIX; pushTypeIndex(); } parameter_list ',' DOTDOTDOT_OP ')'	
				{ copyAndPopTypeIndex($1, $3); free($3); $1[globalTypeIndex++] = DOTDOTDOT_BASE; $1[globalTypeIndex++] = CLOSE_FUNCTION_POSTFIX; }

	| base_type_postfix '(' DOTDOTDOT_OP ')'	{$1[globalTypeIndex++] = CLOSE_FUNCTION_POSTFIX; }

	| pointer_modifier  '(' DOTDOTDOT_OP ')'	{$1[globalTypeIndex++] = CLOSE_FUNCTION_POSTFIX; }
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
type_name:
	  base_type_postfix	{ $$ = $1; }
	| function_modifier	{ $$ = $1; }
	| array_modifier	{ $$ = $1; }
	| pointer_modifier	{ $$ = $1; }
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
parameter_list:
	  type_name						{ $$ = $1; }
	| type_name IDENT					{ struct genericNode* temp = malloc(sizeof(struct symbolNode));
							    	  memcpy(temp->modString, $1, 32);
							    	  memcpy(temp->name, $2, 128);
							    	  temp->size = globalRunningSize;
							    	  registerSymbol(temp);
								  $$ = $1;
 								}
	| parameter_list ',' {pushTypeIndex();} type_name	{ copyAndPopTypeIndex($1, $3); free($3); $$ = $1; }
	| parameter_list ',' {pushTypeIndex();} type_name IDENT	{ struct genericNode* temp = malloc(sizeof(struct symbolNode));
							    	  memcpy(temp->modString, $3, 32);
							    	  memcpy(temp->name, $4, 128);
							    	  temp->size = globalRunningSize;
							    	  registerSymbol(temp);
								  copyAndPopTypeIndex($1, $3);
								  free($3);
								  $$ = $1;
								}
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
statement:
	  ';'						{ $$ = NULL; }
	| expression ';'				{ $$ = $1; }
	| declaration					{ $$ = $1; }
	| scope '}'					{ closeScope(); $$ = $1; }
	| IF_OP '(' {openScope(); } expression ')' statement	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
								  temp->type = IF_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 2;
								  temp->children[0] = $3;
								  temp->children[1] = $5;
								  closeScope();
								  $$ = registerNode(temp);
								}

	| IF_OP '(' {openScope();} expression ')' statement ELSE_OP statement
								{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 3);
								  temp->type = IF_ELSE_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 3;
								  temp->children[0] = $3;
								  temp->children[1] = $5;
								  temp->childern[2] = $7;
								  closeScope();
								  $$ = registerNode(temp);
								}
	| WHILE_OP '(' {openScope();} expression ')' statement  { struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
								  temp->type = WHILE_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 2;
								  temp->children[0] = $3;
								  temp->children[1] = $5;
								  closeScope();
								  $$ = registerNode(temp);
								}

	| DO_OP statement WHILE_OP '(' {openScope();} expression ')' ';'	
								{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
								  temp->type = IF_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 2;
								  temp->children[0] = $3;
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

	| SWITCH_OP '(' {openScope();} expression ')' statement	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
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

	| FOR_OP '(' {openScope();}            ';'            ';'            ')' statement	
								{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 4);
								  temp->type = FOR_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 4;
								  temp->children[0] = NULL;
								  temp->children[1] = NULL;
								  temp->children[2] = NULL;
								  temp->children[3] = $6:
								  $$ = registerNode(temp);
								}

	| FOR_OP '(' {openScope();}            ';'            ';' expression ')' statement
								{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 4);
								  temp->type = FOR_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 4;
								  temp->children[0] = NULL;
								  temp->children[1] = NULL;
								  temp->children[2] = $5;
								  temp->children[3] = $7:
								  $$ = registerNode(temp);
								}

	| FOR_OP '(' {openScope();}            ';' expression ';'            ')' statement
								{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 4);
								  temp->type = FOR_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 4;
								  temp->children[0] = NULL;
								  temp->children[1] = $4;
								  temp->children[2] = NULL;
								  temp->children[3] = $7:
								  $$ = registerNode(temp);
								}

	| FOR_OP '(' {openScope();}            ';' expression ';' expression ')' statement
								{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 4);
								  temp->type = FOR_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 4;
								  temp->children[0] = NULL;
								  temp->children[1] = $4;
								  temp->children[2] = $6;
								  temp->children[3] = $8:
								  $$ = registerNode(temp);
								}

	| FOR_OP '(' {openScope();} expression ';'            ';'            ')' statement
								{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 4);
								  temp->type = FOR_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 4;
								  temp->children[0] = $3;
								  temp->children[1] = NULL;
								  temp->children[2] = NULL;
								  temp->children[3] = $7:
								  $$ = registerNode(temp);
								}

	| FOR_OP '(' {openScope();} expression ';'            ';' expression ')' statement
								{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 4);
								  temp->type = FOR_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 4;
								  temp->children[0] = $3;
								  temp->children[1] = NULL;
								  temp->children[2] = $6;
								  temp->children[3] = $8:
								  $$ = registerNode(temp);
								}

	| FOR_OP '(' {openScope();} expression ';' expression ';'            ')' statement
								{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 4);
								  temp->type = FOR_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 4;
								  temp->children[0] = $3;
								  temp->children[1] = $5;
								  temp->children[2] = NULL;
								  temp->children[3] = $8:
								  $$ = registerNode(temp);
								}

	| FOR_OP '(' {openScope();} expression ';' expression ';' expression ')' statement
								{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 4);
								  temp->type = FOR_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 4;
								  temp->children[0] = $3;
								  temp->children[1] = $5;
								  temp->children[2] = $7;
								  temp->children[3] = $9:
								  $$ = registerNode(temp);
								}

	| FOR_OP '(' {openScope();} declaration            ';' ')' statement
								{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 4);
								  temp->type = FOR_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 4;
								  temp->children[0] = $3;
								  temp->children[1] = NULL;
								  temp->children[2] = NULL;
								  temp->children[3] = $6:
								  $$ = registerNode(temp);
								}

	| FOR_OP '(' {openScope();} declaration expression ';' ')' statement
								{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 4);
								  temp->type = FOR_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 4;
								  temp->children[0] = $3;
								  temp->children[1] = $4;
								  temp->children[2] = NULL;
								  temp->children[3] = $7:
								  $$ = registerNode(temp);
								}

	| FOR_OP '(' {openScope();} declaration            ';' expression ')' statement
								{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 4);
								  temp->type = FOR_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 4;
								  temp->children[0] = $3;
								  temp->children[1] = NULL;
								  temp->children[2] = $5;
								  temp->children[3] = $7:
								  $$ = registerNode(temp);
								}

	| FOR_OP '(' {openScope();} declaration expression ';' expression ')' statement
								{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 4);
								  temp->type = FOR_TYPE;
							 	  memset(temp->modString, NONE_MOD, 32);
								  temp->childCount = 4;
								  temp->children[0] = $3;
								  temp->children[1] = $4;
								  temp->children[2] = $6;
								  temp->children[3] = $8:
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


