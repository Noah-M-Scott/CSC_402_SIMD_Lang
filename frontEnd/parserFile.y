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
%token GTEQU_OP
%token LTEQU_OP
%token NOTEQU_OP
%token ARROW_OP
%token ANDAND_OP
%token OROR_OP
%token SIZEOF_OP
%token COLCOL_OP
%token DOTDOTDOT_OP

%token ADD_BAR_OP
%token SUB_BAR_OP

%token EXTERN_OP
%token GLOBAL_OP
%token VOID_OP
%token BYTE_OP
%token UBYTE_OP
%token WORD_OP
%token UWORD_OP
%token LONG_OP
%token ULONG_OP
%token QUAD_OP
%token UQUAD_OP
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
%nterm <struct genericNode*> hint_modifier
%nterm <struct genericNode*> base_type
%nterm <struct genericNode*> base_type_postfix
%nterm <struct genericNode*> array_modifier
%nterm <struct genericNode*> pointer_modifier
%nterm <struct genericNode*> function_modifier
%nterm <struct genericNode*> type_name
%nterm <struct genericNode*> parameter_list
%nterm <struct genericNode*> statement
%nterm <struct genericNode*> scope


%%

input:
	  %empty
	| input initial_expression
	;

constant:
	  DECIMAL	{ $$ = registerNode(registerSymbol(createImmediate($1, 0)));  }
	| FLOAT		{ $$ = registerNode(registerSymbol(createImmediate($1, 3)));  }
	| BINARY	{ $$ = registerNode(registerSymbol(createImmediate($1, 2)));  }
	| HEX		{ $$ = registerNode(registerSymbol(createImmediate($1, 1)));  }
	;

initial_expression: 
	  IDENT			{ $$ = registerNode(registerSymbol(createRef($1))); }
	| constant		{ $$ = $1; }
	| STRING_LIT		{ $$ = registerNode(registerSymbol(createImmediate($1, 4))); }
	| DOTDOTDOT_OP		{ struct genericNode* temp = malloc(sizeof(struct genericNode)); 
				  temp->childCount = 0; 
				  temp->nodeSize = sizeof(struct genericNode); 
				  temp->type = DOTDOT_TYPE;
				  memset(temp->modString, NONE_MOD, 32);
				  $$ = registerNode(); 
				}
	
	| '(' expression ')'	{ $$ = $2; }
	| initializer_list	{ $$ = $1; }
	| '&' IDENT		{ $$ = registerNode(registerSymbol(createImmediate($1, 5))); }
	| SIZEOF_OP '(' type_name ')'	{ $$ = registerNode(registerSymbol(createSizeOf($1, 0))); }
	| SIZEOF_OP '(' IDENT ')'	{ $$ = registerNode(registerSymbol(createSizeOf($1, 1))); }
	;



initializer:
	  constant			{ $$ = $1; }
	| '{' initializer_list '}'	{ $$ = $2; }
	;


initializer_list:
	  initializer	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
			  temp->type = INIT_LIST_TYPE;
			  memcpy(temp->modString, $1->modString, 32);
			  temp->childCount = 1;
			  temp->children[0] = $1;
			  $$ = registerNode(temp);
			}

	| initializer_list ',' initializer	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
			  			  temp->type = INIT_LIST_TYPE;
						  memcpy(temp->modString, $3->modString, 32);
						  compareTypes($1, $3);
						  temp->childCount = 1;
						  temp->children[0] = $3;
						  $$ = appendAChild($1, registerNode(temp));
						}
	;



postfix_operation:
	  initial_expression			{ $$ = $1; }
	| postfix_operation '[' expression ']'	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
						  temp->type = INDEX_TYPE;
						  memcpy(temp->modString, $1->modString, 32);
						  temp->childCount = 1;
						  temp->children[0] = $3;
						  excludeFloats($3);
						  $$ = registerNode(fetchMod(temp));
						}	
							
	| '&' postfix_operation '[' expression ']'	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
							  temp->type = INDEX_TYPE;
							  memcpy(temp->modString, $2->modString, 32);
							  temp->childCount = 1;
							  temp->children[0] = $4;
							  excludeFloats($4);
							  $$ = registerNode(temp);
							}

	| postfix_operation '(' ')'	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
					  temp->type = CALL_TYPE;
					  memcpy(temp->modString, $1->modString, 32);
					  temp->childCount = 1;
					  temp->children[0] = $1;
					  requireFunctions($1);
					  $$ = registerNode(fetchMod(temp));
					} 

	| postfix_operation '(' argument_list ')' { struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
						    temp->type = CALL_PARAM_TYPE;
						    memcpy(temp->modString, $1->modString, 32);
						    temp->childCount = 2;
						    temp->children[0] = $1;
						    temp->children[1] = $3;
						    requireFunctions($1);
						    $$ = registerNode(fetchMod(temp));
						  }

	| postfix_operation '<' permute_list '>' { struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
						   temp->type = VEC_PERMUTE_TYPE;
						   memcpy(temp->modString, $1->modString, 32);
						   temp->childCount = 2;
						   temp->children[0] = $1;
						   temp->children[1] = $3;
						   compareTypes($1, $3);
						   $$ = registerNode(temp);
						 }

	| postfix_operation '<' constant '>'  { struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
						temp->type = VEC_INDEX_TYPE;
						memcpy(temp->modString, $1->modString, 32);
						temp->childCount = 2;
						temp->children[0] = $1;
						temp->children[1] = $3;
						excludeFloats($3);
						$$ = registerNode(undoVector(temp));
					      }

	| postfix_operation '.' IDENT { struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
					temp->type = DOT_TYPE;
					memcpy(temp->modString, $1->modString, 32);
					temp->childCount = 2;
					temp->children[0] = $1;
					temp->children[1] = createImmediate($3, 5);
					$$ = registerNode(fetchMod(temp));
				      }

	| postfix_operation ARROW_OP IDENT    { struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
						temp->type = ARROW_TYPE;
						memcpy(temp->modString, $1->modString, 32);
						temp->childCount = 2;
						temp->children[0] = $1;
						temp->children[1] = createImmediate($3, 5);
						$$ = registerNode(fetchMod(temp));
				 	      }
	;



permute_list:
	  constant				{ excludeFloats($1); $$ = $1; }
	| permute_list ',' initial_expression	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
						  temp->type = PERMUTE_LIST_TYPE;
						  memcpy(temp->modString, $1->modString, 32);
						  temp->childCount = 2;
						  temp->children[0] = $1;
						  temp->children[1] = $3;
						  excludeFloats($3);
						  $$ = registerNode(temp);
				 	      	}
	;



argument_list:
	  assignment_operation			 { $$ = $1; }

	| argument_list ',' assignment_operation { struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
						  temp->type = ARGUMENT_LIST_TYPE;
						  memcpy(temp->modString, $1->modString, 32);
						  temp->childCount = 2;
						  temp->children[0] = $1;
						  temp->children[1] = $3;
						  $$ = registerNode(temp);
				 	       	 }
	;



prefix_operation:
	  postfix_operation		{ $$ = $1; }
	| '~' prefix_operation		{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
					  temp->type = NOT_TYPE;
					  memcpy(temp->modString, $2->modString, 32);
					  temp->childCount = 1;
					  temp->children[0] = $2;
					  excludeFloats($2);
					  $$ = registerNode(temp);
			 	       	}

	| '!' prefix_operation		{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
					  temp->type = LOGICAL_NOT_TYPE;
					  memcpy(temp->modString, $2->modString, 32);
					  temp->childCount = 1;
					  temp->children[0] = $2;
					  excludeFloats($2);
					  $$ = registerNode(temp);
			 	       	}

	| '+' prefix_operation		{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
					  temp->type = ABS_TYPE;
					  memcpy(temp->modString, $2->modString, 32);
					  temp->childCount = 1;
					  temp->children[0] = $2;
					  $$ = registerNode(temp);
			 	       	}

	| '-' prefix_operation		{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
					  temp->type = NEG_TYPE;
					  memcpy(temp->modString, $2->modString, 32);
					  temp->childCount = 1;
					  temp->children[0] = $2;
					  $$ = registerNode(temp);
			 	       	}

	| '*' prefix_operation		{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
					  temp->type = DEREF_TYPE;
					  memcpy(temp->modString, $2->modString, 32);
					  temp->childCount = 1;
					  temp->children[0] = $2;
					  requirePointers($2);
					  $$ = registerNode(temp);
			 	       	}

	| ADD_BAR_OP prefix_operation	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
					  temp->type = RUN_SUM_TYPE;
					  memcpy(temp->modString, $2->modString, 32);
					  temp->childCount = 1;
					  temp->children[0] = $2;
					  requireVecs($2);
					  $$ = registerNode(temp);
			 	       	}

	| SUB_BAR_OP prefix_operation	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
					  temp->type = RUN_DIF_TYPE;
					  memcpy(temp->modString, $2->modString, 32);
					  temp->childCount = 1;
					  temp->children[0] = $2;
					  requireVecs($2);
					  $$ = registerNode(temp);
			 	       	}

	| '(' type_name ')' prefix_operation    { struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
						  temp->type = PUNN_TYPE;
						  memcpy(temp->modString, $2->modString, 32);
						  temp->childCount = 1;
						  temp->children[0] = $4;
						  $$ = registerNode(temp);
			 		       	}

	| type_name '(' prefix_operation ')'    { struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
						  temp->type = CONV_TYPE;
						  memcpy(temp->modString, $2->modString, 32);
						  temp->childCount = 1;
						  temp->children[0] = $4;
						  $$ = registerNode(temp);
			 		       	}
	;



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
							  $$ = registerNode(temp);
			 			       	}
	;

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

shift_operation:
	  addsub_operation				{ $$ = $1; }
	| shift_operation GTGT_OP addsub_operation	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = RSH_TYPE;
							  memcpy(temp->modString, $1->modString, 32);
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $3;
							  compareTypes($1, $3);
							  $$ = registerNode(temp);
			 			       	}

	| shift_operation LTLT_OP addsub_operation	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = LSH_TYPE;
							  memcpy(temp->modString, $1->modString, 32);
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $3;
							  compareTypes($1, $3);
							  $$ = registerNode(temp);
			 			       	}
	;

relation_operation:
	  shift_operation				{ $$ = $1; }

	| relation_operation '<' shift_operation	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = LT_TYPE;
							  memcpy(temp->modString, $1->modString, 32);
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $3;
							  compareTypes($1, $3);
							  $$ = registerNode(temp);
			 			       	}

	| relation_operation '>' shift_operation	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = GT_TYPE;
							  memcpy(temp->modString, $1->modString, 32);
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $3;
							  compareTypes($1, $3);
							  $$ = registerNode(temp);
			 			       	}

	| relation_operation LTEQU_OP shift_operation	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = LT_EQU_TYPE;
							  memcpy(temp->modString, $1->modString, 32);
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $3;
							  compareTypes($1, $3);
							  $$ = registerNode(temp);
			 			       	}

	| relation_operation GTEQU_OP shift_operation	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
							  temp->type = GT_EQU_TYPE;
							  memcpy(temp->modString, $1->modString, 32);
							  temp->childCount = 2;
							  temp->children[0] = $1;
							  temp->children[1] = $3;
							  compareTypes($1, $3);
							  $$ = registerNode(temp);
			 			       	}

	| relation_operation '<' shift_operation ':' expression	{ struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 3);
								  temp->type = LT_TERN_TYPE;
								  memcpy(temp->modString, $1->modString, 32);
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
								  memcpy(temp->modString, $1->modString, 32);
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
									  memcpy(temp->modString, $1->modString, 32);
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
									  memcpy(temp->modString, $1->modString, 32);
									  temp->childCount = 3;
									  temp->children[0] = $1;
									  temp->children[1] = $3;
									  temp->children[2] = $5;
									  compareTypes($1, $3);
									  compareTypes($3, $5);
									  $$ = registerNode(temp);
			 					       	}
	;

equality_operation:
	  relation_operation				{ $$ = $1; }
	| equality_operation EQUEQU_OP relation_operation
	| equality_operation NOTEQU_OP relation_operation
	| equality_operation EQUEQU_OP relation_operation ':' expression
	| equality_operation NOTEQU_OP relation_operation ':' expression
	;

bitwise_and_operation:
	  equality_operation				{ $$ = $1; }
	| bitwise_and_operation '&' equality_operation
	;

bitwise_eor_operation:
	  bitwise_and_operation				{ $$ = $1; }
	| bitwise_eor_operation '^' bitwise_and_operation
	;

bitwise_or_operation:
	  bitwise_eor_operation				{ $$ = $1; }
	| bitwise_or_operation '|' bitwise_eor_operation
	;

mesh_operation:
	  bitwise_or_operation				{ $$ = $1; }
	| mesh_operation '~' bitwise_or_operation
	;

logical_and_operation:
	  mesh_operation				{ $$ = $1; }
	| logical_and_operation ANDAND_OP mesh_operation
	;

logical_or_operation:
	  logical_and_operation				{ $$ = $1; }
	| logical_or_operation OROR_OP logical_and_operation
	;

ternary_operation:
	  logical_or_operation				{ $$ = $1; }
	| logical_or_operation '?' expression ':' ternary_operation
	;

assignment_operation:
	  ternary_operation				{ $$ = $1; }
	| prefix_operation '=' assignment_operation
	;

comma_operation:
	  assignment_operation				{ $$ = $1; }
	| comma_operation ',' assignment_operation
	;

expression:
	  comma_operation				{ $$ = $1; }
	;

declaration:
	  hint_modifier type_name declaration_init_list ';'
	| hint_modifier type_name IDENT scope
	| type_name declaration_init_list ';'
	| type_name IDENT scope
	;

declaration_init_list:
	  IDENT '=' expression
	| IDENT
	| declaration_init_list ',' IDENT '=' expression
	| declaration_init_list ',' IDENT
	;

hint_modifier:
	  EXTERN_OP	
	| GLOBAL_OP
	| CONST_OP
	;

base_type:
	  VOID_OP
	| BYTE_OP
	| UBYTE_OP
	| WORD_OP
	| UWORD_OP
	| LONG_OP
	| ULONG_OP
	| QUAD_OP
	| UQUAD_OP
	| SINGLE_OP
	| DOUBLE_OP
	| STRUCT_OP IDENT
	;

base_type_postfix:
	  base_type
	| base_type '<' constant '>'
	| SHARED_OP base_type
	| SHARED_OP base_type '<' constant '>'	
	;

array_modifier:
	  base_type_postfix '[' constant ']'
	| pointer_modifier '[' constant ']'
	;

pointer_modifier:
	  base_type_postfix '*'
	| pointer_modifier  '*'
	| array_modifier    '*'
	| function_modifier '*'
	| base_type_postfix SHARED_OP '*'
	| pointer_modifier  SHARED_OP '*'
	| array_modifier    SHARED_OP '*'
	| function_modifier SHARED_OP '*'
	;

function_modifier:
	  base_type_postfix '(' parameter_list ')'
	| pointer_modifier  '(' parameter_list ')'
	| base_type_postfix '(' parameter_list ',' DOTDOTDOT_OP ')'
	| pointer_modifier  '(' parameter_list ',' DOTDOTDOT_OP ')'
	| base_type_postfix '(' DOTDOTDOT_OP ')'
	| pointer_modifier  '(' DOTDOTDOT_OP ')'
	;

type_name:
	  base_type_postfix
	| function_modifier
	| array_modifier
	| pointer_modifier
	;


parameter_list:
	  type_name
	| type_name IDENT
	| parameter_list ',' type_name
	| parameter_list ',' type_name IDENT
	;

statement:
	  ';'
	| expression ';'
	| declaration
	| scope '}'
	| IF_OP '(' expression ')' statement
	| IF_OP '(' expression ')' statement ELSE_OP statement
	| WHILE_OP '(' expression ')' statement
	| DO_OP statement WHILE_OP '(' expression ')' ';'
	| BREAK_OP ';'
	| CONTINUE_OP ';'
	| SWITCH_OP '(' expression ')' statement
	| CASE_OP constant ':' statement
	| DEFAULT_OP ':' statement
	| RETURN_OP ';'
	| RETURN_OP expression ';'
	| IDENT ':' statement
	| GOTO_OP IDENT ';'
	| FOR_OP '('            ';'            ';'            ')' statement	
	| FOR_OP '('            ';'            ';' expression ')' statement
	| FOR_OP '('            ';' expression ';'            ')' statement
	| FOR_OP '('            ';' expression ';' expression ')' statement
	| FOR_OP '(' expression ';'            ';'            ')' statement
	| FOR_OP '(' expression ';'            ';' expression ')' statement
	| FOR_OP '(' expression ';' expression ';'            ')' statement
	| FOR_OP '(' expression ';' expression ';' expression ')' statement
	| FOR_OP '(' declaration            ';' ')' statement
	| FOR_OP '(' declaration expression ';' ')' statement
	| FOR_OP '(' declaration            ';' expression ')' statement
	| FOR_OP '(' declaration expression ';' expression ')' statement
	;


scope:
	  '{' statement
	| scope statement
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


