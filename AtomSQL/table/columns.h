#ifndef DBMS_COLUMNS_H_
#define DBMS_COLUMNS_H_

#include "column.h"
#include "tabledef.h"
#include "../var/typedef.h"
#include "../utils/io.h"
#include "../filesystem/filesystem.h"
#include <vector>

class Columns {

public:
	string schema;
	vector<Column> column;
	uint major;
	uint pages;
	uint size;
	uint num;

	Columns() {
		schema = "";
		major = 0;
		pages = 0;
		size = 0;
		num = 0;
		column.clear();
	}

	uint calSize() {
		size = TABLE_ITEM_NEXT_BYTE + TABLE_ITEM_RID_BYTE;
		size += (column.size() * TABLE_ITEM_NULL_BIT + 31) / 32 * 4;
		for (int i = 0; i < column.size(); ++i) {
			uint type = column[i].type;
			if (type == TYPE_LONGINT) {
				size += TABLE_ITEM_LONGINT_BYTE;
			} else if (type == TYPE_VARCHAR) {
				size += (uint) ((column[i].length + 3) / 4 * 4 * TABLE_ITEM_CHAR_BYTE);
			}
		}
		num = PAGE_SIZE / size;
		return size;
	}

	BufType writeColumns(BufType b, IO *io) {

		//依次写入模式信息、主键、页数
		b = io->writeString(b, schema, TABLE_HEADER_SCHEMA_BIT);
		b = io->writeUInt(b, major);
		b = io->writeUInt(b, pages);

		//依次写入每一个列项
		for (int i = 0; i < column.size(); ++i) {
			b = column[i].writeColumn(b, io);
		}
		return b;
	}

	BufType readColumns(BufType b, IO *io) {

		//依次读入模式信息、主键、页数
		b = io->readString(b, schema, TABLE_HEADER_SCHEMA_BIT);
		b = io->readUInt(b, major);
		b = io->readUInt(b, pages);
		//依次读取每一个列项
		while (true) {
			Column column_read;
			b = column_read.readColumn(b, io);
			if (b == NULL) {
				break;
			}
			column.push_back(column_read);
		}
		calSize();
		return b;
	}
};

#endif
