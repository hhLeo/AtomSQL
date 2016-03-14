#ifndef DBMS_UPDATE_H_
#define DBMS_UPDATE_H_

#include "../table/row.h"
#include "../table/columns.h"
#include "expression.h"

class Update {

public:
	Expression expression;
	string column_name;
	uint column_index;

	int init(Columns &header) {
		column_index = header.column.size();
		for (int i = 0; i < header.column.size(); ++i) {
			if (header.column[i].name == column_name) {
				column_index = i;
				break;
			}
		}
		if (column_index == header.column.size()) {
			return -30;
		}
		int result = expression.init(header);
		return result;
	}
	int cal(Row &record) {
		int result = expression.cal(record);
		if (result) {
			return result;
		}
		expression.var->length = record.items[column_index].var->length;
		if (expression.var->type() != TYPE_NULL) {
			record.items[column_index].var = expression.var;
			record.items[column_index].isNull = false;
		}
		return 0;
	}
};

#endif
