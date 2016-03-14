#ifndef DBMS_EXPRESSIONS_H_
#define DBMS_EXPRESSIONS_H_

#include "../table/row.h"
#include "../table/columns.h"
#include "expression.h"
#include <vector>

class Expressions {

public:
	vector<Expression> expressions;
	string group_by;
	int group;

	int init(Columns &header) {
		group = -1;
		for (int i = 0; i < header.column.size(); ++i) {
			if (group_by == header.column[i].name) {
				group = i;
				break;
			}
		}
		for (int i = 0; i < expressions.size(); ++i) {
			int result = expressions[i].init(header);
			if (result) {
				return result;
			}
		}
		return 0;
	}
	int init(string file1, string file2, Columns &header1, Columns &header2) {
		for (int i = 0; i < expressions.size(); ++i) {
			int result = expressions[i].init(file1, file2, header1, header2);
			if (result) {
				return result;
			}
		}
		return 0;
	}
	int init(string file1, string file2, string file3, Columns &header1, Columns &header2, Columns &header3) {
		for (int i = 0; i < expressions.size(); ++i) {
			int result = expressions[i].init(file1, file2, file3, header1, header2, header3);
			if (result) {
				return result;
			}
		}
		return 0;
	}
	int cal(Row &record) {
		if (expressions.size() == 0) {
			for (int i = 0; i < record.items.size(); ++i) {
				record.items[i].var->print();
				cout << " ";
			}
			cout << endl;
			return 0;
		}
		for (int i = 0; i < expressions.size(); ++i) {
			int result = expressions[i].cal(record);
			if (result) {
				cout << endl;
				return result;
			}
			expressions[i].var->print();
			cout << " ";
		}
		cout << endl;
		return 0;
	}
	int agg(Row &record) {
		for (int i = 0; i < expressions.size(); ++i) {
			int result = expressions[i].agg(record, group);
			if (result) {
				return result;
			}
		}
	}
	int aggprint() {
		if (group >= 0) {
			return agggroup();
		}
		for (int i = 0; i < expressions.size(); ++i) {
			int result = expressions[i].aggprint();
			if (result) {
				return result;
			}
		}
		cout << endl;
	}
	int agggroup() {
		int index = 0;
		while (1) {
			for (int i = 0; i < expressions.size(); ++i) {
				int result = expressions[i].agggroupprint(group, index);
				if (result) {
					return 0;
				}
			}
			index++;
			cout << endl;
		}
	}
	int cal(Row &record1, Row &record2) {
		if (expressions.size() == 0) {
			for (int i = 0; i < record1.items.size(); ++i) {
				record1.items[i].var->print();
				cout << " ";
			}
			for (int i = 0; i < record2.items.size(); ++i) {
				record2.items[i].var->print();
				cout << " ";
			}
			cout << endl;
			return 0;
		}
		for (int i = 0; i < expressions.size(); ++i) {
			int result = expressions[i].cal(record1, record2);
			if (result) {
				cout << endl;
				return result;
			}
			expressions[i].var->print();
			cout << " ";
		}
		cout << endl;
		return 0;
	}
	int cal(Row &record1, Row &record2, Row &record3) {
		if (expressions.size() == 0) {
			for (int i = 0; i < record1.items.size(); ++i) {
				record1.items[i].var->print();
				cout << " ";
			}
			for (int i = 0; i < record2.items.size(); ++i) {
				record2.items[i].var->print();
				cout << " ";
			}
			for (int i = 0; i < record3.items.size(); ++i) {
				record3.items[i].var->print();
				cout << " ";
			}
			cout << endl;
			return 0;
		}
		for (int i = 0; i < expressions.size(); ++i) {
			int result = expressions[i].cal(record1, record2, record3);
			if (result) {
				cout << endl;
				return result;
			}
			expressions[i].var->print();
			cout << " ";
		}
		cout << endl;
		return 0;
	}
};

#endif
