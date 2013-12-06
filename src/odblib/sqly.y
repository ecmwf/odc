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

#include <unistd.h>

#include "eckit/eckit.h"

using namespace SQLYacc;

typedef odb::sql::expression::SQLExpression* SQLExpressionPtr; // For casts.

struct YYSTYPE {
	SQLExpression         *exp;
	SQLTable              *table;
	double                 num;
	string                 val;
	vector<string>         list;
	Expressions            explist;
	std::pair<SQLExpression*,SQLExpression*> ass;
	Dictionary				dic;
	std::pair<SQLExpression*,bool> orderexp;
	std::pair<Expressions,vector<bool> > orderlist;
	vector<SQLTable*>      tablist;
	ColumnDefs             coldefs;
	ColumnDef              coldef;
	ConstraintDefs         condefs;
	ConstraintDef          condef;
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
%token SCHEMA
%token VIEW
%token INDEX
%token TABLE
%token TYPE
%token TYPEDEF
%token TEMPORARY
%token INHERITS
%token DEFAULT

%token CONSTRAINT
%token UNIQUE
%token PRIMARY
%token FOREIGN
%token KEY
%token REFERENCES

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
%type <val>table_name;
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
%type <val>relational_operator;

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
%type <val>default_value;
%type <bol>temporary;
%type <list>inherits;
%type <list>inheritance_list;
%type <list>inheritance_list_;

%type <condefs>constraint_list;
%type <condefs>constraint_list_;
%type <condef>constraint;
%type <val>constraint_name;
%type <condef>primary_key;
%type <condef>foreign_key;
%type <list>column_reference_list;
%type <val>column_reference;

%%

start : statements { SQLSession::current().currentDatabase().setLinks(); }
	;

statements : statement ';'
		   | statements statement ';'
		   ;

statement: select_statement
		 | set_statement
		 | create_schema_statement
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
		 | empty
	;

exit_statement: EXIT { /*delete &SQLSession::current();*/ return 0; }
	;

readonly_statement: READONLY
	;

updated_statement: UPDATED
	;

noreorder_statement: NOREORDER
	;

safeguard_statement: SAFEGUARD
	;

create_schema_statement: CREATE SCHEMA schema_name schema_element_list
        {
            sql::SQLDatabase& db = SQLSession::current().currentDatabase();
            db.schemaAnalyzer().endSchema();
        }
        ;

schema_name: IDENT
           {
               sql::SQLDatabase& db = SQLSession::current().currentDatabase();
               db.schemaAnalyzer().beginSchema($1);
           }
           ;

schema_element_list: schema_element
                   | schema_element_list schema_element
                   ;

schema_element: create_table_statement
              | create_view_statement
              | empty
              ;

create_index_statement: CREATE INDEX IDENT TABLE IDENT
	{
		SQLSession& s  = SQLSession::current();
		s.createIndex($3,$5);
	}
	| CREATE INDEX IDENT '@' IDENT
	{
		SQLSession& s  = SQLSession::current();
		s.createIndex($3,$5);
	}
	;

create_type_statement: create_type IDENT as_or_eq '(' bitfield_def_list ')'
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
		
		//cout << "CREATE TYPE " << typeName << " AS " << typeSignature << ";" << std::endl;		
	}
	;

create_type_statement: create_type IDENT as_or_eq IDENT { type::SQLType::createAlias($4, $2); }
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

temporary: TEMPORARY  { $$ = true; }
         | empty      { $$ = false; }
         ;

inheritance_list: inheritance_list_       { $$ = $1; }
                 | inheritance_list_ ','  { $$ = $1; }
                 ;

inheritance_list_: IDENT                         { $$ = vector<string>(); $$.insert($$.begin(), $1); }
                 | inheritance_list_ ',' IDENT   { $$ = $1; $$.push_back($3); }

inherits: INHERITS '(' inheritance_list ')'   { $$ = $3;               }
        | empty                               { $$ = vector<string>(); }
        ;

constraint_list: constraint_list_ { $$ = $1; }
               | constraint_list_ ',' { $$ = $1; }
               ;

constraint_list_: constraint { $$ = ConstraintDefs(1, $1); }
                | constraint_list_ ',' constraint { $$ = $1; $$.push_back($3); }
                | empty { $$ = ConstraintDefs(0); }
                ;

constraint: primary_key { $$ = $1; }
          | foreign_key { $$ = $1; }
          ;

primary_key: constraint_name UNIQUE '(' column_reference_list ')' { $$ = ConstraintDef($1, $4); }
           | constraint_name PRIMARY KEY '(' column_reference_list ')' { $$ = ConstraintDef($1, $5); }
           ;

foreign_key: constraint_name FOREIGN KEY '(' column_reference_list ')' REFERENCES IDENT '(' column_reference_list ')'
           {
                $$ = ConstraintDef($1, $5, $8, $10);
           }
           ;

constraint_name: CONSTRAINT IDENT { $$ = $2; }
               | empty { $$ = string(); }
               ;

column_reference_list: column_reference { $$ = vector<string>(1, $1); }
                | column_reference_list ',' column_reference { $$ = $1; $$.push_back($3); }
                ;

column_reference: IDENT table_reference { $$ = $1 + $2; }
           ;

create_table_statement: CREATE temporary TABLE table_name AS '(' column_def_list constraint_list ')' inherits
	{
		bool temporary($2);
                string name($4);
		ColumnDefs cols ($7);
		vector<string> inheritance($10);
		TableDef tableDef(name, cols, $8, inheritance);
		SQLSession& s  = SQLSession::current();
		s.currentDatabase().schemaAnalyzer().addTable(tableDef);
	}
	;

table_name: IDENT { $$ = $1; }
          | IDENT '.' IDENT { $$ = $1 + string(".") + $3; }
          | expression_ex { SQLExpression* e($1); $$ = e->title(); }
          ;

/* TODO: optional_as: AS | empty; BREAKS PARSING */

column_def_list: column_def_list_     { $$ = $1; }
               | column_def_list_ ',' { $$ = $1; }
               | empty                { $$ = ColumnDefs(); }
               ;
	 
column_def_list_: column_def                      { $$ = ColumnDefs(1, $1); }
                | column_def_list_ ',' column_def { $$ = $1; $$.push_back($3); }
                ;

column_def: column_name vector_range_decl data_type default_value
	{
		$$ = ColumnDef($1, $3, $2, $4);
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

default_value: DEFAULT expression_ex { SQLExpression* e($2); $$ = e->title(); }
             | empty { $$ = string(); }
             ;

create_view_statement: CREATE VIEW IDENT AS select_statement
	;

select_statement: SELECT distinct select_list into from where group_by order_by
					{   
						bool                   distinct($2);
						Expressions            select_list($3);
						string                 into($4);
						vector<SQLTable*>      from($5);
						SQLExpression          *where($6);
						Expressions            group_by($7);
						std::pair<Expressions,vector<bool> >      order_by($8);

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
	  |                  { $$ = 0;  } 
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

set_statement : SET DATABASE STRING { SQLSession::current().openDatabase($3); }
	; 

set_statement : SET DATABASE STRING AS IDENT { SQLSession::current().openDatabase($3,$5); }
	; 

set_statement : SET VAR EQ assignment_rhs
	{ 
		//cout << "== set variable " << $2 << " to "; if ($4) cout << *($4) << std::endl; else cout << "NULL" << std::endl;
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

            bool missing;
			$$ = SQLSelectFactory::instance().createColumn(columnName, bitfieldName, vectorIndex, table, pshift);
		  }
	   | IDENT bitfield_ref table_reference optional_hash
		{

			std::string columnName      ($1);
			std::string bitfieldName    ($2);
			SQLExpression* vectorIndex  (0); 
			std::string table           ($3);
			SQLExpression* pshift       ($4);

            bool missing;
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

optional_hash : HASH DOUBLE { $$ = new NumberExpression($2); }
			  |             { $$ = new NumberExpression(0); }
			  ;


atom_or_number : '(' expression ')'           { $$ = $2; }
			   | '-' expression               { $$ = ast("-",$2); }
			   | DOUBLE                       { $$ = new NumberExpression($1); }
			   | column                   
			   | VAR                          { $$ = SQLSession::current().currentDatabase().getVariable($1); } 
			   | '?' DOUBLE                   { $$ = new ParameterExpression($2); }
			   | func '(' expression_list ')' { $$ = ast($1, $3); }
			   | func '(' empty ')'           { $$ = ast($1, emptyExpressionList); }
			   | func '(' '*' ')'             
				{
					if (string("count") != $1)
						throw eckit::UserError(string("Only function COUNT can accept '*' as parameter (") + $1 + ")");

					$$ = ast("count", new NumberExpression(1.0));
				}
			   | STRING                       { $$ = new StringExpression($1); }
			   ;


func : IDENT { $$ = $1;      }
	 | COUNT { $$ = "count"; }
	 ;

/* note: a^b^c -> a^(b^c) as in fortran */

power       : atom_or_number
			;

factor      : factor '*' power          { $$ = ast("*",$1,$3);   }
            | factor '/' power          { $$ = ast("/",$1,$3); }
            /* | factor '%' power          { $$ = new CondMOD($1,$3); } */
            | power
            ;

term        : term '+' factor           { $$ = ast("+",$1,$3);   }
            | term '-' factor           { $$ = ast("-",$1,$3);   }
            /* | term '&' factor */
            | factor
            ;

relational_operator: '>' { $$ = ">"; }
                   | EQ  { $$ = "="; }
                   | '<' { $$ = "<"; }
                   | GE  { $$ = ">="; }
                   | LE  { $$ = "<="; }
                   | NE  { $$ = "<>"; }
                   ;
condition   : term relational_operator term relational_operator term { $$ = ast("and", ast($2,$1,$3), ast($4,$3,$5)); }
            | term relational_operator term                          { $$ = ast($2, $1, $3); }
            | condition  IN '(' expression_list ')'                  { $4.push_back($1); $$ = ast("in",$4);   }
            | condition  IN VAR         
			{ 
				SQLExpression* v = SQLSession::current().currentDatabase().getVariable($3);
				ASSERT(v && v->isVector());
				Expressions e(v->vector());
				e.push_back($1);
				$$ = ast("in", e);
			}
            | condition  NOT IN '(' expression_list ')'  { $5.push_back($1); $$ = ast("not_in",$5);   }
            | condition  NOT IN VAR  
			{ 
				SQLExpression* v = SQLSession::current().currentDatabase().getVariable($4);
				ASSERT(v && v->isVector());
				Expressions e(v->vector());
				e.push_back($1);
				$$ = ast("not_in", e);
			}

            | NOT condition             { $$ = ast("not",$2);   }
			| condition IS NIL          { $$ = ast("null",$1);   }
			| condition IS NOT NIL      { $$ = ast("not_null",$1);   }
			| condition BETWEEN term AND term { $$ = ast("between",$1,$3,$5); }
			| condition NOT BETWEEN term AND term { $$ = ast("not_between",$1,$4,$6); }
            | term
            ;

conjonction : conjonction AND condition       { $$ = ast("and",$1,$3);   }
            | condition
            ;

disjonction : disjonction OR conjonction      { $$ = ast("or",$1,$3);   }
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
					//cerr << "==== index: '" << i << "'" << std::endl;
					$$ = container->vector()[i];
				}
				else
				if (container->isDictionary())
				{
					// TODO: check title always returns string repr of it's value
					string key = index->title();
					//cerr << "==== key: '" << key << "'" << std::endl;
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

