#ifndef DBMS_EXPRTERM_H_
#define DBMS_EXPRTERM_H_

#include "../table/row.h"
#include "../table/columns.h"
#include "expressionOp.h"
#include "../var/varfactory.h"
#include <map>

class ExprTerm {

public:
	int term_func;
	string table_name;
	string column_name;
	int table_index;
	int column_index;
	Var *var;
	Var *group_var;

	ll sum_value;
	ll count_value;
	ll max_value;
	ll min_value;
	map<string, ll> sum_string;
	map<string, ll> count_string;
	map<string, ll> max_string;
	map<string, ll> min_string;
	map<ll, ll> sum_longint;
	map<ll, ll> count_longint;
	map<ll, ll> max_longint;
	map<ll, ll> min_longint;

	int getFunc(string func) {
		if (func == "count") {
			return FUNC_OP_COUNT;
		}
		if (func == "sum") {
			return FUNC_OP_SUM;
		}
		if (func == "avg") {
			return FUNC_OP_AVG;
		}
		if (func == "max") {
			return FUNC_OP_MAX;
		}
		if (func == "min") {
			return FUNC_OP_MIN;
		}
		return FUNC_OP_NULL;
	}
	int init(Columns &header) {
		sum_value = 0;
		count_value = 0;
		min_value = ((ll)1) << 62;
		max_value = -min_value;
		table_index = 1;
		column_index = header.column.size();
		for (int i = 0; i < header.column.size(); ++i) {
			if (header.column[i].name == column_name) {
				column_index = i;
				break;
			}
		}
		if (column_index == header.column.size()) {
			return -15;
		}
		return 0;
	}
	int init(string file1, string file2, Columns &header1, Columns &header2) {
		if (file1 == table_name) {
			table_index = 1;
			column_index = header1.column.size();
			for (int i = 0; i < header1.column.size(); ++i) {
				if (header1.column[i].name == column_name) {
					column_index = i;
					break;
				}
			}
			if (column_index == header1.column.size()) {
				return -15;
			}
		}
		else {
			table_index = 2;
			column_index = header2.column.size();
			for (int i = 0; i < header2.column.size(); ++i) {
				if (header2.column[i].name == column_name) {
					column_index = i;
					break;
				}
			}
			if (column_index == header2.column.size()) {
				return -15;
			}
		}
		return 0;
	}
	int init(string file1, string file2, string file3, Columns &header1, Columns &header2, Columns &header3) {
		if (file1 == table_name) {
			table_index = 1;
			column_index = header1.column.size();
			for (int i = 0; i < header1.column.size(); ++i) {
				if (header1.column[i].name == column_name) {
					column_index = i;
					break;
				}
			}
			if (column_index == header1.column.size()) {
				return -15;
			}
		}
		else if (file2 == table_name) {
			table_index = 2;
			column_index = header2.column.size();
			for (int i = 0; i < header2.column.size(); ++i) {
				if (header2.column[i].name == column_name) {
					column_index = i;
					break;
				}
			}
			if (column_index == header2.column.size()) {
				return -15;
			}
		}
		else {
			table_index = 3;
			column_index = header3.column.size();
			for (int i = 0; i < header3.column.size(); ++i) {
				if (header3.column[i].name == column_name) {
					column_index = i;
					break;
				}
			}
			if (column_index == header3.column.size()) {
				return -15;
			}
		}
		return 0;
	}
	int agg(Row &record, int group) {
		if (group >= 0) {
			//cout << "NOW AGG" << endl;
			group_var = record.items[group].var;
			if (group_var->type() == TYPE_VARCHAR) {
				string s = ((Varchar*) group_var)->get();
				if (sum_string.find(s) == sum_string.end()) {
					sum_string[s] = 0;
					count_string[s] = 0;
					min_string[s] = ((ll) 1) << 62;
					max_string[s] = -min_string[s];
				}
				var = record.items[column_index].var;
				if (var->type() != TYPE_LONGINT) {
					return 0;
				}
				ll l = ((Longint*) var)->get();
				sum_string[s] = sum_string[s] + l;
				count_string[s] = count_string[s] + 1;
				if (l > max_string[s]) {
					max_string[s] = l;
				}
				if (l < min_string[s]) {
					min_string[s] = l;
				}
			}
			if (group_var->type() == TYPE_LONGINT) {
				//cout << "NOW PRINT 11111" << endl;
				ll s = ((Longint*) group_var)->get();
				if (sum_longint.find(s) == sum_longint.end()) {
					sum_longint[s] = 0;
					count_longint[s] = 0;
					min_longint[s] = ((ll) 1) << 62;
					max_longint[s] = -min_longint[s];
				}
				var = record.items[column_index].var;
				if (var->type() != TYPE_LONGINT) {
					return 0;
				}
				ll l = ((Longint*) var)->get();
				sum_longint[s] = sum_longint[s] + l;
				count_longint[s] = count_longint[s] + 1;
				if (l > max_longint[s]) {
					max_longint[s] = l;
				}
				if (l < min_longint[s]) {
					min_longint[s] = l;
				}
				//cout << "NOW PRINT 333" << endl;
			}
			return 0;
		}
		var = record.items[column_index].var;
		if (var->type() != TYPE_LONGINT) {
			return 0;
		}
		ll l = ((Longint*) var)->get();
		sum_value = sum_value + l;
		count_value = count_value + 1;
		if (l > max_value) {
			max_value = l;
		}
		if (l < min_value) {
			min_value = l;
		}
		return 0;
	}
	int aggprint() {
		if (term_func == FUNC_OP_COUNT) {
			cout << count_value << ' ';
		}
		if (term_func == FUNC_OP_SUM) {
			cout << sum_value << ' ';
		}
		if (term_func == FUNC_OP_AVG) {
			if (count_value != 0) {
				cout << (double) sum_value / count_value << ' ';
			}
		}
		if (term_func == FUNC_OP_MAX) {
			cout << max_value << ' ';
		}
		if (term_func == FUNC_OP_MIN) {
			cout << min_value << ' ';
		}
		return 0;
	}
	int agggroupprint(int group, int index) {
		if (group_var->type() == TYPE_VARCHAR) {
			map<string, ll>::iterator it_sum = sum_string.begin();
			map<string, ll>::iterator it_count = count_string.begin();
			map<string, ll>::iterator it_max =  max_string.begin();
			map<string, ll>::iterator it_min = min_string.begin();
			while (it_sum != sum_string.end()) {
				if (index < 0) {
					break;
				}
				it_sum++;
				it_count++;
				it_max++;
				it_min++;
				index--;
			}
			if (it_sum == sum_string.end()) {
				return -1;
			}
			if (term_func == FUNC_OP_COUNT) {
				cout << it_count->second << ' ';
			}
			else if (term_func == FUNC_OP_SUM) {
				cout << it_sum->second << ' ';
			}
			else if (term_func == FUNC_OP_AVG) {
				cout << (double) it_sum->second / it_count->second << ' ';
			}
			else if (term_func == FUNC_OP_MAX) {
				cout << it_max->second << ' ';
			}
			else if (term_func == FUNC_OP_MIN) {
				cout << it_min->second << ' ';
			}
			else {
				cout << it_sum->first << ' ';
			}
			return 0;
		}
		if (group_var->type() == TYPE_LONGINT) {
			map<ll, ll>::iterator it_sum = sum_longint.begin();
			map<ll, ll>::iterator it_count = count_longint.begin();
			map<ll, ll>::iterator it_max =  max_longint.begin();
			map<ll, ll>::iterator it_min = min_longint.begin();
			while (it_sum != sum_longint.end()) {
				if (index < 0) {
					break;
				}
				it_sum++;
				it_count++;
				it_max++;
				it_min++;
				index--;
			}
			if (it_sum == sum_longint.end()) {
				return -1;
			}
			if (term_func == FUNC_OP_COUNT) {
				cout << it_count->second << ' ';
			}
			else if (term_func == FUNC_OP_SUM) {
				cout << it_sum->second << ' ';
			}
			else if (term_func == FUNC_OP_AVG) {
				cout << (double) it_sum->second / it_count->second << ' ';
			}
			else if (term_func == FUNC_OP_MAX) {
				cout << it_max->second << ' ';
			}
			else if (term_func == FUNC_OP_MIN) {
				cout << it_min->second << ' ';
			}
			else {
				cout << it_sum->first << ' ';
			}
			return 0;
		}
		return 0;
	}
	int cal(Row &record) {
		var = record.items[column_index].var;
		return 0;
	}
	int cal(Row &record1, Row &record2) {
		if (table_index == 1) {
			var = record1.items[column_index].var;
		}
		else {
			var = record2.items[column_index].var;
		}
		return 0;
	}
	int cal(Row &record1, Row &record2, Row &record3) {
		if (table_index == 1) {
			var = record1.items[column_index].var;
		}
		else if (table_index == 2) {
			var = record2.items[column_index].var;
		}
		else {
			var = record3.items[column_index].var;
		}
		return 0;
	}
};

#endif
