extern char yytext[];

nylex()
{
  int q;
  q = yylex();
  nypushs(yytext);
  return(q);
}
