%option reentrant
%option bison-bridge
%{

/*
 * (C) Copyright 1996-2012 ECMWF.
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

#include "odb_api/odblib_lex.h"

// http://flex.sourceforge.net/manual/Multiple-Input-Buffers.html#Multiple-Input-Buffers

const size_t maxIncludeDepth = 2000;
YY_BUFFER_STATE includeStack[maxIncludeDepth];
int includeStackPtr = 0;

void pushInclude(const std::string& sql, const std::string& yypath, YY_BUFFER_STATE state, odblib_scan_t scanner)
{
    if (includeStackPtr > maxIncludeDepth)
        throw eckit::UserError("Includes nested too deeply");
    includeStack[includeStackPtr++] = state;
//YY_BUFFER_STATE odblib__scan_bytes (odblib_const char *bytes,odblib__size_t len ,odblib_scan_t odblib_scanner );
//void odblib__switch_to_buffer (YY_BUFFER_STATE new_buffer ,odblib_scan_t odblib_scanner );
    odblib__switch_to_buffer(odblib__scan_bytes(sql.c_str(), sql.size(), scanner), scanner);
}

int popInclude(odblib_scan_t scanner)
{
    YY_BUFFER_STATE currentState (includeStack[includeStackPtr]);

    if ( includeStackPtr - 1 <= 0 )
    {
        return 1;
    }
    else
    {
        //odblib__delete_buffer(YY_CURRENT_BUFFER);
        // FIXME: leak (?)
        //odblib__delete_buffer(currentState, scanner);
        odblib__switch_to_buffer(includeStack[--includeStackPtr], scanner);
        return 0;
    }
}


int wrap(odblib_scan_t scanner) {
    //Log::info() << "** wrap **" << std::endl;
    return popInclude(scanner);
}

/*
%option noyywrap
*/

%}
IDENT   [_A-Za-z]+[_0-9A-Za-z]*
VAR     [$][_0-9A-Za-z]+
NUMB    [-]?[0-9]+(\.[0-9]*)?
SEMICOLON [;]


%s LEX_ORDERBY
%s LEX_CREATE
%x incl

%%
#include                          BEGIN(incl);

@[lL][iI][nN][kK]                 return LINK;
[sS][eE][tT]                      return SET;
[dD][aA][tT][aA][bB][aA][sS][eE]  return DATABASE;
[sS][eE][lL][eE][cC][tT]          return SELECT;
[iI][nN][tT][oO]                  return INTO;
[fF][rR][oO][mM]                  return FROM;
[wW][hH][eE][rR][eE]              return WHERE;
[iI][nN][sS][eE][rR][tT]          return INSERT;
[vV][aA][lL][uU][eE][sS]          return VALUES;
[aA][nN][dD]                      return AND;
[oO][rR]                          return OR;
[oO][nN]                          return ON;
[iI][sS]                          return IS;
[nN][uU][lL][lL]                  return NIL;
[nN][oO][tT]                      return NOT;
[cC][oO][uU][nN][tT]              return COUNT;
[eE][xX][iI][tT]                  return EXIT;
[vV][iI][eE][wW]                  { BEGIN 0; return VIEW; }
[cC][rR][eE][aA][tT][eE]          { BEGIN LEX_CREATE; return CREATE; }
[sS][cC][hH][eE][mM][aA]          { BEGIN 0; return SCHEMA; }
<LEX_CREATE>[iI][nN][dD][eE][xX]  { BEGIN 0; return INDEX; }
<LEX_CREATE>[tT][aA][bB][lL][eE]  { BEGIN 0; return TABLE; }
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
[tT][eE][mM][pP][oO][rR][aA][rR][yY] return TEMPORARY;
<LEX_ORDERBY>[aA][sS][cC]         return ASC;
<LEX_ORDERBY>[dD][eE][sS][cC]     return DESC;
{SEMICOLON}	                      { BEGIN 0; return ';'; }
[aA][sS]                          return AS;
\#                                return HASH; 
[cC][oO][nN][sS][tT][rR][aA][iI][nN][tT] return CONSTRAINT;
[uU][nN][iI][qQ][uU][eE]          return UNIQUE;
[pP][rR][iI][mM][aA][rR][yY]      return PRIMARY;
[fF][oO][rR][eE][iI][gG][nN]      return FOREIGN;
[kK][eE][yY]                      return KEY;
[rR][eE][fF][eE][rR][eE][nN][cC][eE][sS] return REFERENCES;
[dD][eE][fF][aA][uU][lL][tT]      return DEFAULT;
[iI][nN][hH][eE][rR][iI][tT][sS]  { BEGIN 0; return INHERITS; }
[mM][aA][tT][cC][hH]              return MATCH;
[qQ][uU][eE][rR][yY]              return QUERY;
[lL][iI][kK][eE]                  return LIKE;
[rR][lL][iI][kK][eE]              return RLIKE;
<incl>[ \t]*                      /* eat the whitespace */
<incl>[^ \t\n]+ {                 /* name of the file to be included */
    //cerr << "include " << yytext << std::endl;

    eckit::PathName fileName(StringTool::unQuote(yytext));

    //cerr << "Opening " << fileName << std::endl;
    std::string text(StringTool::readFile(fileName));

    pushInclude(text, fileName, YY_CURRENT_BUFFER, odblib_scanner);
    BEGIN(INITIAL);
}

<<EOF>> { 
    //cerr << "** EOF **" << std::endl;
    odblib_terminate();
}

\"|\'   {
            int c, end = yytext[0];

            yyleng = 0;
            while ((c = yyinput(yyscanner)) && c != end)
            {
                yytext[yyleng++] = (c == '\\') ? yyinput(yyscanner) : c;
                // TODO: FIXME:
                //if (c == '\n') 
                //    ++SQLYacc::odblib_lineno;
            }

            yytext[yyleng++] = 0;
            yylval->val = yytext;
            return STRING;
        }

\{      {
            int c, end = '}';

            yyleng = 0;
            while ((c = yyinput(yyscanner)) && c != end)
            {
                yytext[yyleng++] = (c == '\\') ? yyinput(yyscanner) : c;
                // TODO: FIXME:
                //if (c == '\n') 
                //    ++SQLYacc::odblib_lineno;
            }

            yytext[yyleng++] = 0;
            yylval->val = yytext;
            return EMBEDDED_CODE;
        }

{IDENT}	{ 
			char *p = yytext; while(*p) { *p = tolower(*p); p++; }
			yylval->val = yytext; 
			//cerr << "****I " << yylval.val << " ****" << std::endl; 
			return IDENT; 
		}
{VAR}   { 
			char *p = yytext; while(*p) { *p = tolower(*p); p++; }
			yylval->val = yytext; 
			//cerr << "****V " << yylval.val << " ****" << std::endl; 
			return VAR; 
		}

{NUMB}	{
			yylval->num = atof(yytext);
			//cerr << "****N " << yylval.num << " ****" << std::endl; 
			return DOUBLE;
		}

"--".*  ;
"//".*  ;

[ \t]*  ;
\n      ; //++SQLYacc::odblib_lineno;

"="       return EQ;
"=="      return EQ;
"!="      return NE;
"<>"      return NE;
">="      return GE;
"<="      return LE;
"!"       return NOT;

.		return *yytext;

%%
