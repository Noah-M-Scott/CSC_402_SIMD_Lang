%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int yylex(void);
void yyerror(char const *);

FILE *inFile;
FILE *outFile;
%}

%define api.value.type union
%token <unsigned long long> NUMBER
%token <long long> SIGN_NUMBER
%token <double> FLOAT
%token <char*> IDENT
%token LTLT_OP
%token GTGT_OP
%token EQUEQU_OP
%token GTEQU_OP
%token LTEQU_OP
%token NOTEQU_OP
%token ARROW_OP
%token SWAP_OP
%token ANDAND_OP
%token OROR_OP
%token SIZEOF_OP
%token COLCOL_OP
%token DOTDOTDOT_OP

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
%token ENUM_OP

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



%%

input:
	  %empty
	| input initial_expression
	;

constant:
	  SIGN_NUMBER   {printf("%lld\n", $1);}
	| NUMBER	{printf("%llu\n", $1);}
	| FLOAT		{printf("%lf\n", $1);}
	;

initial_expression: 
	  IDENT
	| constant
	| DOTDOTDOT_OP
	| '(' expression ')'
	| iota_vector
	| initializer
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
	  '[' expression ',' expression ']'
	| '(' expression ',' expression ']'
	| '[' expression ',' expression ')'
	| '(' expression ',' expression ')'
	| '[' expression ',' expression ',' expression ']'
	| '(' expression ',' expression ',' expression ']'
	| '[' expression ',' expression ',' expression ')'
	| '(' expression ',' expression ',' expression ')'
	;

postfix_operation:
	  initial_expression
	| postfix_operation '[' expression ']'
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
	| permute_list ',' initial_expression ':' initial_expression
	| permute_list ',' initial_expression ':' initial_expression ':' initial_expression
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
	| '&' prefix_operation
	| '*' prefix_operation
	| '|' prefix_operation
	| OROR_OP prefix_operation
	| '(' type_name ')' prefix_operation
	| type_name '(' prefix_operation ')'
	| SIZEOF_OP '(' type_name ')'
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
	| prefix_operation SWAP_OP assignment_operation
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
	| STRUCT_OP IDENT '{' struct_declaration_list '}'
	| ENUM_OP IDENT '{' enumerator_list '}'
	;

struct_declaration_list:
	  struct_declaration
	| struct_declaration_list ',' struct_declaration
	;

struct_declaration:
	  type_name struct_declaration_initializer_list ';'
	;

struct_declaration_initializer_list:
	  IDENT
	| struct_declaration_initializer_list ',' IDENT
	;

enumerator_list:
	  IDENT
	| IDENT '=' constant
	| enumerator_list ',' IDENT
	| enumerator_list ',' IDENT '=' constant
	;

base_type_postfix:
	  base_type
	| base_type '<' constant '>'
	| CONST_OP  base_type
	| SHARED_OP base_type
	| CONST_OP  base_type '<' expression '>'
	| SHARED_OP base_type '<' expression '>'	
	;

array_modifier:
	  base_type_postfix '[' expression ']'
	| pointer_modifier '[' expression ']'
	| base_type_postfix CONST_OP  '[' expression ']'
	| base_type_postfix SHARED_OP '[' expression ']'
	| pointer_modifier  CONST_OP  '[' expression ']'
	| pointer_modifier  SHARED_OP '[' expression ']'	
	;

pointer_modifier:
	  base_type_postfix '*'
	| pointer_modifier  '*'
	| array_modifier    '*'
	| function_modifier '*'
	| base_type_postfix CONST_OP  '*'
	| pointer_modifier  CONST_OP  '*'
	| array_modifier    CONST_OP  '*'
	| function_modifier CONST_OP  '*'
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


directive:
	  INCLUDE_OP '"' IDENT '"'
	| IFDEF_OP IDENT THEN_OP directive
	| IFDEF_OP IDENT THEN_OP directive ELSE_OP directive
	| IFNDEF_OP IDENT THEN_OP directive
	| IFNDEF_OP IDENT THEN_OP directive ELSE_OP directive
	| IF_OP constant THEN_OP directive
	| IF_OP constant THEN_OP directive ELSE_OP directive
	| DEFINE_OP IDENT
	| UNDEF_OP IDENT
	| NOTHING_OP
	;

compiler_directive:
	  '#' directive '#'
	;


%%

unsigned long GLOBAL_LINE_NUMBER;

#include "lex.yy.c"

void yyerror(char const *s){
	fprintf(stderr, "%s\n", s);
}

int main(int argc, char *argv[]){
	inFile = stdin;
	outFile = stdout;

	yyin = stdin;
	GLOBAL_LINE_NUMBER = 0;
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


