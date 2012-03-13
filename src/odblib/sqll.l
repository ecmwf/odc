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
#ifdef linux
#define YY_INPUT(buf,result,max_size) \
  { \
	int c = fgetc(yyin); \
	result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); \
   }

#endif
%}
IDENT   [_A-Za-z]+[_0-9A-Za-z]*
VAR     [$][_0-9A-Za-z]+
NUMB    [-]?[0-9]+(\.[0-9]*)?
SEMICOLON [;]

%s LEX_ORDERBY
%s LEX_CREATE

%%
"@LINK"|"@link" return LINK;
"set"     return SET;
"SET"      return SET;
"database" return DATABASE;
"DATABASE" return DATABASE;
"select"   return SELECT;
"SELECT"   return SELECT;
"into"     return INTO;
"INTO"     return INTO;
"from"     return FROM;
"FROM"     return FROM;
"where"    return WHERE;
"WHERE"    return WHERE;
"and"      return AND;
"AND"      return AND;
"or"      return OR;
"OR"      return OR;
"is"      return IS;
"IS"     return IS;
"null"    return NIL;
"NULL"    return NIL;
"not"    return NOT;
"NOT"    return NOT;
"count"    return COUNT;
"COUNT"    return COUNT;

"exit"    return EXIT;
"EXIT"    return EXIT;

"view"    { BEGIN 0; return VIEW; }
"VIEW"    { BEGIN 0; return VIEW; }

"create"|"CREATE"   { BEGIN LEX_CREATE; return CREATE; }

<LEX_CREATE>"index"|"INDEX"    { BEGIN 0; return INDEX; }

"table"|"TABLE"    { BEGIN 0; return TABLE; }

<LEX_CREATE>"TYPE"|"type"    { BEGIN 0; return TYPE; }
"TYPEDEF"|"typedef"    { BEGIN 0; return TYPEDEF; }

"TYPEOF"|"typeof"    { return TYPEOF; }

"between"    return BETWEEN;
"BETWEEN"    return BETWEEN;

"distinct"   return DISTINCT;
"DISTINCT"   return DISTINCT;
"all"        return ALL;
"ALL"        return ALL;

"missing"    return NIL;
"MISSING"    return NIL;

"group"|"GROUP" return GROUP;

"order"|"ORDER"    { BEGIN LEX_ORDERBY; return ORDER; }

"by"    return BY;
"BY"    return BY;

"in"    return IN;
"IN"    return IN;

"READONLY"    return READONLY;
"readonly"    return READONLY;
"UPDATED"    return UPDATED;
"updated"    return UPDATED;
"NOREORDER"    return NOREORDER;
"noreorder"    return NOREORDER;
"SAFEGUARD"    return SAFEGUARD;
"safeguard"    return SAFEGUARD;

<LEX_ORDERBY>"asc"    return ASC;
<LEX_ORDERBY>"ASC"    return ASC;

<LEX_ORDERBY>"desc"    return DESC;
<LEX_ORDERBY>"DESC"    return DESC;

{SEMICOLON}	{ BEGIN 0; return ';'; }

"as"|"AS"    return AS;

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
\n      ;

"="       return EQ;
"=="      return EQ;
"!="      return NE;
"<>"      return NE;
">="      return GE;
"<="      return LE;
"!"       return NOT;

.		return *yytext;

%%
