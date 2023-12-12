%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>

#define YYDEBUG 1

unsigned long GLOBAL_LINE_NUMBER = 1;
unsigned long IN_COM = 0;
#include "frontEnd/nodeDAG.h"

int yylex(void);
void yyerror(char const *);

FILE *inFile;
%}

%define parse.error verbose

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
%token ANDAND_OP
%token OROR_OP

%token PERCENT_OP
%token EQUALSIGN_OP
%token SEMICOLON_OP

%token QUESTION_OP
%token TILDE_OP
%token PLUS_OP
%token DASH_OP
%token SPLAT_OP
%token SLASH_OP
%token HASH_OP
%token BANG_OP
%token COLON_OP
%token BAR_OP
%token AMPER_OP
%token CARROT_OP

%token OPENSQUARE_OP
%token CLOSESQUARE_OP
%token OPENCHEV_OP
%token CLOSECHEV_OP
%token OPENPAR_OP
%token CLOSEPAR_OP
%token OPENSEMI_OP
%token CLOSESEMI_OP

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

%token BACKSLASH_OP
%token COMMA_OP

%nterm <struct genericNode*> input
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
	  %empty	{ 
		struct genericNode* temp = malloc(sizeof(struct genericNode));
		temp->type = SCOPE_TYPE;
		memset(temp->modString, NONE_MOD, 32); //the big global scope
		temp->childCount = 0;
		$$ = registerNode(temp);
	}

	| input statement	{ $$ = appendAChild($1, $2); } //link statements together in a scope
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// note, instead of sizeof, the preprocessing step has sizeofs and NULL baked in and swaps in a constant
constant:
	  DECIMAL			{ $$ = registerNode(registerSymbol(createImmediate($1,  0)));  }
	| FLOAT				{ $$ = registerNode(registerSymbol(createImmediate($1,  3)));  }
	| BINARY			{ $$ = registerNode(registerSymbol(createImmediate($1,  2)));  }
	| HEX				{ $$ = registerNode(registerSymbol(createImmediate($1,  1)));  }
	| initializer_list BACKSLASH_OP		{ $$ = registerNode(registerSymbol(createImmediate($1, 6))); } //like string lit, but of const type
	| STRING_LIT		{ $$ = registerNode(registerSymbol(createImmediate($1, 4))); } //string immediate (char*)
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
initial_expression: 
	  IDENT						{ $$ = registerNode(registerSymbol(createRef($1)));          } //create referance
	| constant					{ $$ = $1;                                                   } //passthrough

	| OPENPAR_OP expression CLOSEPAR_OP		{ $$ = $2; }	//pass
	| AMPER_OP IDENT					{ $$ = registerNode(registerSymbol(createImmediate($2, 5))); } //get data label
	
	| OPENSQUARE_OP expression CLOSESQUARE_OP { 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
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

	  BACKSLASH_OP DECIMAL			{ dataLitIndex = strlen($2);  dataLitType = 0; $$ = malloc(dataLitIndex + 1); strcpy($$, $2);  }
	| BACKSLASH_OP FLOAT			{ dataLitIndex = strlen($2);  dataLitType = 1; $$ = malloc(dataLitIndex + 1); strcpy($$, $2);  }
	| BACKSLASH_OP BINARY			{ dataLitIndex = strlen($2);  dataLitType = 0; $$ = malloc(dataLitIndex + 1); strcpy($$, $2);  }
	| BACKSLASH_OP HEX				{ dataLitIndex = strlen($2);  dataLitType = 0; $$ = malloc(dataLitIndex + 1); strcpy($$, $2);  }

	| initializer_list COMMA_OP DECIMAL { 
		checkDataLitType(0); 
		$$ = realloc($1, dataLitIndex + strlen( $3 ) + 3);
		$$[dataLitIndex + 0] = ',';
		$$[dataLitIndex + 1] = ' ';
		strcpy(&$$[dataLitIndex + 2], $3); //indexing $$, then getting it's reference: &
		dataLitIndex += strlen($3) + 2; 
	}

	| initializer_list COMMA_OP FLOAT { 
		checkDataLitType(1); 
		$$ = realloc($1, dataLitIndex + strlen( $3 ) + 3); 
		$$[dataLitIndex + 0] = ',';
		$$[dataLitIndex + 1] = ' ';
		strcpy(&$$[dataLitIndex + 2], $3); 
		dataLitIndex += strlen($3) + 2; 
	}

	| initializer_list COMMA_OP BINARY { 
		checkDataLitType(0); 
		$$ = realloc($1, dataLitIndex + strlen( $3 ) + 3); 
		$$[dataLitIndex + 0] = ',';
		$$[dataLitIndex + 1] = ' ';
		strcpy(&$$[dataLitIndex + 2], $3); 
		dataLitIndex += strlen($3) + 2; 
	}

	| initializer_list COMMA_OP HEX { 
		checkDataLitType(0); 
		$$ = realloc($1, dataLitIndex + strlen( $3 ) + 3);
		$$[dataLitIndex + 0] = ',';
		$$[dataLitIndex + 1] = ' ';
		strcpy(&$$[dataLitIndex + 2], $3); 
		dataLitIndex += strlen($3) + 2; 
	}

	;



//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
postfix_operation:
	  initial_expression				{ $$ = $1; }

	| postfix_operation OPENPAR_OP CLOSEPAR_OP { 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
		temp->type = CALL_TYPE;
		memcpy(temp->modString, $1->modString, 32);
		temp->childCount = 1;
		temp->children[0] = $1;
		requireFunctions($1);
		$$ = registerNode(fetchFunc(temp)); //this operates on function pointers and functions
	} 

	| postfix_operation argument_list CLOSEPAR_OP { 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*)*2);
		temp->type = CALL_PARAM_TYPE;
		memcpy(temp->modString, $1->modString, 32);
		temp->childCount = 2;
		temp->children[0] = $1;
		temp->children[1] = $2;
		requireFunctions($1);
		checkArgumentTypes($1, $2);
		$$ = registerNode(fetchFunc(temp)); //same here
	}

	| postfix_operation permute_list CLOSESQUARE_OP { 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
		temp->type = PERMUTE_TYPE;
		memcpy(temp->modString, $1->modString, 32);
		temp->childCount = 2;
		temp->children[0] = $1;
		temp->children[1] = $2;
		checkLengths($1, $2);
		requireVecs($1);
		$$ = registerNode(temp);
	}

	| postfix_operation OPENSQUARE_OP constant CLOSESQUARE_OP { 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
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

	  OPENSQUARE_OP constant { 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
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

	| permute_list COMMA_OP constant	{ 
		enforceScalerInts($3);
		$1->modString[1] -= 1;	//add one to the length
		$$ = appendAChild($1, $3);	//add the constant to the list
	}
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
argument_list:

	  OPENPAR_OP ternary_operation {
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
		temp->type = ARGUMENT_LIST_TYPE;
		memset(temp->modString, NONE_MOD, 32);
		temp->childCount = 1;
		temp->children[0] = $2;
		$$ = registerNode(temp);	//create the head, like above
	}

	| argument_list COMMA_OP ternary_operation 	 { $$ = appendAChild($1, $3); /* add a argument to the list */ }
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
prefix_operation:

	  postfix_operation			{ $$ = $1; }

	| TILDE_OP prefix_operation { 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
		temp->type = NOT_TYPE;
		memcpy(temp->modString, $2->modString, 32);
		temp->childCount = 1;
		temp->children[0] = $2;
		excludeFloats($2);		//no bitwise on floats
		$$ = registerNode(temp);
	}

	| BANG_OP prefix_operation { 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
		temp->type = LOGICAL_NOT_TYPE;
		memcpy(temp->modString, $2->modString, 32); //no matter the type, 0 = FALSE; !0 = TRUE
		temp->childCount = 1;
		temp->children[0] = $2;
		$$ = registerNode(temp);
	}

	| DASH_OP prefix_operation { 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*));
		temp->type = NEG_TYPE;
		memcpy(temp->modString, $2->modString, 32);
		temp->childCount = 1;
		temp->children[0] = $2;
		$$ = registerNode(temp);
	}

	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
multdiv_operation:
	  prefix_operation				{ $$ = $1; }

	| multdiv_operation SPLAT_OP prefix_operation { 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
		temp->type = MUL_TYPE;
		temp->childCount = 2;
		temp->children[0] = $1;
		temp->children[1] = $3;
		compareTypes($1, $3, temp->modString); //check and select type
		$$ = registerNode(temp);
	}

	| multdiv_operation SLASH_OP prefix_operation { 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
		temp->type = DIV_TYPE;
		temp->childCount = 2;
		temp->children[0] = $1;
		temp->children[1] = $3;
		compareTypes($1, $3, temp->modString); //check and select type
		$$ = registerNode(temp);
	}

	| multdiv_operation PERCENT_OP prefix_operation { 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
		temp->type = MOD_TYPE;
		temp->childCount = 2;
		temp->children[0] = $1;
		temp->children[1] = $3;
		compareTypes($1, $3, temp->modString); //check and select type
		excludeFloats($1);
		$$ = registerNode(temp);
	}
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
addsub_operation:

	  multdiv_operation				{ $$ = $1; }

	| addsub_operation PLUS_OP multdiv_operation { 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
		temp->type = ADD_TYPE;
		temp->childCount = 2;
		temp->children[0] = $1;
		temp->children[1] = $3;
		compareTypes($1, $3, temp->modString); //check and select type
		$$ = registerNode(temp);
	}

	| addsub_operation DASH_OP multdiv_operation { 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
		temp->type = SUB_TYPE;
		temp->childCount = 2;
		temp->children[0] = $1;
		temp->children[1] = $3;
		compareTypes($1, $3, temp->modString); //check and select type
		$$ = registerNode(temp);
	}

	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
shift_operation:

	  addsub_operation				{ $$ = $1; }

	| shift_operation GTGT_OP addsub_operation	{ 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
		temp->type = RSH_TYPE;
		temp->childCount = 2;
		temp->children[0] = $1;
		temp->children[1] = $3;
		compareTypes($1, $3, temp->modString); //check and select type
		excludeFloats($1);
		$$ = registerNode(temp);
	}

	| shift_operation LTLT_OP addsub_operation	{ 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
		temp->type = LSH_TYPE;
		temp->childCount = 2;
		temp->children[0] = $1;
		temp->children[1] = $3;
		compareTypes($1, $3, temp->modString); //check and select type
		excludeFloats($1);
		$$ = registerNode(temp);
	}
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
relation_operation:

	  shift_operation				{ $$ = $1; }

	| relation_operation OPENCHEV_OP shift_operation { 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
		temp->type = LT_TYPE;
		temp->childCount = 2;
		temp->children[0] = $1;
		temp->children[1] = $3;
		compareTypes($1, $3, temp->modString); //check and select type; typeA > typeA = typeA; only thing that matters is 0 = FALSE, !0 = TRUE
		$$ = registerNode(temp);
  	}

	| relation_operation CLOSECHEV_OP shift_operation { 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
		temp->type = GT_TYPE;
		temp->childCount = 2;
		temp->children[0] = $1;
		temp->children[1] = $3;
		compareTypes($1, $3, temp->modString); //check and select type; typeA > typeA = typeA; only thing that matters is 0 = FALSE, !0 = TRUE
		$$ = registerNode(temp);
	}

	| relation_operation LTEQU_OP shift_operation { 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
		temp->type = LT_EQU_TYPE;
		temp->childCount = 2;
		temp->children[0] = $1;
		temp->children[1] = $3;
		compareTypes($1, $3, temp->modString); //check and select type; typeA > typeA = typeA; only thing that matters is 0 = FALSE, !0 = TRUE
		$$ = registerNode(temp);
	}

	| relation_operation GTEQU_OP shift_operation { 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
		temp->type = GT_EQU_TYPE;
		temp->childCount = 2;
		temp->children[0] = $1;
		temp->children[1] = $3;
		compareTypes($1, $3, temp->modString); //check and select type; typeA > typeA = typeA; only thing that matters is 0 = FALSE, !0 = TRUE
		$$ = registerNode(temp);
	}
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
equality_operation:

	  relation_operation { $$ = $1; }

	| equality_operation EQUEQU_OP relation_operation { 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
		temp->type = EQU_EQU_TYPE;
		temp->childCount = 2;
		temp->children[0] = $1;
		temp->children[1] = $3;
		compareTypes($1, $3, temp->modString); //check and select type; typeA > typeA = typeA; only thing that matters is 0 = FALSE, !0 = TRUE
		$$ = registerNode(temp);
	}

	| equality_operation NOTEQU_OP relation_operation { 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
		temp->type = NOT_EQU_TYPE;
		temp->childCount = 2;
		temp->children[0] = $1;
		temp->children[1] = $3;
		compareTypes($1, $3, temp->modString); //check and select type; typeA > typeA = typeA; only thing that matters is 0 = FALSE, !0 = TRUE
		$$ = registerNode(temp);
	}
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
bitwise_and_operation:

	  equality_operation { $$ = $1; }

	| bitwise_and_operation AMPER_OP equality_operation { 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
		temp->type = AND_TYPE;
		temp->childCount = 2;
		temp->children[0] = $1;
		temp->children[1] = $3;
		compareTypes($1, $3, temp->modString); //check and select type
		excludeFloats($1);
		$$ = registerNode(temp);
	}
	;



//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
bitwise_eor_operation:
	  bitwise_and_operation				  { $$ = $1; }

	| bitwise_eor_operation CARROT_OP bitwise_and_operation { 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
		temp->type = EOR_TYPE;
		temp->childCount = 2;
		temp->children[0] = $1;
		temp->children[1] = $3;
		compareTypes($1, $3, temp->modString); //check and select type
		excludeFloats($1);
		$$ = registerNode(temp);
	}
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
bitwise_or_operation:
	  bitwise_eor_operation				 { $$ = $1; }

	| bitwise_or_operation BAR_OP bitwise_eor_operation {
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
		temp->type = OR_TYPE;
		temp->childCount = 2;
		temp->children[0] = $1;
		temp->children[1] = $3;
		compareTypes($1, $3, temp->modString); //check and select type
		excludeFloats($1);
		$$ = registerNode(temp);
	}
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
logical_and_operation:
	  bitwise_or_operation				  { $$ = $1; }

	| logical_and_operation ANDAND_OP bitwise_or_operation {
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
		temp->type = LOGIC_AND_TYPE;
		memcpy(temp->modString, $1->modString, 32); //carry the left type forward, but logical ops ignore type
		//enforce matching vector lengths / scalars
		temp->childCount = 2;
		temp->children[0] = $1;
		temp->children[1] = $3;
		$$ = registerNode(temp);
	}
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
logical_or_operation:
	  logical_and_operation				{ $$ = $1; }

	| logical_or_operation OROR_OP logical_and_operation {
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
		temp->type = LOGIC_OR_TYPE;
		memcpy(temp->modString, $1->modString, 32); //carry the left type forward, but logical ops ignore type
		temp->childCount = 2;
		temp->children[0] = $1;
		temp->children[1] = $3;
		$$ = registerNode(temp);
	}

	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
ternary_operation:
	  logical_or_operation				{ $$ = $1; }
	| logical_or_operation QUESTION_OP expression COLON_OP ternary_operation {
		struct genericNode* temp = malloc(sizeof(struct genericNode)+sizeof(struct genericNode*)*3);
		temp->type = TERN_TYPE;
		memcpy(temp->modString, $3->modString, 32);
		temp->childCount = 3;
		temp->children[0] = $1;
		temp->children[1] = $3;
		temp->children[2] = $5;
		compareTypes($3, $5, temp->modString); //check and select type
		$$ = registerNode(temp);
	}
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
expression:
	  ternary_operation	{ $$ = $1; }
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
variable_declaration:

	  type_name IDENT SEMICOLON_OP { 	//does both variables and stub functions
		closeFalseScope(); 												//close the previous false scope from typename
		struct symbolEntry* temp = malloc(sizeof(struct symbolEntry));	//create a new symbol

		memcpy(temp->modString, $1, 32);								//set its type 
		memcpy(temp->name, $2, 128);									//set its name
		temp->innerScope = NULL;										//no inner scope (isn't a function)
		if(currentScopeCounter != 0)
			excludeFunctionsType($1);										//no pure functions
		$$ = registerNode(registerSymbol(temp));						//create the symbol, pass back a reference
	}

	| type_name IDENT EQUALSIGN_OP expression SEMICOLON_OP {
		closeFalseScope(); 												//close the previous false scope from typename
		struct symbolEntry* temp = malloc(sizeof(struct symbolEntry));	//create a new symbol
		if(currentScopeCounter < 1)
		if($4->type != SYMBOL_TYPE){
			fprintf(stderr, "ERR: cannot use EQU in a zero scope, LINE %ld\n", GLOBAL_LINE_NUMBER);
			exit(1);
		}

		memcpy(temp->modString, $1, 32);								//set its type
		memcpy(temp->name, $2, 128);									//set its name
		temp->innerScope = NULL;										//no inner scope (isn't a function)
		excludeFunctionsType($1);		 								//no pure functions
		struct genericNode* tempequ = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
		tempequ->type = EQU_TYPE;												//creating a set operation...
		tempequ->childCount = 2;
		tempequ->children[0] = registerNode(registerSymbol(temp));  			//register the symbol,referencing the symbol we just made
		tempequ->children[1] = $4;
		compareTypes(tempequ->children[0], $4, tempequ->modString); 			//check and select type
		$$ = registerNode(tempequ);												//pass back the set operation
	}
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
function_declaration:
	  type_name IDENT statement	{
		requireFunctionsType($1);										//needs to be a pure function
		closeFalseScope();												//close the false scope created by function mod
		struct symbolEntry* temp = malloc(sizeof(struct symbolEntry));	//new symbol
		memcpy(temp->modString, $1, 32);								//type
		free($1);
		strcpy(temp->name, $2);											//name
		temp->innerScope = $3;											//has a scope
		if($3->type != SCOPE_TYPE){
			fprintf(stderr, "ERR: FUNCTIONS REQUIRE STMT TO BE A SCOPE, LINE %ld\n", GLOBAL_LINE_NUMBER);
			exit(1);
		}
		$$ = registerNodeFunction(registerSymbol(temp));				//register and return
		enforceZeroScope();												//on the zeroth scope
		endFunction();													//end the function DAG list
	}
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
declaration:
	  variable_declaration		 { $$ = $1; } //passthrough
	| function_declaration		 { $$ = $1; } //^
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
hint_modifier:
	  EXTERN_OP	{ $$ = EXTERN_HINT; enforceZeroScope(); }	//mark something as extern
	| GLOBAL_OP	{ $$ = GLOBAL_HINT; enforceZeroScope(); }	//mark something as persisting between files
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
base_type:
	  VOID_OP		{ char *temp = calloc(32, 1); globalTypeIndex = 0; temp[globalTypeIndex] = VOID_BASE;   $$ = temp; }
	| BYTE_OP		{ char *temp = calloc(32, 1); globalTypeIndex = 0; temp[globalTypeIndex] = BYTE_BASE;   $$ = temp; }
	| WORD_OP		{ char *temp = calloc(32, 1); globalTypeIndex = 0; temp[globalTypeIndex] = WORD_BASE;   $$ = temp; }
	| LONG_OP		{ char *temp = calloc(32, 1); globalTypeIndex = 0; temp[globalTypeIndex] = LONG_BASE;   $$ = temp; }
	| QUAD_OP		{ char *temp = calloc(32, 1); globalTypeIndex = 0; temp[globalTypeIndex] = QUAD_BASE;   $$ = temp; }
	| SINGLE_OP		{ char *temp = calloc(32, 1); globalTypeIndex = 0; temp[globalTypeIndex] = SINGLE_BASE; $$ = temp; }
	| DOUBLE_OP		{ char *temp = calloc(32, 1); globalTypeIndex = 0; temp[globalTypeIndex] = DOUBLE_BASE; $$ = temp; }
	| hint_modifier VOID_OP		{ enforceZeroScope(); char *temp = calloc(32, 1); globalTypeIndex = 1; temp[0] = $1; temp[globalTypeIndex] = VOID_BASE;   $$ = temp; }
	| hint_modifier BYTE_OP		{ enforceZeroScope(); char *temp = calloc(32, 1); globalTypeIndex = 1; temp[0] = $1; temp[globalTypeIndex] = BYTE_BASE;   $$ = temp; }
	| hint_modifier WORD_OP		{ enforceZeroScope(); char *temp = calloc(32, 1); globalTypeIndex = 1; temp[0] = $1; temp[globalTypeIndex] = WORD_BASE;   $$ = temp; }
	| hint_modifier LONG_OP		{ enforceZeroScope(); char *temp = calloc(32, 1); globalTypeIndex = 1; temp[0] = $1; temp[globalTypeIndex] = LONG_BASE;   $$ = temp; }
	| hint_modifier QUAD_OP		{ enforceZeroScope(); char *temp = calloc(32, 1); globalTypeIndex = 1; temp[0] = $1; temp[globalTypeIndex] = QUAD_BASE;   $$ = temp; }
	| hint_modifier SINGLE_OP	{ enforceZeroScope(); char *temp = calloc(32, 1); globalTypeIndex = 1; temp[0] = $1; temp[globalTypeIndex] = SINGLE_BASE; $$ = temp; }
	| hint_modifier DOUBLE_OP	{ enforceZeroScope(); char *temp = calloc(32, 1); globalTypeIndex = 1; temp[0] = $1; temp[globalTypeIndex] = DOUBLE_BASE; $$ = temp; }
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
base_type_postfix:
	  base_type				{ $$ = $1; }

	| base_type OPENCHEV_OP constant CLOSECHEV_OP { 
		$1[globalTypeIndex + 2] = $1[globalTypeIndex];	//as a vector is VECTORMOD SIZE BASETYPE, we have to move BASETYPE
		$1[globalTypeIndex] = VECTOR_MOD;
		enforceScalerInts($3);							//vector size must be an int
		long temp;
		if($1[globalTypeIndex + 2] == BYTE_BASE){
			fprintf(stderr, "ERR: currently, byte vectors are unsuported\n");
			exit(1);
		}

		sscanf( ((struct symbolEntry*)($3->children[0]))->constValue, "%ld", &temp); //extract the constant size from the constant
		if(temp > maxForType($1[globalTypeIndex + 2]) || temp < 1){
			fprintf(stderr, "ERR: vector constant too large (not 1 to %d), %ld", maxForType($1[globalTypeIndex + 2]), temp); //vectors are short, 1 to 128 elements
			exit(1);
		}
		$1[globalTypeIndex + 1] = (char)(-temp);	//enter length, vector lengths are stored negative to avoid conflicts with the TYPE markers
		globalTypeIndex += 2;
		$$ = $1;
	}	
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
pointer_modifier:
	  OPENSQUARE_OP base_type_postfix CLOSESQUARE_OP		{ $2[++globalTypeIndex] = POINTER_POSTFIX; $$ = $2; } 
	| OPENSQUARE_OP pointer_modifier  CLOSESQUARE_OP		{ $2[++globalTypeIndex] = POINTER_POSTFIX; $$ = $2; } //mark as pointer
	| OPENSQUARE_OP function_modifier CLOSESQUARE_OP		{ $2[++globalTypeIndex] = POINTER_POSTFIX; $$ = $2; }
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
function_modbase:
	  base_type_postfix OPENPAR_OP { openFalseScope(); $1[++globalTypeIndex] = FUNCTION_POSTFIX; $$ = $1; } //mark as function
 	| pointer_modifier  OPENPAR_OP { openFalseScope(); $1[++globalTypeIndex] = FUNCTION_POSTFIX; $$ = $1; } //we open a false scope here
	;


function_modifier:
	  function_modbase CLOSEPAR_OP { //finished the function, pop the old index, nothing to copy, no params
		$1[++globalTypeIndex] = CLOSE_FUNCTION_POSTFIX; 
		$$ = $1; 
	}

	| function_modbase parameter_list CLOSEPAR_OP { //finished the function, pop the old index, copy the param types
		copyAndPopTypeIndex($1, $2); 
		free($2); 
		$1[++globalTypeIndex] = CLOSE_FUNCTION_POSTFIX; 
		$$ = $1; 
	}
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
type_name:
	  base_type_postfix	{ $$ = $1; }
	| function_modifier	{ $$ = $1; }
	| pointer_modifier	{ $$ = $1; }
	;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

pushTypeIndexR: %empty {pushTypeIndex();} 
	;

parameter_list:

	  pushTypeIndexR type_name IDENT { 
		struct symbolEntry* temp = malloc(sizeof(struct symbolEntry));
		memcpy(temp->modString, $2, 32);
		strcpy(temp->name, $3);
		registerSymbol(temp);
		paramIndex = 0;
		temp->paramIndex = paramIndex++;
		$$ = $2;						// don't pop here as this typename will be merged repeatedly, then merged with the final one
 	}

	| parameter_list COMMA_OP pushTypeIndexR type_name IDENT	{ 
		struct symbolEntry* temp = malloc(sizeof(struct symbolEntry));
		memcpy(temp->modString, $4, 32);
		strcpy(temp->name, $5);
		registerSymbol(temp);
		temp->paramIndex = paramIndex++;
		copyAndPopTypeIndex($1, $4);
		free($4);
		$$ = $1;
	}
	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
openScopeHelper: OPENPAR_OP {openScope(); setBasicBlock();}  //control flow structure's () are part of their scope
	;

statement:
	  SEMICOLON_OP					{ $$ = NULL; }
	| expression SEMICOLON_OP		{ $$ = $1; }
	| declaration					{ $$ = $1; }
	| scope CLOSESEMI_OP			{ closeScope(); $$ = $1; }


	| IDENT EQUALSIGN_OP expression SEMICOLON_OP {
		clobberStores(); 																					//clober all symbol stores
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
		temp->type = EQU_TYPE;
		temp->childCount = 2;
		temp->children[0] = registerNode(registerSymbol(createRef($1)));				//create a reference on the right side
		temp->children[1] = $3;
		compareTypes(temp->children[0], $3, temp->modString);							//compare and check types
		$$ = registerNode(temp);
	}

	| OPENSQUARE_OP expression CLOSESQUARE_OP EQUALSIGN_OP expression SEMICOLON_OP { 
		clobberStores();																//clober all symbol stores
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
		temp->type = EQU_TYPE;
		memcpy(temp->modString, $2->modString, 32);
		temp->childCount = 2;
		temp->children[0] = $2;					//this is a raw store
		temp->children[1] = $5;
		compareTypes($2, $5, temp->modString);	//compare and check types
		$$ = registerNode(temp);
	}


	| IF_OP openScopeHelper expression CLOSEPAR_OP statement { 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
		temp->type = IF_TYPE;
		memset(temp->modString, NONE_MOD, 32);	//control flow doesn't have a type
		temp->childCount = 2;
		temp->children[0] = $3;
		temp->children[1] = $5;
		enforceScalers($3);
		closeScope();
		setBasicBlock();
		$$ = registerNode(temp);
	}

	| IF_OP openScopeHelper expression CLOSEPAR_OP statement ELSE_OP statement { 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 3);
		temp->type = IF_ELSE_TYPE;
		memset(temp->modString, NONE_MOD, 32);	//control flow doesn't have a type
		temp->childCount = 3;
		temp->children[0] = $3;
		temp->children[1] = $5;
		temp->children[2] = $7;
		enforceScalers($3);
		closeScope();
		setBasicBlock();
		$$ = registerNode(temp);
	}

	| WHILE_OP openScopeHelper expression CLOSEPAR_OP statement  {
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 2);
		temp->type = WHILE_TYPE;
		memset(temp->modString, NONE_MOD, 32);	//control flow doesn't have a type
		temp->childCount = 2;
		temp->children[0] = $3;
		temp->children[1] = $5;
		enforceScalers($3);
		closeScope();
		setBasicBlock();
		$$ = registerNode(temp);
	}
							
	| BREAK_OP SEMICOLON_OP {
		struct genericNode* temp = malloc(sizeof(struct genericNode));
		temp->type = BREAK_TYPE;
		memset(temp->modString, NONE_MOD, 32); //control flow doesn't have a type
		temp->childCount = 0;
		$$ = registerNode(temp);
	}

	| CONTINUE_OP SEMICOLON_OP { 
		struct genericNode* temp = malloc(sizeof(struct genericNode));
		temp->type = CONTINUE_TYPE;
		memset(temp->modString, NONE_MOD, 32); //control flow doesn't have a type
		temp->childCount = 0;
		$$ = registerNode(temp);
	}

	| RETURN_OP SEMICOLON_OP {
		struct genericNode* temp = malloc(sizeof(struct genericNode));
		temp->type = RETURN_TYPE;
		memset(temp->modString, NONE_MOD, 32); //control flow doesn't have a type
		temp->childCount = 0;
		$$ = registerNode(temp);
	}

	| RETURN_OP expression SEMICOLON_OP { 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 1);
		temp->type = RETURN_EXP_TYPE;
		memset(temp->modString, NONE_MOD, 32); //control flow doesn't have a type
		temp->childCount = 1;
		temp->children[0] = $2;
		$$ = registerNode(temp);
	}
	
	| IDENT COLON_OP statement {
		setBasicBlock();
		struct symbolEntry* temp = malloc(sizeof(struct symbolEntry));	//create a new label symbol
		memset(temp->modString, NONE_MOD, 32);							//set its type..
		temp->modString[0] = LABEL_BASE;								//..as label
		memcpy(temp->name, $1, 128);									//set its name
		temp->innerScope = NULL;										//no inner scope (isn't a function)
		struct genericNode* temp2 = registerNode(registerSymbol(temp));
		temp2->type = LABEL_TYPE;
		$$ = appendAChild(temp2, $3); 
	}

	| GOTO_OP IDENT SEMICOLON_OP	{ 
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 1);
		temp->type = GOTO_TYPE;
		memset(temp->modString, NONE_MOD, 32); //control flow doesn't have a type
		temp->childCount = 1;
		temp->children[0] = registerNode(registerSymbol(createRef($2)));
		$$ = registerNode(temp);
	}

	| FOR_OP openScopeHelper statement            SEMICOLON_OP statement CLOSEPAR_OP statement {
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 4);
		temp->type = FOR_TYPE;
		memset(temp->modString, NONE_MOD, 32); //control flow doesn't have a type
		temp->childCount = 4;
		temp->children[0] = $3;
		temp->children[1] = NULL;
		temp->children[2] = $5;
		temp->children[3] = $7;
		closeScope();
		setBasicBlock();
		$$ = registerNode(temp);
	}

	| FOR_OP openScopeHelper statement expression SEMICOLON_OP statement CLOSEPAR_OP statement {
		struct genericNode* temp = malloc(sizeof(struct genericNode) + sizeof(struct genericNode*) * 4);
		temp->type = FOR_TYPE;
		memset(temp->modString, NONE_MOD, 32); //control flow doesn't have a type
		temp->childCount = 4;
		temp->children[0] = $3;
		temp->children[1] = $4;
		temp->children[2] = $6;
		temp->children[3] = $8;
		enforceScalers($4);
		closeScope();
		setBasicBlock();
		$$ = registerNode(temp);
	}

	;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
scope:

	  OPENSEMI_OP { 
		struct genericNode* temp = malloc(sizeof(struct genericNode));
		temp->type = SCOPE_TYPE;
		memset(temp->modString, NONE_MOD, 32); //control flow doesn't have a type
		temp->childCount = 0;
		openScope();
		$$ = registerNode(temp);
	}

	| scope statement		{ $$ = appendAChild($1, $2); } //link statements together in a scope
	;


%%

#include "lex.yy.c"

void yyerror(char const *s){
	fprintf(stderr, "%s on line %ld\n", s, GLOBAL_LINE_NUMBER);
}


struct genericNode** frontEnd(char *filename){

	//yyin = fopen("../demo.txt", "r");
	yyin = stdin;
	GLOBAL_LINE_NUMBER = 0;

	yydebug = 0;
	
	initNodes();

	yyparse();
	
	printf("\n\nok\n");

	return DAG;
}


