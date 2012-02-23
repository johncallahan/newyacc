#!/bin/csh -f
onintr icu
sed -e "s/extern short yyerrflag/extern long yylineno; extern short yyerrflag/" \
      -e "s/=yylex/=nylex/" \
      -e "s/--yyerrflag;/--yyerrflag; nyipop();/" \
      -e "s/=yylex/=nylex/" < $1 > $1.$$.temp
mv $1.$$.temp $1
goto cu
icu:
\rm -rf $1 $1.$$.temp
echo "interrupted\!"
cu:
