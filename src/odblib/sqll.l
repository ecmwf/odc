%{

/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef YYBISON
/* FLEX */
	#undef YY_INPUT
	#define YY_INPUT(buf,result,max_size) \
	{ \
		int n = (max_size < (inputEnd - inputText)) ? max_size : (inputEnd - inputText); \
		if (n > 0) { \
			memcpy(buf, inputText, n); \
			inputText += n; \
		} \
		result = (n == 0) ? YY_NULL : n; \
	}
/* LEX */
#else
	extern char* inputText;
	#undef input
	#undef unput
	#define input() (*inputText++)
	#define unput(c) (*--inputText = c)
#endif

%}
IDENT   [_A-Za-z]+[_0-9A-Za-z]*
VAR     [$][_0-9A-Za-z]+
NUMB    [-]?[0-9]+(\.[0-9]*)?
SEMICOLON [;]

%s LEX_ORDERBY
%s LEX_CREATE

%%
@[lL][iI][nN][kK]                 return LINK;
[sS][eE][tT]                      return SET;
[dD][aA][tT][aA][bB][aA][sS][eE]  return DATABASE;
[sS][eE][lL][eE][cC][tT]          return SELECT;
[iI][nN][tT][oO]                  return INTO;
[fF][rR][oO][mM]                  return FROM;
[wW][hH][eE][rR][eE]              return WHERE;
[aA][nN][dD]                      return AND;
[oO][rR]                          return OR;
[iI][sS]                          return IS;
[nN][uU][lL][lL]                  return NIL;
[nN][oO][tT]                      return NOT;
[cC][oO][uU][nN][tT]              return COUNT;
[eE][xX][iI][tT]                  return EXIT;
[vV][iI][eE][wW]                  { BEGIN 0; return VIEW; }
[cC][rR][eE][aA][tT][eE]          { BEGIN LEX_CREATE; return CREATE; }
<LEX_CREATE>[iI][nN][dD][eE][xX]  { BEGIN 0; return INDEX; }
[tT][aA][bB][lL][eE]              { BEGIN 0; return TABLE; }
<LEX_CREATE>[tT][yY][pP][eE]      { BEGIN 0; return TYPE; }
[tT][yY][pP][eE][dD][eE][fF]      { BEGIN 0; return TYPEDEF; }
[tT][yY][pP][eE][oO][fF]          return TYPEOF;
[bB][eE][tT][wW][eE][eE][nN]      return BETWEEN;
[dD][iI][sS][tT][iI][nN][cC][tT]  return DISTINCT;
[aA][lL][lL]                      return ALL;
[mM][iI][sS][sS][iI][nN][gG]      return NIL;
[gG][rR][oO][uU][pP]              return GROUP;
[oO][rR][dD][eE][rR]              { BEGIN LEX_ORDERBY; return ORDER; }
[bB][yY]                          return BY;
[iI][nN]                          return IN;
[rR][eE][aA][dD][oO][nN][lL][yY]  return READONLY;
[uU][pP][dD][aA][tT][eE][dD]      return UPDATED;
[nN][oO][rR][eE][oO][rR][dD][eE][rR] return NOREORDER;
[sS][aA][fF][eE][gG][uU][aA][rR][dD] return SAFEGUARD;
<LEX_ORDERBY>[aA][sS][cC]         return ASC;
<LEX_ORDERBY>[dD][eE][sS][cC]     return DESC;
{SEMICOLON}	                      { BEGIN 0; return ';'; }
[aA][sS]                          return AS;

\"|\'  {
			int c,q = yytext[0];

			yyleng = 0;

			while((c = yyinput()) && c != q && c != '\n')
			{
               if(c == '\\') yytext[yyleng++] = yyinput();
               else yytext[yyleng++] =  c;
			}

            yytext[yyleng++] = 0;
            yylval.val = yytext;
			//cerr << "**** lex: STRING='" << yytext << "'" << endl;
            return STRING;
        }

{IDENT}	{ 
			char *p = yytext; while(*p) { *p = tolower(*p); p++; }
			yylval.val = yytext; 
			//cerr << "****I " << yylval.val << " ****" << endl; 
			return IDENT; 
		}
{VAR}   { 
			char *p = yytext; while(*p) { *p = tolower(*p); p++; }
			yylval.val = yytext; 
			//cerr << "****V " << yylval.val << " ****" << endl; 
			return VAR; 
		}

{NUMB}	{
			yylval.num = atof(yytext);
			//cerr << "****N " << yylval.num << " ****" << endl; 
			return DOUBLE;
		}

\#      {
           int c;
           while((c = yyinput()) && (c != '\n'))
                    ;
        }
"--".*  ;
"//".*  ;

[ \t]*  ;
\n      ++yylineno;

"="       return EQ;
"=="      return EQ;
"!="      return NE;
"<>"      return NE;
">="      return GE;
"<="      return LE;
"!"       return NOT;

.		return *yytext;

%%
