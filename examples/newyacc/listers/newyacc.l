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

^\%token				{  return(TOKEN); }
^\%tag					{ return(TAG); }
^\%[a-z][a-z]*				{  return(RWORD); }
\%\{					{ return(LCURL); }
\%\}					{ return(RCURL); }
\/\*					{ BEGIN COMMENT; }
<COMMENT>[^\*]+\*			{ if(input() != '/') yymore();
					  else BEGIN 0; }
\<					{  return('<'); }
\>					{  return('>'); }
\#[1-9][0-9]*				{ return(OPENITEM); }
\&[1-9][0-9]*				{ return(SELECTIVEITEM); }
\@					{ return('@'); }
\$					{ return('$'); }
\!					{ return('!'); }
\=					{ return(ASSIGN); }
\+\=					{ return(CONCAT); }
\+					{ return('+'); }
[0-9][0-9]*				{  return(NUMBER); }
\"[^\"\n\r]*				{ if(yytext[yyleng-1] == '\\')
					    yymore();
					  else {
					    if(input() != '\"') return(ERROR);
					    return(STRING); } }
\%prec[ \t\r\n][ \t\r\n]*[A-Za-z][$_A-Za-z0-9]* {  return(PREC); }
\%prec[ \t\r\n][ \t\r\n]*'[^'\n\r]'	{  return(PREC); }
\%prec[ \t\r\n][ \t\r\n]*'\\[^'\n\r]'	{  return(PREC); }
\%\%					{ return(MARK); }
\%union                                 { return(UNION); }
\%start                                 { return(START); }
IF					{ return(IFR); }
\&\&					{ return(ANDAND); }
\|\|					{ return(OROR); }
THEN					{ return(THENR); }
ELSE					{ return(ELSER); }
\~					{ return(NOT); }
\=\=					{ return(EQUAL); }
\!\=					{ return(NOTEQUAL); }
[_\.$A-Za-z][$_\.A-Za-z0-9]*		{ return(IDENTIFIER); }
[_\.$A-Za-z][$\._A-Za-z0-9]*[ \t\r\n]*:	{ return(C_IDENTIFIER);  }
\{					{ return(LSQUIGGLY); }
\}					{ return(RSQUIGGLY); }
\,					{ return(','); }
'[^'\n\r]				|
'\\[^'\n\r]				{ if(input() != '\'') return(ERROR);
					  return(LITERAL); }
\|					{ return(VBAR); }
\;					{ return(SEMICOLON); }
\[					{ return('['); }
\]					{ return(']'); }
\(					{ return('('); }
\)					{ return(')'); }
\\					{ return('\\'); }
[ \t\r\n]				;

