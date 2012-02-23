#!/bin/csh -f
onintr icu
sed -e "s/getc(yyin)/(yycharno++,getc(yyin))/" \
       -e "s/yylineno++/yylineno++,yybline=yycharno/" \
       -e "s/extern int yylineno/extern long yycharno,yytok,yybline;extern long yylineno/" \
       -e "s/int yylineno =1/extern long yycharno,yytok,yybline,yylineno/" \
       -e "s/if (\!yymorfg)/yytok=yycharno+1;&/" < $1 > $1.$$.temp
mv $1.$$.temp $1
goto cu
icu:
\rm -rf $1 $1.$$.temp
echo "interrupted\!"
cu:

