/*
 * sql.y: yacc specification for SQL
 * bison -d sql.y
 * bison -d -o sql.y.cpp sql.l -p"json2tdata_"
 * -p "json2tdata_"是给语法分析器加一个前缀。
 * 有这个选项，就会生成json2tdata_parse等以json2tdata开头的函数。
 * 
 * Huang Huan
 * CST, Tsinghua University
 */
 
 /* part1 : copy to .c file 原封不动的复制到flex的生成代码中 */
%{
	/* 声明 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include "node_def.h"
#include "parser_internal.h"
#include "bison.tab.h"

//%code requires { #include "parser_internal.h" }


using namespace std;

//sql-master
#define YYERROR_VERBOSE
void yyerror(const char *s);
int yylex(void);
//extern int yylineno;
#define YYDEBUG 0
int yydebug=0;
//sql-master

/*
#ifndef yyrestart
void yyrestart(FILE*);
#endif
*/

/*
 * string representation of tokens; provided by scanner
 */
extern char *yytext;

/*
 * points to root of parse tree
 */
static NODE *parse_tree;

int bExit;                 // when to return from RBparse

%}


/* 辅助定义 */
%union {
	int ival;
	float rval;
	char *sval;
	bool bval;
	NODE *n;
}


/* part2 : 声明在flex中定义的那些TOKEN */
%token CREATE DROP USE SHOW SELECT DESC INSERT DELETE UPDATE QUIT
%token TABLES DATABASE TABLE INDEX 
%token SET WHERE FROM ORDER GROUP CHECK BY INTO VALUES AS DISTINCT ALL
%token PRIMARY KEY ASC IN
%token AND OR EXCEPT INTERSECT UNION
%token NOT TOKEN_NULL
%token MAX MIN SUM AVG COUNT
%token EQU NEQ LSS GTR LEQ GEQ CONCAT IS LIKE T_EOF
%token PLUS MINUS MULTIPLY DIVIDE
%token INTEGER CHAR FLOAT VARCHAR
%token <ival> INT_LITERAL
%token <rval> FLOAT_LITERAL
%token <sval> STRING_LITERAL IDENTIFIER T_SHELL_CMD


/*
%type <cval> comp_op
%type <aval> function_name
%type <tval> column_type
%type <combo> select_combo bool_op
%type <oprt> oprt
*/
%type <sval> comp_op function_name column_type select_combo bool_op oprt_plus oprt_mul
%type <sval> opt_alias database_name table_name column_name
%type <sval> opt_asc_desc opt_distinct
%type <bval> can_null
%type <n> command
%type <n> create select insert delete drop update use show desc quit
%type <n> create_database create_table create_index
%type <n> select_statement
%type <n> drop_database drop_table drop_index
%type <n> opt_where_condition where_condition condition in_statement
%type <n> expression_list expression expr_plus mulexp primary term
%type <n> table table_ref
%type <n> opt_group_by_clause opt_order_by_clause
%type <n> insert_list values_list literal_value
%type <n> bool_term relattr_or_value relattr
%type <n> column_dec_list column_dec
%type <n> opt_key_dec_list key_dec_list key_dec

%left OR CONTAC
%left AND
%nonassoc EQU NEQ
%nonassoc LSS GTR LEQ GEQ
%left  PLUS MINUS
%left  MULTIPLY DIVIDE

%%
/* 语法规则 */

/* Bison能够完全支持LR(1)文法。*/
/* 这种文法的特点是只要多向前看一个TOKEN，就能够决定如何解析。*/
/* bison用$$表示规则左边的对象，用$1 $2 $3 等依次表示规则右边的对象。*/

start
	: command ';'
		{
			parse_tree = $1;
			YYACCEPT;
		}
	| T_SHELL_CMD
		{
			if (!isatty(0)) {
				cout << ($1) << "\n";
				cout.flush();
			}
			system($1);
			parse_tree = NULL;
			YYACCEPT;
		}
/*	| error
		{
			
			reset_scanner();
			parse_tree = NULL;
			YYACCEPT;
			
		}*/
	| T_EOF
		{
			parse_tree = NULL;
			bExit = 1;
			YYACCEPT;
		}
	;

command
	: create
	| select
	| insert
	| delete
	| drop
	| update
	| use
	| show
	| desc
	| quit
	/*| /* empty */
	;

quit
	: QUIT { $$ = quit_node(); }
	;

use
	: USE database_name { $$ = use_database_node($2); }
	;

desc
	: DESC table_name { $$ = desc_table_node($2); }
	;

show
	: SHOW TABLES
		{
			$$ = show_table_node(NULL);
		}
	| SHOW TABLE table_name
		{
			$$ = show_table_node($3);
		}
	;

update
	: UPDATE IDENTIFIER SET relattr EQU expr_plus opt_where_condition
		{
			$$ = update_node($2, $4, $6, $7);
		}
	;

drop
	: drop_table
	| drop_index
	| drop_database
	;

create
	: create_table
	| create_index
	| create_database
	;

create_database
	: CREATE DATABASE database_name { $$ = create_database_node($3); }
	;

drop_database
	: DROP DATABASE database_name { $$ = drop_database_node($3); }
	;

drop_table
	: DROP TABLE table_name { $$ = drop_table_node($3); }
	;

database_name
	: IDENTIFIER { $$ = $1; }
	;

create_index
	: CREATE INDEX table_name '(' column_name ')'
		{ 
			$$ = create_index_node($3, $5); 
		}
	;

drop_index
	: DROP INDEX table_name '(' column_name ')'
		{
			$$ = drop_index_node($3, $5);
		}
	;

create_table
	: CREATE TABLE table_name '(' column_dec_list opt_key_dec_list ')' 
		{
			$$ = create_table_node($3, $5, $6);
		}
	;

column_dec_list
	: column_dec { $$ = list_node($1); }
	| column_dec_list ',' column_dec { $$ = prepend($3, $1); }
	;

column_dec
	: column_name column_type '(' INT_LITERAL ')' can_null 
		{ 
			/*printf("column '%s' (%d)\n", $1, $2);*/
			$$ = attrtype_node($1, $2, $4, $6);
		}
	;

can_null
	: NOT TOKEN_NULL { $$ = false; }
	| /* empty */	{ $$ = true; }
	;

column_type
	: INTEGER 		{ $$ = (char*)"int"; }
	| FLOAT 		{ $$ = (char*)"float"; }
	| CHAR 			{ $$ = (char*)"char"; }
	| VARCHAR 		{ $$ = (char*)"varchar"; }
	;

opt_key_dec_list
	: ',' key_dec_list { $$ = $2; }
	| /* empty */ { $$ = NULL; }
	;

key_dec_list
	: key_dec { $$ = list_node($1); }
	| key_dec_list ',' key_dec { $$ = prepend($3, $1); }
	;

key_dec
	: PRIMARY KEY '(' column_name ')' { $$ = relation_node($4); }
	;

select
	: select_statement
		{
			$$ = select_node($1, NULL, NULL);
		}
	| select_statement select_combo select 
		{ 
			$$ = select_node($1, $2, $3);
		}
	;

/* combo : 组合 */
select_combo
	: UNION { $$ = (char*)"union"; }
	| INTERSECT { $$ = (char*)"intersect"; }
	| EXCEPT { $$ = (char*)"except"; }
	;

/*
		****  select_statement  --  select_node (query)  ****
	opt_distinct		:	opt_distinct (distinct | all | nothing)
	expression_list		:	select_clause_node (non_mt_select_clause)
	table 				:	table_list_node (non_mt_relation_list)
	opt_where_condition	:	condition_list_node (opt_where_clause)
	opt_order_by_clause	:	orderrelattr (order_by_node)
	opt_group_by_clause	:	grouprelattr (group_by_node)
*/
select_statement
	: SELECT opt_distinct expression_list FROM table opt_where_condition opt_order_by_clause opt_group_by_clause
		{
			$$ = query_node($2, $3, $5, $6, $7, $8);
		}
	| '(' select_statement ')' { $$ = $2; }
	;

opt_distinct
	: DISTINCT { $$ = (char*)"distinct"; }
	| ALL { $$ = (char*)"all"; }
	| /* empty */ { $$ = NULL; }
	;

opt_where_condition
	: where_condition {$$ = $1;}
	| /* empty */		{$$ = NULL;}
	;

where_condition
	: WHERE condition { $$ = $2; }
	;

opt_group_by_clause
	: GROUP BY relattr { $$ = $3; } 
	| /* empty */ { $$ = NULL;}
	;

opt_order_by_clause
	: ORDER BY relattr opt_asc_desc	{ $$ = order_attr_node($3, $4); }
	| /* empty */ { $$ = NULL;}
	;

opt_asc_desc
	: DESC { $$ = (char*)"desc"; }
	| ASC { $$ = (char*)"asc"; }
	| /* empty */ { $$ = (char*)"asc"; }
	;

condition
	: bool_term
		{
			$$ = condition_list_node($1);
		}
	| bool_term bool_op condition 
		{ 
			$$ = condition_list_node($1, $2, $3);
		}
	;

bool_term
	: relattr comp_op relattr_or_value
		{
			$$ = condition_node($1, $2, $3);
		}
	| relattr comp_op TOKEN_NULL
		{
			$$ = condition_node($1, $2, relattr_or_value_node(NULL, NULL));
		}
	| relattr in_statement
		{
			$$ = in_node($1, $2);
		}
	;

relattr_or_value
	: relattr
		{
			$$ = relattr_or_value_node($1, NULL);
		}
	| literal_value
		{
			$$ = relattr_or_value_node(NULL, $1);
		}
	;

relattr
	: IDENTIFIER '.' IDENTIFIER
		{
			$$ = relattr_node($1, $3);
		}
	| IDENTIFIER '.' TOKEN_NULL
		{
			$$ = relattr_node($1, NULL);	/* can this ok? */
		}
	| IDENTIFIER
		{
			$$ = relattr_node(NULL, $1);
		}
	;

in_statement
	:  IN '(' values_list ')' { $$ = $3; }
	|	IN '(' select ')'
   	{
   		fprintf(stderr, "****WARNING: IN SELECT statement not yet supported\n");
   	}
   ;

bool_op
	: AND { $$ = (char*)"and"; } 
	| OR { $$ = (char*)"or"; }
	;

comp_op
	: EQU { $$ = (char*)"="; } 
	| NEQ { $$ = (char*)"<>"; }
	| LSS { $$ = (char*)"<"; } 
	| GTR { $$ = (char*)">"; } 
	| LEQ { $$ = (char*)"<="; } 
	| GEQ { $$ = (char*)">="; } 
	| IS { $$ = (char*)"is"; } 
	| LIKE { $$ = (char*)"like"; }
	;

/* 聚合函数 (aggregate function) */
/*
	expression_list	: select_clause_node (non_mt_select_clause)
*/
expression_list
	: expression { $$ = list_node($1); }
	| expression_list ',' expression { $$ = prepend($3, $1); }
	| MULTIPLY
		{
			$$ = NULL;
		}
	;

/*
	opt_alias	:	AS IDENTIFIER
*/
expression
	: expr_plus opt_alias	{ $$ = opt_alias_node($1, $2); }
	;

expr_plus
	: expr_plus oprt_plus mulexp { $$ = expr_node($1, $2, $3); }
	| mulexp { $$ = expr_node(NULL, NULL, $1); }
	;

mulexp
	: mulexp oprt_mul primary 	{ $$ = expr_node($1, $2, $3); }
	| primary 		{ $$ = expr_node(NULL, NULL, $1); }
	;

oprt_plus
  : PLUS { $$ = (char*)"+"; }
  | MINUS { $$ = (char*)"-"; }
  ;

oprt_mul
  : MULTIPLY { $$ = (char*)"*"; }
  | DIVIDE { $$ = (char*)"/"; }
  ;

primary
	: '(' expr_plus ')' 	{ $$ = $2; }
	| MINUS primary 		{ $$ = expr_node(NULL, (char*)"-", $2); }
	| term 			{ $$ = $1; } 
	| literal_value		{ $$ = $1; }
	;

/*
	expression 	:	(aggrelattr)
	function 	:	count | sum | avg | min |max
	NO_F		:	no function
*/
term
	: TOKEN_NULL { $$ = NULL; }
	| column_name
		{
			$$ = aggrelattr_node(NULL, NULL, $1);
		}
	| table_name '.' column_name
		{
			$$ = aggrelattr_node(NULL, $1, $3);
		}
	| function_name '(' column_name ')'
		{
			$$ = aggrelattr_node($1, NULL, $3);
		}
	| function_name '(' table_name '.' column_name ')'
		{
			$$ = aggrelattr_node($1, $3, $5);
		}
	| function_name '(' MULTIPLY ')'
		{
			$$ = aggrelattr_node($1, NULL, (char*)"*");
		}
	;

opt_alias
	: AS IDENTIFIER { $$ = $2; }
	| IDENTIFIER { $$ = $1; }
	| /* empty */ { $$ = NULL; }
	;

function_name
	: COUNT { $$ = (char*)"count"; }
	| SUM { $$ = (char*)"sum"; }
	| AVG { $$ = (char*)"avg"; }
	| MIN { $$ = (char*)"min"; }
	| MAX{ $$ = (char*)"max"; }
	;

column_name
	: IDENTIFIER { $$ = $1; }
	;

table_name
	: IDENTIFIER { $$ = $1; }
	;

table
	: table_ref { $$ = list_node($1); }
	| table ',' table_ref { $$ = prepend($3, $1); }
	;

table_ref
	: table_name opt_alias { $$ = table_list_node($1, $2);}
	;

insert
	: INSERT INTO table_name VALUES insert_list
		{
			$$ = insert_node($3, $5);
		}
	;

insert_list
	: '(' values_list ')'
		{
			$$ = list_node($2);
		}
	| insert_list ',' '(' values_list ')'
		{
			$$ = prepend($4, $1);
		}
	;

values_list
	: literal_value { $$ = list_node($1); }
	| values_list ',' literal_value
		{ 
			$$ = prepend($3, $1); 
		}
	| TOKEN_NULL {
		$$ = list_node(value_node((char*)"null", NULL));
	}
	| values_list ',' TOKEN_NULL {
		$$ = prepend(value_node((char*)"null", NULL), $1);
	}
	;

literal_value
	: INT_LITERAL { $$ = value_node((char*)"int", (void *)& $1); }
	| FLOAT_LITERAL { $$ = value_node((char*)"float", (void *)& $1); }
	| STRING_LITERAL { $$ = value_node((char*)"string", (void *) $1); }/* have & or not ?*/
	;

delete
	: DELETE FROM table_name where_condition
		{
			$$ = delete_node($3, $4);
		}
	;



%%
/* 用户子程序 */
/* part3 : 语法的推导过程 */
/*
 * Required by yacc
 */

/* the output of the parser goes to the output file */
// extern FILE * outputFilePtr; 
RC AtomSQLparse()
{
	RC rc;
	bExit = 0;
	exe_start();
	while (!bExit) {
		/* Reset parser and scanner for a new query */
    	new_query();
//#if 0
    	/* Print a prompt : AtomSQL */
    	cout << PROMPT;

    	/* Get the prompt to actually show up on the screen */
    	cout.flush(); 

//#endif
    	/* If a query was successfully read, interpret it */
    	if(yyparse() == 0 && parse_tree != NULL) {
    		if ((rc = interp(parse_tree))) {
    		// if ((rc = interp(outputFilePtr, parse_tree))) {
        		// PrintError(rc);
            	if (rc < 0) {
            		bExit = true;
            	}
        	}
        }
	}
        	exe_stop();
	return rc;
}

int main(int argc, char const *argv[])
{
	if (argc == 1) {
		AtomSQLparse();
	}
	else if (argc == 2) {
		FILE *inputFile;
		inputFile = fopen(argv[1], "r");
		if(!inputFile) {
			fprintf(stderr, "unable to open %s\n", argv[1]);
			return -1;
		}
		extern FILE* yyin;
		yyin = inputFile;
		RC rc;
		bExit = 0;
		exe_start();
		while (!bExit) {
			/* Reset parser and scanner for a new query */
	    	new_query();

	    	/* If a query was successfully read, interpret it */
	    	if(yyparse() == 0 && parse_tree != NULL) {
	    		if ((rc = interp(parse_tree))) {
	    		// if ((rc = interp(outputFilePtr, parse_tree))) {
	        		// PrintError(rc);
	            	if (rc < 0) {
	            		bExit = true;
	            	}
	        	}
	        }
		}
	  exe_stop();
		fclose(inputFile);
		return rc;
	}
	else {
		cout << "\nUsage: ./sql_parser\n" << "     | ./sql_parser filename\n\n";
	}

	return 0;
}

void yyerror(const char *s) {
	puts(s);
	fprintf(stdout, "yyError!\n");
}

extern "C"{
/*
 * Sometimes required
 */
int yywrap(void)
{
   return 1;
}
};
