#ifndef DBMS_BOOLTERM_H_
#define DBMS_BOOLTERM_H_

#include "../table/row.h"
#include "../table/columns.h"
#include "../var/varfactory.h"
#include "conditionOp.h"
#include <regex>

class BoolTerm {

public:
	int comp_op;
	string left_table_name;
	string left_column_name;
	string right_table_name;
	string right_column_name;
	int left_table;
	int left_column;
	int right_table;
	int right_column;
	Var *right_value;
	int getOp(string op) {
		if (op == "=") {
			return COMP_OP_EQU;
		}
		if (op == "<>") {
			return COMP_OP_NEQ;
		}
		if (op == "<") {
			return COMP_OP_LSS;
		}
		if (op == ">") {
			return COMP_OP_GTR;
		}
		if (op == "<=") {
			return COMP_OP_LEQ;
		}
		if (op == ">=") {
			return COMP_OP_GEQ;
		}
		if (op == "is") {
			return COMP_OP_IS;
		}
		if (op == "like") {
			return COMP_OP_LIKE;
		}
		return COMP_OP_NULL;
	}
	int cal(Row &record, bool &term) {
		Var *var1 = record.items[left_column].var;
		Var *var2 = right_value;
		if (right_value == NULL) {
			var2 = record.items[right_column].var;
		}
		if ((var1 == NULL)  || (var2 == NULL)) {
			term = false;
			return 0;
		}
		if (comp_op == COMP_OP_IS) {
			term = (var1->type() == TYPE_NULL);
			return 0;
		}
		if ((var1->type()) != (var2->type())) {
			if (var1->type() == TYPE_NULL) {
				term = false;
				return 0;
			}
			return -9;
		}
		if (var1->type() == TYPE_VARCHAR) {
			string s1 = ((Varchar*) var1)->get();
			string s2 = ((Varchar*) var2)->get();
			if (comp_op == COMP_OP_EQU) {
				term = (s1 == s2);
				return 0;
			}
			if (comp_op == COMP_OP_NEQ) {
				term = (s1 != s2);
				return 0;
			}
			if (comp_op == COMP_OP_LIKE) {
				if (s2[0] == '%' && s2[s2.length()-1] == '%') {
					string pat0 = ".*" + s2.substr(1, s2.length()-2) + ".*";
					regex pattern0(pat0);
					term = regex_match(s1, pattern0);
				}
				else if (s2[0] == '%') {
					string pat1 = ".*" + s2.substr(1, s2.length()-1);
					regex pattern1(pat1);
					term = regex_match(s1, pattern1);
				}
				else if (s2[s2.length()-1] == '%') {
					string pat2 = s2.substr(0, s2.length()-1) + ".*";
					regex pattern2(pat2);
					term = regex_match(s1, pattern2);
				}
				else {
					term = false;
				}
				return 0;
			}
			return -10;
		}
		ll l1 = ((Longint*) var1)->get();
		ll l2 = ((Longint*) var2)->get();
		if (comp_op == COMP_OP_EQU) {
			term = (l1 == l2);
			return 0;
		}
		if (comp_op == COMP_OP_NEQ) {
			term = (l1 != l2);
			return 0;
		}
		if (comp_op == COMP_OP_LSS) {
			term = (l1 < l2);
			return 0;
		}
		if (comp_op == COMP_OP_GTR) {
			term = (l1 > l2);
			return 0;
		}
		if (comp_op == COMP_OP_LEQ) {
			term = (l1 <= l2);
			return 0;
		}
		if (comp_op == COMP_OP_GEQ) {
			term = (l1 >= l2);
			return 0;
		}
		return -11;
	}
	int cal(Row &record1, Row &record2, bool &term) {
		Var *var1;
		if (left_table == 1) {
			var1 = record1.items[left_column].var;
		}
		else {
			var1 = record2.items[left_column].var;
		}
		Var *var2 = right_value;
		if (right_value == NULL) {
			if (right_table == 1) {
				var2 = record1.items[right_column].var;
			}
			else {
				var2 = record2.items[right_column].var;
			}
		}
		if ((var1 == NULL)  || (var2 == NULL)) {
			term = false;
			return 0;
		}
		if (comp_op == COMP_OP_IS) {
			term = (var1->type() == TYPE_NULL);
			return 0;
		}
		if ((var1->type()) != (var2->type())) {
			if ((var1->type()) == TYPE_NULL) {
				term = false;
				return 0;
			}
			return -9;
		}
		if (var1->type() == TYPE_VARCHAR) {
			string s1 = ((Varchar*) var1)->get();
			string s2 = ((Varchar*) var2)->get();
			if (comp_op == COMP_OP_EQU) {
				term = (s1 == s2);
				return 0;
			}
			if (comp_op == COMP_OP_NEQ) {
				term = (s1 != s2);
				return 0;
			}
			if (comp_op == COMP_OP_LIKE) {
				term = false;
				return 0;
			}
			return -10;
		}
		ll l1 = ((Longint*) var1)->get();
		ll l2 = ((Longint*) var2)->get();
		if (comp_op == COMP_OP_EQU) {
			term = (l1 == l2);
			return 0;
		}
		if (comp_op == COMP_OP_NEQ) {
			term = (l1 != l2);
			return 0;
		}
		if (comp_op == COMP_OP_LSS) {
			term = (l1 < l2);
			return 0;
		}
		if (comp_op == COMP_OP_GTR) {
			term = (l1 > l2);
			return 0;
		}
		if (comp_op == COMP_OP_LEQ) {
			term = (l1 <= l2);
			return 0;
		}
		if (comp_op == COMP_OP_GEQ) {
			term = (l1 >= l2);
			return 0;
		}
		return -11;
	}
	int cal(Row &record1, Row &record2, Row &record3, bool &term) {
		Var *var1;
		if (left_table == 1) {
			var1 = record1.items[left_column].var;
		}
		else if (left_table == 2) {
			var1 = record2.items[left_column].var;
		}
		else {
			var1 = record3.items[left_column].var;
		}
		Var *var2 = right_value;
		if (right_value == NULL) {
			if (right_table == 1) {
				var2 = record1.items[right_column].var;
			}
			else if (right_table == 2) {
				var2 = record2.items[right_column].var;
			}
			else {
				var2 = record3.items[right_column].var;
			}
		}
		if ((var1 == NULL)  || (var2 == NULL)) {
			term = false;
			return 0;
		}
		if (comp_op == COMP_OP_IS) {
			term = (var1->type() == TYPE_NULL);
			return 0;
		}
		if ((var1->type()) != (var2->type())) {
			if ((var1->type()) == TYPE_NULL) {
				term = false;
				return 0;
			}
			return -9;
		}
		if (var1->type() == TYPE_VARCHAR) {
			string s1 = ((Varchar*) var1)->get();
			string s2 = ((Varchar*) var2)->get();
			if (comp_op == COMP_OP_EQU) {
				term = (s1 == s2);
				return 0;
			}
			if (comp_op == COMP_OP_NEQ) {
				term = (s1 != s2);
				return 0;
			}
			if (comp_op == COMP_OP_LIKE) {
				term = false;
				return 0;
			}
			return -10;
		}
		ll l1 = ((Longint*) var1)->get();
		ll l2 = ((Longint*) var2)->get();
		if (comp_op == COMP_OP_EQU) {
			term = (l1 == l2);
			return 0;
		}
		if (comp_op == COMP_OP_NEQ) {
			term = (l1 != l2);
			return 0;
		}
		if (comp_op == COMP_OP_LSS) {
			term = (l1 < l2);
			return 0;
		}
		if (comp_op == COMP_OP_GTR) {
			term = (l1 > l2);
			return 0;
		}
		if (comp_op == COMP_OP_LEQ) {
			term = (l1 <= l2);
			return 0;
		}
		if (comp_op == COMP_OP_GEQ) {
			term = (l1 >= l2);
			return 0;
		}
		return -11;
	}
	int init(Columns &header) {
		left_table = 1;
		left_column = header.column.size();
		for (int i = 0; i < header.column.size(); ++i) {
			if (header.column[i].name == left_column_name) {
				left_column = i;
				break;
			}
		}
		if (left_column == header.column.size()) {
			return -7;
		}
		if (right_value != NULL) {
			return 0;
		}
		right_table = 1;
		right_column = header.column.size();
		for (int i = 0; i < header.column.size(); ++i) {
			if (header.column[i].name == right_column_name) {
				right_column = i;
				break;
			}
		}
		if (right_column == header.column.size()) {
			return -7;
		}
		return 0;
	}
	int init(string file1, string file2, Columns &header1, Columns &header2) {
		if (file1 == left_table_name) {
			left_table = 1;
			left_column = header1.column.size();
			for (int i = 0; i < header1.column.size(); ++i) {
				if (header1.column[i].name == left_column_name) {
					left_column = i;
					break;
				}
			}
			if (left_column == header1.column.size()) {
				return -7;
			}
		}
		else {
			left_table = 2;
			left_column = header2.column.size();
			for (int i = 0; i < header2.column.size(); ++i) {
				if (header2.column[i].name == left_column_name) {
					left_column = i;
					break;
				}
			}
			if (left_column == header2.column.size()) {
				return -7;
			}
		}
		if (right_table_name != "") {
			right_value = NULL;
		}
		if (right_value != NULL) {
			return 0;
		}
		if (file1 == right_table_name) {
			right_table = 1;
			right_column = header1.column.size();
			for (int i = 0; i < header1.column.size(); ++i) {
				if (header1.column[i].name == right_column_name) {
					right_column = i;
					break;
				}
			}
			if (right_column == header1.column.size()) {
				return -7;
			}
		}
		else {
			right_table = 2;
			right_column = header2.column.size();
			for (int i = 0; i < header2.column.size(); ++i) {
				if (header2.column[i].name == right_column_name) {
					right_column = i;
					break;
				}
			}
			if (right_column == header2.column.size()) {
				return -7;
			}
		}
		return 0;
	}
	int init(string file1, string file2, string file3, Columns &header1, Columns &header2, Columns &header3) {
		if (file1 == left_table_name) {
			left_table = 1;
			left_column = header1.column.size();
			for (int i = 0; i < header1.column.size(); ++i) {
				if (header1.column[i].name == left_column_name) {
					left_column = i;
					break;
				}
			}
			if (left_column == header1.column.size()) {
				return -7;
			}
		}
		else if (file2 == left_table_name) {
			left_table = 2;
			left_column = header2.column.size();
			for (int i = 0; i < header2.column.size(); ++i) {
				if (header2.column[i].name == left_column_name) {
					left_column = i;
					break;
				}
			}
			if (left_column == header2.column.size()) {
				return -7;
			}
		}
		else {
			left_table = 3;
			left_column = header3.column.size();
			for (int i = 0; i < header3.column.size(); ++i) {
				if (header3.column[i].name == left_column_name) {
					left_column = i;
					break;
				}
			}
			if (left_column == header2.column.size()) {
				return -7;
			}
		}
		if (right_table_name != "") {
			right_value = NULL;
		}
		if (right_value != NULL) {
			return 0;
		}
		if (file1 == right_table_name) {
			right_table = 1;
			right_column = header1.column.size();
			for (int i = 0; i < header1.column.size(); ++i) {
				if (header1.column[i].name == right_column_name) {
					right_column = i;
					break;
				}
			}
			if (right_column == header1.column.size()) {
				return -7;
			}
		}
		else if (file2 == right_table_name) {
			right_table = 2;
			right_column = header2.column.size();
			for (int i = 0; i < header2.column.size(); ++i) {
				if (header2.column[i].name == right_column_name) {
					right_column = i;
					break;
				}
			}
			if (right_column == header2.column.size()) {
				return -7;
			}
		}
		else {
			right_table = 3;
			right_column = header3.column.size();
			for (int i = 0; i < header3.column.size(); ++i) {
				if (header3.column[i].name == right_column_name) {
					right_column = i;
					break;
				}
			}
			if (right_column == header3.column.size()) {
				return -7;
			}
		}
		return 0;
	}
};

#endif
