# 
# Makefile for the AtomSQL project parser
# 
# Huang Huan
# CST, Tsinghua University
# 

TARGET = -o sql_parser
CXX = g++ -std=c++11
YACC = bison -d
LEX = flex

all: sql_parser
	@echo "Finished building!"

sql_parser: bison.tab.c lex.yy.c parser_internal.h bison.tab.h
	$(CXX) $(TARGET) bison.tab.c lex.yy.c 

bison.tab.h: bison.tab.c

bison.tab.c: bison.y parser_internal.h
	$(YACC) bison.y

lex.yy.c: flex.l parser_internal.h bison.tab.h
	$(LEX) flex.l

clean:
	rm bison.output bison.tab.c bison.tab.h lex.yy.c sql_parser
 
