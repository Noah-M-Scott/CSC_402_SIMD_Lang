%option caseless
%option noyywrap



%%
[ \r\t]+	 	{ continue; }

[\n]			{ GLOBAL_LINE_NUMBER++; continue; }

[0-9]+"."[0-9]+  	{ sscanf(yytext, "%lf", &yylval.FLOAT); return FLOAT; } //float
"-"[0-9]+"."[0-9]+  	{ sscanf(yytext, "%lf", &yylval.FLOAT); return FLOAT; } //float

[0-9]+	 		{ sscanf(yytext, "%lld", &yylval.NUMBER); return NUMBER; } //decimal
"-"[0-9]+	 	{ sscanf(yytext, "%lld", &yylval.SIGN_NUMBER); return SIGN_NUMBER; } //decimal

"0x"[0-9a-fA-F]+ 	{ sscanf(yytext, "%llx", &yylval.NUMBER); return NUMBER; } //hex
"0b"[0-1]+		{ unsigned long long mask = 1; yylval.NUMBER = 0; int i = 1; while(1) if(yytext[i++] == 0) break; i-=2; for(; i > 1; i--, mask <<= 1) yylval.NUMBER |= (yytext[i] == '1') ? mask : 0; return NUMBER; } //binary


"+|"		{ return ADD_BAR_OP; }
"-|"		{ return SUB_BAR_OP; }
"<<"		{ return LTLT_OP; }
">>"		{ return GTGT_OP; }
"=="		{ return EQUEQU_OP; }
">="		{ return GTEQU_OP; }
"<="		{ return LTEQU_OP; }
"!="		{ return NOTEQU_OP; }
"->"		{ return ARROW_OP; }
"<->"		{ return SWAP_OP; }
"&&"		{ return ANDAND_OP; }
"||"		{ return OROR_OP; }
"SIZEOF"	{ return SIZEOF_OP; }
"::"		{ return COLCOL_OP; }
"..."		{ return DOTDOTDOT_OP; }
"EXTERN"	{ return EXTERN_OP; }
"GLOBAL"	{ return GLOBAL_OP; }
"VOID"		{ return VOID_OP; }
"BYTE"		{ return BYTE_OP; }
"UBYTE"		{ return UBYTE_OP; }
"WORD"		{ return WORD_OP; }
"UWORD"		{ return UWORD_OP; }
"LONG"		{ return LONG_OP; }
"ULONG"		{ return ULONG_OP; }
"INT"		{ return LONG_OP; }
"UINT"		{ return ULONG_OP; }
"QUAD"		{ return QUAD_OP; }
"UQUAD"		{ return UQUAD_OP; }
"SINGLE"	{ return SINGLE_OP; }
"DOUBLE"	{ return DOUBLE_OP; }
"STRUCT"	{ return STRUCT_OP; }
"ENUM"		{ return ENUM_OP; }
"CONST"		{ return CONST_OP; }
"SHARED"	{ return SHARED_OP; }
"IF"		{ return IF_OP; }
"ELSE"		{ return ELSE_OP; }
"WHILE"		{ return WHILE_OP; }
"DO"		{ return DO_OP; }
"FOR"		{ return FOR_OP; }
"BREAK"		{ return BREAK_OP; }
"CONTINUE"	{ return CONTINUE_OP; }
"SWITCH"	{ return SWITCH_OP; }
"CASE"		{ return CASE_OP; }
"DEFAULT"	{ return DEFAULT_OP; }
"RETURN"	{ return RETURN_OP; }
"GOTO"		{ return GOTO_OP; }
"INCLUDE"	{ return INCLUDE_OP; }
"IFDEF"		{ return IFDEF_OP; }
"IFNDEF"	{ return IFNDEF_OP; }
"THEN"		{ return THEN_OP; }
"DEFINE"	{ return DEFINE_OP; }
"UNDEF"		{ return UNDEF_OP; }
"NOTHING"	{ return NOTHING_OP; }


[a-zA-Z][a-zA-Z0-9_]*	{ yylval.IDENT = strdup(yytext); return IDENT; }
[a-zA-Z_][a-zA-Z0-9_]+	{ yylval.IDENT = strdup(yytext); return IDENT; }


%%


void binscan(const char* textIn, unsigned long long* numberOut){

	unsigned long long mask = 1;

	*numberOut = 0;
	int i = 0;	

	while(1)
		if(textIn[i++] == 0)
			break;

	--i;
	for(; i > 1; i--, mask <<= 1){
		
		*numberOut |= (textIn[i] == '1') ? mask : 0;

	}

	return;
}

//int yywarp(void) {return 1;}











