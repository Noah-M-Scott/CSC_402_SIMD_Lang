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
	| iota_vector		{ $$ = $1; }
	| initializer_list	{ $$ = $1; }
	| '&' IDENT		{ $$ = registerNode(registerSymbol(createImmediate($1, 5))); }
	| SIZEOF_OP '(' type_name ')'	{ $$ = registerNode(registerSymbol(createSizeOf($1, 0))); }
	| SIZEOF_OP '(' IDENT ')'	{ $$ = registerNode(registerSymbol(createSizeOf($1, 1))); }
	;

initializer:
	  constant
	| '{' initializer_list '}'
	;

initializer_list:
	  initializer
	| initializer_list ',' initializer
	;

iota_vector:
	  '[' constant ',' constant ']'
	| '(' constant ',' constant ']'
	| '[' constant ',' constant ')'
	| '(' constant ',' constant ')'
	| '[' constant ',' expression ',' constant ']'
	| '(' constant ',' expression ',' constant ']'
	| '[' constant ',' expression ',' constant ')'
	| '(' constant ',' expression ',' constant ')'
	;

postfix_operation:
	  initial_expression
	| postfix_operation '[' expression ']'
	| '&' postfix_operation '[' expression ']'
	| postfix_operation '(' ')'
	| postfix_operation '(' argument_list ')'
	| postfix_operation '<' permute_list '>'
	| postfix_operation '<' block_permute '>'
	| postfix_operation '.' IDENT
	| postfix_operation ARROW_OP IDENT
	;

permute_list:
	  initial_expression
	| permute_list ',' initial_expression
	| permute_list ',' initial_expression ':' constant
	| permute_list ',' initial_expression ':' constant ':' constant
	;

block_permute:
	  constant
	| block_permute COLCOL_OP constant
	;

argument_list:
	  assignment_operation
	| argument_list ',' assignment_operation
	;

prefix_operation:
	  postfix_operation
	| '~' prefix_operation
	| '!' prefix_operation
	| '+' prefix_operation
	| '-' prefix_operation
	| '*' prefix_operation
	| ADD_BAR_OP prefix_operation
	| SUB_BAR_OP prefix_operation
	| '(' type_name ')' prefix_operation
	| type_name '(' prefix_operation ')'
	;

multdiv_operation:
	  prefix_operation
	| multdiv_operation '*' prefix_operation
	| multdiv_operation '/' prefix_operation
	| multdiv_operation '%' prefix_operation
	;

addsub_operation:
	  multdiv_operation
	| addsub_operation '+' multdiv_operation
	| addsub_operation '-' multdiv_operation
	;

shift_operation:
	  addsub_operation
	| shift_operation GTGT_OP addsub_operation
	| shift_operation LTLT_OP addsub_operation
	;

relation_operation:
	  shift_operation
	| relation_operation '<' shift_operation
	| relation_operation '>' shift_operation
	| relation_operation LTEQU_OP shift_operation
	| relation_operation GTEQU_OP shift_operation
	| relation_operation '<' shift_operation ':' expression
	| relation_operation '>' shift_operation ':' expression
	| relation_operation LTEQU_OP shift_operation ':' expression
	| relation_operation GTEQU_OP shift_operation ':' expression
	;

equality_operation:
	  relation_operation
	| equality_operation EQUEQU_OP relation_operation
	| equality_operation NOTEQU_OP relation_operation
	| equality_operation EQUEQU_OP relation_operation ':' expression
	| equality_operation NOTEQU_OP relation_operation ':' expression
	;

bitwise_and_operation:
	  equality_operation
	| bitwise_and_operation '&' equality_operation
	;

bitwise_eor_operation:
	  bitwise_and_operation
	| bitwise_eor_operation '^' bitwise_and_operation
	;

bitwise_or_operation:
	  bitwise_eor_operation
	| bitwise_or_operation '|' bitwise_eor_operation
	;

mesh_operation:
	  bitwise_or_operation
	| mesh_operation '~' bitwise_or_operation
	;

logical_and_operation:
	  mesh_operation
	| logical_and_operation ANDAND_OP mesh_operation
	;

logical_or_operation:
	  logical_and_operation
	| logical_or_operation OROR_OP logical_and_operation
	;

ternary_operation:
	  logical_or_operation
	| logical_or_operation '?' expression ':' ternary_operation
	;

assignment_operation:
	  ternary_operation
	| prefix_operation '=' assignment_operation
	;

comma_operation:
	  assignment_operation
	| comma_operation ',' assignment_operation
	;

expression:
	  comma_operation
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


