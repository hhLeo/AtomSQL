#ifndef DBMS_COLUMN_H_
#define DBMS_COLUMN_H_

#include "../var/typedef.h"
#include "../utils/io.h"
#include "tabledef.h"

class Column {

public:
	uint type;
	uint length;
	bool canNull;
	string name;

	//写入列项
	BufType writeColumn(BufType b, IO *io) {

		//依次写入类型、大小、是否可空、名字
		uint x = type;
		x = x << TABLE_HEADER_LENGTH_BIT;
		x = x + (length & ((1 << TABLE_HEADER_LENGTH_BIT) - 1));
		x = x << TABLE_HEADER_NULL_BIT;
		x = x + canNull;
		b = io->writeUInt(b, x);
		b = io->writeString(b, name, TABLE_HEADER_NAME_BIT);
		return b;
	}

	//读取列项
	BufType readColumn(BufType b, IO *io) {

		//依次读入类型、大小、是否可空、名字
		uint x;
		b = io->readUInt(b, x);
		if (x == 0) {
			return NULL;
		}
		canNull = x & ((1 << TABLE_HEADER_NULL_BIT) - 1);
		x = x >> TABLE_HEADER_NULL_BIT;
		length = x & ((1 << TABLE_HEADER_LENGTH_BIT) - 1);
		x = x >> TABLE_HEADER_LENGTH_BIT;
		type = x & ((1 << TABLE_HEADER_TYPES_BIT) - 1);
		b = io->readString(b, name, TABLE_HEADER_NAME_BIT);
		return b;
	}
};

#endif
