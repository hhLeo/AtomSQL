#ifndef NODE_DEF_H
#define NODE_DEF_H
/*
 * structure of parse tree nodes
 */
struct node{
//   NODEKIND kind;
   int kind;

   union u{
      /* SM component nodes */
      /* create table node */
      struct{
         char *relname;
         struct node *attrlist;
         struct node *keydeclist;
      } CREATETABLE;

      /* create index node */
      struct{
         char *relname;
         char *attrname;
      } CREATEINDEX;

      /* create database node */
      struct{
        char *dbname;
      } CREATEDATABASE;

      /* drop table node */
      struct{
         char *relname;
      } DROPTABLE;

      /* drop index node */
      struct{
         char *relname;
         char *attrname;
      } DROPINDEX;

      /* drop database node */
      struct{
        char *dbname;
      } DROPDATABASE;

      /* use database node */
      struct{
        char *dbname;
      } USEDATABASE;

      /* desc table node */
      struct{
         char *relname;
      } DESCTABLE;

      /* show table node -- as func "print"*/
      struct{
         char *relname;
      } SHOWTABLE;

      /* QL component nodes */
      /* query node */
      struct{
        char *optdistinct;
         struct node *selectclause;
         struct node *tablelist;
         struct node *conditionlist;
         struct node *orderrelattr; 
         struct node *grouprelattr; 
      } QUERY;

      /* select node */
      struct{
        struct node *query;
        char *combo;
        struct node *select;
      } SELECT;

      /* insert node */
      struct{
         char *relname;
         struct node *insertlist;
      } INSERT;

      /* delete node */
      struct{
         char *relname;
         struct node *conditionlist;
      } DELETE;

      /* update node */
      struct{
         char *relname;
         struct node *relattr;
         struct node *relorvalue;
         struct node *conditionlist;
      } UPDATE;

      /* command support nodes */
      /* relation attribute node */
      struct{
         char *relname;
         char *attrname;
      } RELATTR;

      /* order + relation attribute node */
      struct{
         struct node *relattr;
         char *optasc;
      } ORDERATTR;

      /* agg func + relation attribute node */
      struct{
         char *func;
         char *relname;
         char *attrname;
      } AGGRELATTR;

      /* condition node */
      struct{
         struct node *lhsRelattr;
         char *op;
         struct node *rhsRelattr;
         struct node *rhsValue;
      } CONDITION;

      /* condition list node */
      struct{
         struct node *boolterm;
         char *boolop;
         struct node *condlist;
      } CONDITIONLIST;

      /* relation-attribute or value */
      struct{
         struct node *relattr;
         struct node *value;
      } RELATTR_OR_VALUE;

      /* <attribute, type> pair */
      struct{
         char *attrname;
         char *type;
         int length;
         bool canNull;
      } ATTRTYPE;

      /* <value, type> pair */
      struct{
         char *type;
         int  ival;
         float rval;
         char *sval;
      } VALUE;

      /* relation node */
      struct{
         char *relname;
      } RELATION;

      /* list node */
      struct{
         struct node *curr;
         struct node *next;
      } LIST;

      /* opt alias node */
      struct{
        struct node *exprplus;
        char *alias;
      } OPTALIAS;

      /* expr node */
      struct{
        struct node *expleft;
        char *exprop; //operator
        struct node *expright;
      } EXPR;

      /* table list node */
      struct{
        char *relname;
        char *alias;
      } TABLELIST;

      /* in node */
      struct{
        struct node *relattr;
        struct node *instatement;
      } IN;

      /* quit node */
      struct{
      } QUIT;
   } u;
};
typedef struct node NODE;
#endif
