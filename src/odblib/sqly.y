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

#define YYMAXDEPTH 500
#include "eckit/machine.h"

using namespace SQLYacc;

typedef odb::sql::expression::SQLExpression* SQLExpressionPtr; // For casts.

struct YYSTYPE {
	SQLExpression         *exp;
	SQLTable              *table;
	double                 num;
	string                 val;
	vector<string>         list;
	Expressions            explist;
	pair<SQLExpression*,SQLExpression*> ass;
	Dictionary				dic;
	pair<SQLExpression*,bool> orderexp;
	pair<Expressions,vector<bool> > orderlist;
	vector<SQLTable*>      tablist;
	ColumnDefs             coldefs;
	ColumnDef              coldef;
	Range                  r;
	bool                   bol;
};

#ifdef YYBISON
#define YYSTYPE_IS_DECLARED
int yylex(); 
//int yylex(YYSTYPE*, void*);
//int yylex ( YYSTYPE * lvalp, YYLTYPE * llocp, yyscan_t scanner );
//int yydebug;
#endif

extern "C" int isatty(int);

Expressions emptyExpressionList;

%}

%token <val>STRING
%token <val>IDENT
%token <val>VAR

%token <num>DOUBLE

%token SEMICOLON
%token LINK
%token TYPEOF
%token READONLY
%token UPDATED
%token NOREORDER
%token SAFEGUARD

%token EXIT

%token SELECT
%token INTO
%token FROM
%token SET
%token DATABASE
%token COUNT
%token WHERE
%token GROUP
%token ORDER
%token BY

%token CREATE
%token VIEW
%token INDEX
%token TABLE
%token TYPE
%token TYPEDEF

%token EQ
%token GE
%token LE
%token NE
%token IN
%token NOT
%token AND
%token OR
%token IS
%token AS
%token NIL
%token ALL
%token DISTINCT
%token BETWEEN

%token ASC
%token DESC

%token HASH

%type <exp>column
%type <exp>vector_index 

%type <exp>condition;
%type <exp>atom_or_number;
%type <exp>expression;
%type <exp>expression_ex;

%type <exp>factor;
%type <exp>power;
%type <exp>term;
%type <exp>conjonction;
%type <exp>disjonction;
%type <exp>optional_hash;

%type <exp>where;

%type <exp>assignment_rhs;
%type <exp>vector;
%type <exp>dictionary;
%type <ass>association;
%type <dic>association_list;
%type <explist>expression_list;
%type <explist>expression_list_ex;

%type <tablist>table_list;
%type <table>table
%type <val>table_reference;
%type <tablist>from;

%type <explist>group_by;

%type <orderlist>order_by;
%type <orderlist>order_list;
%type <orderexp>order;

%type <explist>select_list;
%type <explist>select_list_;
%type <exp>select;
%type <exp>select_;

%type <bol>distinct;
%type <val>into;
%type <val>func;

%type <r>vector_range_decl;
%type <val>column_name;
%type <val>bitfield_ref;
%type <coldefs>column_def_list;
%type <coldefs>column_def_list_;
%type <coldef>column_def;
%type <coldefs>bitfield_def_list;
%type <coldefs>bitfield_def_list_;
%type <coldef>bitfield_def;
%type <val>data_type;
%%

start : statements { SQLSession::current().currentDatabase().setLinks(); }
	;

statements : statement 
		   | statements statement
		   ;

statement: select_statement 
		 | set_statement
		 | create_view_statement
		 | create_index_statement 
		 | create_type_statement
		 | create_table_statement
		 | readonly_statement
		 | updated_statement
		 | noreorder_statement
		 | safeguard_statement
		 | exit_statement
	     | error
	;

exit_statement: EXIT ';' { /*delete &SQLSession::current();*/ return 0; }
	;

readonly_statement: READONLY ';'
	;

updated_statement: UPDATED ';'
	;

noreorder_statement: NOREORDER ';'
	;

safeguard_statement: SAFEGUARD ';'
	;

create_index_statement: CREATE INDEX IDENT TABLE IDENT ';'
	{
		SQLSession& s  = SQLSession::current();
		s.createIndex($3,$5);
	}
	| CREATE INDEX IDENT '@' IDENT ';'
	{
		SQLSession& s  = SQLSession::current();
		s.createIndex($3,$5);
	}
	;

create_type_statement: create_type IDENT as_or_eq '(' bitfield_def_list ')' ';'
	{
		string		typeName = $2;
		ColumnDefs	colDefs = $5;
		FieldNames	fields;
		Sizes		sizes;
		for (ColumnDefs::size_type i = 0; i < colDefs.size(); i++) {
			string name = colDefs[i].name();
			string memberType = colDefs[i].type();

			fields.push_back(name);

			int size = atof(memberType.c_str() + 3); // bit[0-9]+
			ASSERT(size > 0);

			sizes.push_back(size);
		}
		string typeSignature = type::SQLBitfield::make("Bitfield", fields, sizes, typeName.c_str());

		SQLSession::current()
		.currentDatabase()
		.schemaAnalyzer().addBitfieldType(typeName, fields, sizes, typeSignature);
		
		//cout << "CREATE TYPE " << typeName << " AS " << typeSignature << ";" << endl;		
	}
	;

create_type_statement: create_type IDENT as_or_eq IDENT ';' { type::SQLType::createAlias($4, $2); }
	;

create_type: CREATE TYPE
           | TYPEDEF
           ;

as_or_eq: AS
        | EQ
        ;

bitfield_def_list: bitfield_def_list_ { $$ = $1; }
                 | bitfield_def_list_ ',' { $$ = $1; }
	;

bitfield_def_list_: bitfield_def { $$ = ColumnDefs(1, $1); }
                  | bitfield_def_list_ ',' bitfield_def { $$ = $1; $$.push_back($3); }
	;

bitfield_def: column_def
	{
		/* Type should look like bit[0-9]+ '*/
		$$ = $1;
	}
	;

create_table_statement: CREATE TABLE IDENT AS '(' column_def_list ')' ';'
	{
		string tableName = $3;
		ColumnDefs cols = $6;

		TableDef tableDef(tableName, cols);
		SQLSession& s  = SQLSession::current();
		s.currentDatabase().schemaAnalyzer().addTable(tableDef);

		//cout << "CREATE TABLE " << tableName << endl;

		//SQLCreateTable ct(tableName, cols);
		//ct.execute();
	}
	;

column_def_list: column_def_list_     { $$ = $1; }
               | column_def_list_ ',' { $$ = $1; }
               ;
	 
column_def_list_: column_def                      { $$ = ColumnDefs(1, $1); }
                | column_def_list_ ',' column_def { $$ = $1; $$.push_back($3); }
                ;

column_def: column_name vector_range_decl data_type
	{
		//cout << "ColumnDef: " << $1 << "," << $3 << "," << $2.first << "-" << $2.second << endl;
		$$ = ColumnDef($1, $3, $2);
	}
	;

vector_range_decl: '[' DOUBLE ']'            { $$ = make_pair(1, $2); }
                 | '[' DOUBLE ':' DOUBLE ']' { $$ = make_pair($2, $4); }
                 | empty                     { $$ = make_pair(0, 0); }
                 ;

column_name: IDENT { $$ = $1; }
           ;

data_type: IDENT                 { $$ = $1; }
         | LINK                  { $$ = "@LINK"; } 
         | TYPEOF '(' column ')' { $$ = ($3)->type()->name(); }
         ;

create_view_statement: CREATE VIEW IDENT AS select_statement
	;

opt_semicolon: ';' | empty
	;

select_statement: SELECT distinct select_list into from where group_by order_by opt_semicolon
					{   
						bool                   distinct($2);
						Expressions            select_list($3);
						string                 into($4);
						vector<SQLTable*>      from($5);
						odb::sql::expression::SQLExpression *where($6);
						Expressions            group_by($7);
						pair<Expressions,vector<bool> >      order_by($8);

						SQLSelect* sqlSelect = SQLSelectFactory::instance()
							.create(distinct, select_list, into, from, where, group_by, order_by);
						SQLSession::current().statement(sqlSelect);
					}
                ;

distinct: DISTINCT { $$ = true; }
        | ALL      { $$ = false; }
		| empty    { $$ = false; }
		;

into: INTO IDENT   { $$ = $2; }
    | INTO STRING  { $$ = $2; }
    | empty        { $$ = ""; }
    ;

from : FROM table_list { $$ = $2; }
	 | empty           { $$ = vector<SQLTable*>(); }
	 ;

where : WHERE expression { $$ = $2; }
	  |                  { $$ = 0; } 
	  ;

vector	: '[' expression_list_ex ']' { $$ = new Expressions($2); }
		;

dictionary: '{' association_list '}' { $$ = new Dictionary($2); }
		  ;

assignment_rhs	: expression_ex
		;

association: expression ':' expression_ex  { $$ = make_pair($1, $3); }
		   ;

association_list: association { Dictionary d; d[($1 .first)->title()] = $1 .second; $$ = d; }
				| association_list ',' association { $$ = $1; $$[($3 .first)->title()] = $3 .second; }
				| empty { $$ = Dictionary(); }
				;

set_statement : SET DATABASE STRING ';' { SQLSession::current().openDatabase($3); }
	; 

set_statement : SET DATABASE STRING AS IDENT ';' { SQLSession::current().openDatabase($3,$5); }
	; 

set_statement : SET VAR EQ assignment_rhs ';'
	{ 
		//cout << "== set variable " << $2 << " to ";
		//if ($4) cout << *($4) << endl; else cout << "NULL" << endl;
		SQLSession::current().currentDatabase().setVariable($2, $4);
	}
	; 

bitfield_ref: '.' IDENT  { $$ = $2; }
			|            { $$ = string(); }

column: IDENT vector_index table_reference optional_hash
		  {
			std::string columnName      ($1);
			std::string bitfieldName    ;
			SQLExpression* vectorIndex  ($2);
			std::string table           ($3);
			SQLExpression* pshift       ($4);

			$$ = SQLSelectFactory::instance().createColumn(columnName, bitfieldName, vectorIndex, table, pshift);
		  }
	   | IDENT bitfield_ref table_reference optional_hash
		{
			std::string columnName      ($1);
			std::string bitfieldName    ($2);
			SQLExpression* vectorIndex  (0); 
			std::string table           ($3);
			SQLExpression* pshift       ($4);

			$$ = SQLSelectFactory::instance().createColumn(columnName, bitfieldName, vectorIndex, table, pshift);
		 }
	  ;

vector_index : '[' expression ']'    { $$ = $2; }
             | empty                 { $$ = NULL; }
             ;

table_reference: '@' IDENT   { $$ = string("@") + $2; }
               | empty       { $$ = string(""); }
               ;

table : IDENT '.' IDENT { SQLSession& s  = SQLSession::current(); $$ = s.findTable($1,$3); }
	  | IDENT           { SQLSession& s  = SQLSession::current(); $$ = s.findTable($1); }
	  | STRING			{ SQLSession& s  = SQLSession::current(); $$ = s.findFile($1); }
	  ;

table_list : table                  { $$ = vector<SQLTable*>(1,$1); }
	       | table_list  ',' table  { $$ = $1; $$.push_back($3); }
	       ;

/*================= SELECT =========================================*/

select_list: select_list_     { $$ = $1; }
           | select_list_ ',' { $$ = $1; }
           ;

select_list_ : select         { $$ = Expressions(1, $1); }
	| select_list_ ',' select { $$ = $1; $$.push_back($3); }
	;

select: select_ access_decl { $$ = $1; }
      ;

select_: '*' table_reference                              { $$ = new ColumnExpression("*", $2);  } 
	   | IDENT '.' '*'   table_reference                   { $$ = new BitColumnExpression($1, "*", $4); }
	   | IDENT '[' expression ':' expression ']' table
		{
			// TODO: Add simillar rule for BitColumnExpression.
			bool missing = false;
			int begin = $3->eval(missing); //ASSERT(!missing);
			int end = $5->eval(missing); //ASSERT(!missing);
			$$ = new ColumnExpression($1, $7, begin, end);
		}
	   | expression AS IDENT table_reference { $$ = $1; $$->title($3 + $4); }
	   | expression
	   ;

access_decl: UPDATED
           | READONLY
           | empty
           ;

/*================= GROUP BY ======================================*/
group_by: GROUP BY expression_list { $$ = $3;                       }
        | empty                    { $$ = Expressions(); }
	    ;

/*================= ORDER =========================================*/

order_by: ORDER BY order_list { $$ = $3;                       }
        | empty               { $$ = make_pair(Expressions(),vector<bool>()); }
	    ;


order_list : order                  { $$ = make_pair(Expressions(1, $1 .first),vector<bool>(1, $1 .second)); }
           | order_list ',' order   { $$ = $1; $$.first.push_back($3 .first); $$.second.push_back($3 .second); }
		   ;

order : expression DESC      { $$ = make_pair($1, false); }
      | expression ASC       { $$ = make_pair($1, true); }
	  | expression			 { $$ = make_pair($1, true); }
	  ;

/*================= EXTENDED EXPRESSION =================================*/

expression_ex : expression
			  | vector 
			  | dictionary
			  ;

expression_list_ex : expression_ex   {  $$ = Expressions(1, $1); }
					| expression_list_ex ',' expression_ex { $$ = $1; $$.push_back($3); }
					;

/*================= EXPRESSION =========================================*/

expression_list : expression         {  $$ = Expressions(1, $1); }
				| expression_list ',' expression { $$ = $1; $$.push_back($3); }
				;

optional_hash : HASH expression { $$ = $2; }
			  |                 { $$ = new NumberExpression(0); }
			  ;


atom_or_number : '(' expression ')'           { $$ = $2; }
			   | '-' expression               { $$ = FunctionFactory::instance().build("-",$2); }
			   | DOUBLE                       { $$ = new NumberExpression($1); }
			   | column                   
			   | VAR                          { $$ = SQLSession::current().currentDatabase().getVariable($1); } 
			   | '?' DOUBLE                   { $$ = new ParameterExpression($2); }
			   | func '(' expression_list ')' { $$ = FunctionFactory::instance().build($1, $3); }
			   | func '(' empty ')'           { $$ = FunctionFactory::instance().build($1, emptyExpressionList); }
			   | func '(' '*' ')'             
				{
					if (string("count") != $1)
						throw eckit::UserError(string("Only function COUNT can accept '*' as parameter (") + $1 + ")");

					$$ = FunctionFactory::instance().build("count", new NumberExpression(1.0));
				}
			   | STRING                       { $$ = new StringExpression($1); }
			   ;


func : IDENT { $$ = $1;      }
	 | COUNT { $$ = "count"; }
	 ;

/* note: a^b^c -> a^(b^c) as in fortran */

power       : atom_or_number
			;

factor      : factor '*' power          { $$ = FunctionFactory::instance().build("*",$1,$3);   }
            | factor '/' power          { $$ = FunctionFactory::instance().build("/",$1,$3); }
            /* | factor '%' power          { $$ = new CondMOD($1,$3); } */
            | power
            ;

term        : term '+' factor           { $$ = FunctionFactory::instance().build("+",$1,$3);   }
            | term '-' factor           { $$ = FunctionFactory::instance().build("-",$1,$3);   }
            /* | term '&' factor */
            | factor
            ;

condition   : condition '>' term        { $$ = FunctionFactory::instance().build(">",$1,$3);   }
            | condition EQ term         { $$ = FunctionFactory::instance().build("=",$1,$3);   }
            | condition '<' term        { $$ = FunctionFactory::instance().build("<",$1,$3);   }
            | condition  GE term        { $$ = FunctionFactory::instance().build(">=",$1,$3);   }
            | condition  LE term        { $$ = FunctionFactory::instance().build("<=",$1,$3);   }
            | condition  NE term        { $$ = FunctionFactory::instance().build("<>",$1,$3);   }
            | condition  IN '(' expression_list ')'      { $4.push_back($1); $$ = FunctionFactory::instance().build("in",$4);   }
            | condition  IN VAR         
			{ 
				SQLExpression* v = SQLSession::current().currentDatabase().getVariable($3);
				ASSERT(v && v->isVector());
				Expressions e(v->vector());
				e.push_back($1);
				$$ = FunctionFactory::instance().build("in", e);
			}
            | condition  NOT IN '(' expression_list ')'  { $5.push_back($1); $$ = FunctionFactory::instance().build("not_in",$5);   }
            | condition  NOT IN VAR  
			{ 
				SQLExpression* v = SQLSession::current().currentDatabase().getVariable($4);
				ASSERT(v && v->isVector());
				Expressions e(v->vector());
				e.push_back($1);
				$$ = FunctionFactory::instance().build("not_in", e);
			}

            | NOT condition             { $$ = FunctionFactory::instance().build("not",$2);   }
			| condition IS NIL          { $$ = FunctionFactory::instance().build("null",$1);   }
			| condition IS NOT NIL      { $$ = FunctionFactory::instance().build("not_null",$1);   }
			| condition BETWEEN term AND term { $$ = FunctionFactory::instance().build("between",$1,$3,$5); }
			| condition NOT BETWEEN term AND term { $$ = FunctionFactory::instance().build("not_between",$1,$4,$6); }
            | term
            ;

conjonction : conjonction AND condition       { $$ = FunctionFactory::instance().build("and",$1,$3);   }
            | condition
            ;

disjonction : disjonction OR conjonction      { $$ = FunctionFactory::instance().build("or",$1,$3);   }
            | conjonction
            ;

expression  : disjonction
			| expression '[' expression ']'  
			{
				SQLExpression* container = $1;
				SQLExpression* index = $3;

				bool missing = false;
				long i = index->eval(missing);
				ASSERT(! missing);
				if (container->isVector())
				{
					//cerr << "==== index: '" << i << "'" << endl;
					$$ = container->vector()[i];
				}
				else
				if (container->isDictionary())
				{
					// TODO: check title always returns string repr of it's value
					string key = index->title();
					//cerr << "==== key: '" << key << "'" << endl;
					if (container->dictionary().find(key) == container->dictionary().end())
					{
						stringstream ss;
						ss << "Key '" << key << "' not found.";
						throw eckit::UserError(ss.str());
					}
					
					$$ = container->dictionary()[key];
				}
			}
            ;

empty :
      ;	

%%
#include "sqll.c"

