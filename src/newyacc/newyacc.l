%START COMMENT
a	[Aa]
b	[Bb]
c	[Cc]
d	[Dd]
e	[Ee]
f	[Ff]
g	[Gg]
h	[Hh]
i	[Ii]
j	[Jj]
k	[Kk]
l	[Ll]
m	[Mm]
n	[Nn]
o	[Oo]
p	[Pp]
q	[Qq]
r	[Rr]
s	[Ss]
t	[Tt]
u	[Uu]
v	[Vv]
w	[Ww]
x	[Xx]
y	[Yy]
z	[Zz]

%%

^\%token				{ ECHO; return(TOKEN); }
^\%tag					{ return(TAG); }
^\%[a-z][a-z]*				{ ECHO; return(RWORD); }
\%\{					{ return(LCURL); }
\%\}					{ return(RCURL); }
\/\*					{ BEGIN COMMENT; }
<COMMENT>[^\*]+\*			{ if(input() != '/') yymore();
					  else BEGIN 0; }
\<					{ ECHO; return('<'); }
\>					{ ECHO; return('>'); }
\#[1-9][0-9]*				{ yylval.num = atoi(yytext+1);
					  return(OPENITEM); }
\&[1-9][0-9]*				{ yylval.num = atoi(yytext+1);
					  return(SELECTIVEITEM); }
\@					{ return('@'); }
\$					{ return('$'); }
\!					{ return('!'); }
\=					{ return(ASSIGN); }
\+\=					{ return(CONCAT); }
\+					{ return('+'); }
[0-9][0-9]*				{ ECHO; return(NUMBER); }
\"[^\"\n\r]*				{ if(yytext[yyleng-1] == '\\')
					    yymore();
					  else {
					    if(input() != '\"') return(ERROR);
					    yylval.str = yytext+1;
					    return(STRING); } }
\%prec[ \t\r\n][ \t\r\n]*[A-Za-z][$_A-Za-z0-9]* { ECHO; return(PREC); }
\%prec[ \t\r\n][ \t\r\n]*'[^'\n\r]'	{ ECHO; return(PREC); }
\%prec[ \t\r\n][ \t\r\n]*'\\[^'\n\r]'	{ ECHO; return(PREC); }
\%\%					{ return(MARK); }
\%union                                 { return(UNION); }
\%start                                 { return(START); }
IF					{ return(IF); }
\&\&					{ return(ANDAND); }
\|\|					{ return(OROR); }
THEN					{ return(THEN); }
ELSE					{ return(ELSE); }
\~					{ return(NOT); }
\=\=					{ return(EQUAL); }
\!\=					{ return(NOTEQUAL); }
[_\.$A-Za-z][$_\.A-Za-z0-9]*		{ yylval.str=NEW_STR(strlen(yytext)+1);
					  strcpy(yylval.str,yytext);
					  if(ny_bounce && !ny_displays) ECHO;
					  if(ny_displays) ny_AddDisplay(yytext,0);
					  return(IDENTIFIER); }
[_\.$A-Za-z][$\._A-Za-z0-9]*[ \t\r\n]*:	{ yylval.str = yytext;
					  return(C_IDENTIFIER);  }
\{					{ return(LSQUIGGLY); }
\}					{ return(RSQUIGGLY); }
\,					{ if(ny_bounce) ECHO; return(','); }
'[^'\n\r]				|
'\\[^'\n\r]				{ if(input() != '\'') return(ERROR);
					  yylval.str = yytext;
					  return(LITERAL); }
\|					{ return(VBAR); }
\;					{ return(SEMICOLON); }
\[					{ return('['); }
\]					{ return(']'); }
\(					{ return('('); }
\)					{ return(')'); }
\\					{ return('\\'); }
[ \t\r\n]				{ ECHO; }
