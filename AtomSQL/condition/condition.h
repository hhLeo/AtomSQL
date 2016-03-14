#ifndef DBMS_CONDITION_H_
#define DBMS_CONDITION_H_

#include "../table/row.h"
#include "../table/columns.h"
#include "boolTerm.h"
#include "conditionOp.h"
#include <vector>

class Condition {

public:
	vector<int> bool_op;
	vector<BoolTerm> bool_terms;
	int getOp(string op) {
		if (op == "and") {
			return BOOL_OP_AND;
		}
		if (op == "or") {
			return BOOL_OP_OR;
		}
		return BOOL_OP_NULL;
	}
	int cal(Row &row, bool &term) {
		term = false;
		for (int i = 0; i < bool_terms.size(); ++i) {
			int result = bool_terms[i].cal(row, term);
			if (result) {
				return result;
			}
			if (i >= bool_terms.size() - 1) {
				continue;
			}
			if (bool_op[i] == BOOL_OP_AND) {
				if (term == false) {
					return 0;
				}
			}
			if (bool_op[i] == BOOL_OP_OR) {
				if (term == true) {
					return 0;
				}
			}
		}
		return 0;
	}
	int cal(Row &row1, Row &row2, bool &term) {
		term = false;
		for (int i = 0; i < bool_terms.size(); ++i) {
			int result = bool_terms[i].cal(row1, row2, term);
			if (result) {
				return result;
			}
			if (i >= bool_terms.size() - 1) {
				continue;
			}
			if (bool_op[i] == BOOL_OP_AND) {
				if (term == false) {
					return 0;
				}
			}
			if (bool_op[i] == BOOL_OP_OR) {
				if (term == true) {
					return 0;
				}
			}
		}
		return 0;
	}
	int cal(Row &row1, Row &row2, Row &row3, bool &term) {
		term = false;
		for (int i = 0; i < bool_terms.size(); ++i) {
			int result = bool_terms[i].cal(row1, row2, row3, term);
			if (result) {
				return result;
			}
			if (i >= bool_terms.size() - 1) {
				continue;
			}
			if (bool_op[i] == BOOL_OP_AND) {
				if (term == false) {
					return 0;
				}
			}
			if (bool_op[i] == BOOL_OP_OR) {
				if (term == true) {
					return 0;
				}
			}
		}
		return 0;
	}
	int init(Columns &header) {
		for (int i = 0; i < bool_terms.size(); ++i) {
			int result = bool_terms[i].init(header);
			if (result) {
				return result;
			}
		}
		return 0;
	}
	int init(string file1, string file2, Columns &header1, Columns &header2) {
		for (int i = 0; i < bool_terms.size(); ++i) {
			int result = bool_terms[i].init(file1, file2, header1, header2);
			if (result) {
				return result;
			}
		}
		return 0;
	}
	int init(string file1, string file2, string file3, Columns &header1, Columns &header2, Columns &header3) {
		for (int i = 0; i < bool_terms.size(); ++i) {
			int result = bool_terms[i].init(file1, file2, file3, header1, header2, header3);
			if (result) {
				return result;
			}
		}
		return 0;
	}
};

#endif
