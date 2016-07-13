#ifndef YYSTYPE_DEFINITION
#define YYSTYPE_DEFINITION

using namespace SQLYacc;

typedef SQLExpression* SQLExpressionPtr; // For casts.

struct YYSTYPE {
    SQLExpression         *exp;
    Table                 table;
    double                num;
    std::string           val;
    std::vector<std::string>         list;
    Expressions            explist;
    std::pair<SQLExpression*,bool> orderexp;
    std::pair<Expressions,std::vector<bool> > orderlist;
    std::vector<Table>     tablist;
    ColumnDefs             coldefs;
    ColumnDef              coldef;
    ConstraintDefs         condefs;
    ConstraintDef          condef;
    Range                  r;
    bool                   bol;
    SelectAST              select_statement;
    std::pair<ColumnDefs, ConstraintDefs> tablemd;             
};
#endif

#ifdef YY_DECL
#undef YY_DECL
#endif
#define YY_DECL int odblib_lex (YYSTYPE *odblib_lval_param, odblib_scan_t odblib_scanner, odb::sql::SQLSession*)

#define YYSTYPE_IS_DECLARED 1

YY_DECL;

