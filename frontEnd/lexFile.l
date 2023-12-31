%option caseless
%option noyywrap



%%

[ \r\t]+	 	{ continue; }

[\n]			{ IN_COM = 0; GLOBAL_LINE_NUMBER++; continue; }

"\""[a-zA-Z0-9_]+"\"" { if(IN_COM) continue; yylval.STRING_LIT = strdup(yytext); return STRING_LIT; }


[0-9]+"."[0-9]+  	{ if(IN_COM) continue; yylval.FLOAT = strdup(yytext); return FLOAT; } //float
"-"[0-9]+"."[0-9]+  	{ if(IN_COM) continue; yylval.FLOAT = strdup(yytext); return FLOAT; } //float

[0-9]+	 		{ if(IN_COM) continue; yylval.DECIMAL = strdup(yytext); return DECIMAL; } //decimal
"-"[0-9]+	 	{ if(IN_COM) continue; yylval.DECIMAL = strdup(yytext); return DECIMAL; } //decimal

"0x"[0-9a-fA-F]+ 	{ if(IN_COM) continue; yylval.HEX = strdup(yytext); return HEX; } //hex
"0b"[0-1]+		{ if(IN_COM) continue; yylval.BINARY = strdup(yytext); return BINARY; } //binary

"//"		{ IN_COM = 1; continue; }

"/*"		{ IN_COM = 1; continue; }
"*/"		{ IN_COM = 0; continue; }


"?"			{ if(IN_COM) continue; return QUESTION_OP; }
"~"			{ if(IN_COM) continue; return TILDE_OP; }
"+"			{ if(IN_COM) continue; return PLUS_OP; }
"-"			{ if(IN_COM) continue; return DASH_OP; }
"*"			{ if(IN_COM) continue; return SPLAT_OP; }
"/"			{ if(IN_COM) continue; return SLASH_OP; }
"#"			{ if(IN_COM) continue; return HASH_OP; }
"!"			{ if(IN_COM) continue; return BANG_OP; }
":"			{ if(IN_COM) continue; return COLON_OP; }
"|"			{ if(IN_COM) continue; return BAR_OP; }
"&"			{ if(IN_COM) continue; return AMPER_OP; }
"^"			{ if(IN_COM) continue; return CARROT_OP; }
"["			{ if(IN_COM) continue; return OPENSQUARE_OP; }
"]"			{ if(IN_COM) continue; return CLOSESQUARE_OP; }
"<"			{ if(IN_COM) continue; return OPENCHEV_OP; }
">"			{ if(IN_COM) continue; return CLOSECHEV_OP; }
"("			{ if(IN_COM) continue; return OPENPAR_OP; }
")"			{ if(IN_COM) continue; return CLOSEPAR_OP; }
"{"			{ if(IN_COM) continue; return OPENSEMI_OP; }
"}"			{ if(IN_COM) continue; return CLOSESEMI_OP; }
"\\"		{ if(IN_COM) continue; return BACKSLASH_OP; }
","			{ if(IN_COM) continue; return COMMA_OP; }
"%"			{ if(IN_COM) continue; return PERCENT_OP; }

";"			{ if(IN_COM) continue; return SEMICOLON_OP; }
"="			{ if(IN_COM) continue; return EQUALSIGN_OP; }

"+|"		{ if(IN_COM) continue; return ADD_BAR_OP; }
"-|"		{ if(IN_COM) continue; return SUB_BAR_OP; }
"<<"		{ if(IN_COM) continue; return LTLT_OP; }
">>"		{ if(IN_COM) continue; return GTGT_OP; }
"=="		{ if(IN_COM) continue; return EQUEQU_OP; }
">="		{ if(IN_COM) continue; return GTEQU_OP; }
"<="		{ if(IN_COM) continue; return LTEQU_OP; }
"!="		{ if(IN_COM) continue; return NOTEQU_OP; }
"&&"		{ if(IN_COM) continue; return ANDAND_OP; }
"||"		{ if(IN_COM) continue; return OROR_OP; }
"EXTERN"	{ if(IN_COM) continue; return EXTERN_OP; }
"GLOBAL"	{ if(IN_COM) continue; return GLOBAL_OP; }
"VOID"		{ if(IN_COM) continue; return VOID_OP; }
"BYTE"		{ if(IN_COM) continue; return BYTE_OP; }
"WORD"		{ if(IN_COM) continue; return WORD_OP; }
"LONG"		{ if(IN_COM) continue; return LONG_OP; }
"INT"		{ if(IN_COM) continue; return LONG_OP; }
"QUAD"		{ if(IN_COM) continue; return QUAD_OP; }
"SINGLE"	{ if(IN_COM) continue; return SINGLE_OP; }
"DOUBLE"	{ if(IN_COM) continue; return DOUBLE_OP; }
"CONST"		{ if(IN_COM) continue; return CONST_OP; }
"STORED"	{ if(IN_COM) continue; return SHARED_OP; }
"STRD"		{ if(IN_COM) continue; return SHARED_OP; }
"IF"		{ if(IN_COM) continue; return IF_OP; }
"ELSE"		{ if(IN_COM) continue; return ELSE_OP; }
"WHILE"		{ if(IN_COM) continue; return WHILE_OP; }
"DO"		{ if(IN_COM) continue; return DO_OP; }
"FOR"		{ if(IN_COM) continue; return FOR_OP; }
"BREAK"		{ if(IN_COM) continue; return BREAK_OP; }
"CONTINUE"	{ if(IN_COM) continue; return CONTINUE_OP; }
"RETURN"	{ if(IN_COM) continue; return RETURN_OP; }
"GOTO"		{ if(IN_COM) continue; return GOTO_OP; }
"INCLUDE"	{ if(IN_COM) continue; return INCLUDE_OP; }
"IFDEF"		{ if(IN_COM) continue; return IFDEF_OP; }
"IFNDEF"	{ if(IN_COM) continue; return IFNDEF_OP; }
"THEN"		{ if(IN_COM) continue; return THEN_OP; }
"DEFINE"	{ if(IN_COM) continue; return DEFINE_OP; }
"UNDEF"		{ if(IN_COM) continue; return UNDEF_OP; }
"NOTHING"	{ if(IN_COM) continue; return NOTHING_OP; }
"BLOCK"		{ if(IN_COM) continue; return BLOCK_OP; }


[a-zA-Z][a-zA-Z0-9_]*	{ if(IN_COM) continue; yylval.IDENT = strdup(yytext); return IDENT; }


%%












