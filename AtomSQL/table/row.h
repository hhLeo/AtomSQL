#ifndef DBMS_ROW_H_
#define DBMS_ROW_H_

#include "tabledef.h"
#include "item.h"
#include "columns.h"
#include <vector>

class Row {

public:
	vector<Item> items;
	ull rid;
	uint next;

	//写记录
	BufType write(BufType b, IO *io, Columns &header) {

		//写入next值
		b = io->writeUInt(b, next);

		//写入rid值
		b = io->writeULongint(b, rid);

		if (next == 1) {
			for (int i = 0; i < (header.size - TABLE_ITEM_NEXT_BYTE - TABLE_ITEM_RID_BYTE) / sizeof(uint); ++i) {
				b = io->writeUInt(b, 0);
			}
			return b;
		}

		//写入每一列项是否为空
		uint nulls = 0;
		for (int i = 0; i < items.size(); ++i) {
			nulls += ((ull) items[i].isNull << (TABLE_ITEM_NULL_BIT * i));
			if (((i + 1) % 32 == 0) || (i == items.size() - 1)) {
				b = io->writeUInt(b, nulls);
				nulls = 0;
			}
		}

		//写入每一列项数据
		for (int i = 0; i < items.size(); ++i) {
			b = items[i].var->write(b, io);
		}
		return b;
	}

	//读记录
	BufType read(BufType b, IO *io, Columns &header) {

		//读入next值
		b = io->readUInt(b, next);

		//读入rid值
		b = io->readULongint(b, rid);

		//当前槽为空槽
		if (next == 1) {
			return b + (header.size - TABLE_ITEM_NEXT_BYTE - TABLE_ITEM_RID_BYTE) / sizeof(uint);
		}
		items.clear();
		//读入每一列项是否为空
		for (int i = 0; i < (header.column.size() + 31) / 32; ++i) {
			uint nulls = 0;
			b = io->readUInt(b, nulls);
			for (int j = i * 32; j < (i + 1) * 32; ++j) {
				if (j >= header.column.size()) {
					break;
				}
				Item item;
				item.isNull = nulls % (1 << TABLE_ITEM_NULL_BIT);
				nulls = nulls >> TABLE_ITEM_NULL_BIT;
				items.push_back(item);
			}
		}

		//读入每一列项的数据
		for (int i = 0; i < header.column.size(); ++i) {
			if (!items[i].isNull) {
				b = VarFactory::get(b, io, &items[i].var, header.column[i].type, header.column[i].length);
			}
			else {
				b = VarFactory::get(b, io, &items[i].var, TYPE_NULL, header.column[i].length, header.column[i].type);
			}
		}
		return b;
	}

	void release() {
		for (int i = 0; i < items.size(); ++i) {
			if (items[i].var != NULL) {
				delete items[i].var;
				items[i].var = NULL;
			}
		}
	}

	//移除记录
	BufType remove(BufType b, IO *io, Columns &header) {

		//将next槽指向自己，表明该槽已空
		next = 1;
		b = io->writeUInt(b, next);

		//rid值不变
		b = io->writeUInt(b, rid);

		//其余部分用零填充
		for (int i = 0; i < (header.column.size() - TABLE_ITEM_NEXT_BYTE) / sizeof(uint); ++i) {
			b = io->writeUInt(b, 0);
		}
		return b;
	}

	//更新记录
	BufType update(BufType b, IO *io) {

		//next指针不变
		b = b + TABLE_ITEM_NEXT_BYTE / sizeof(uint);

		//写入rid值
		b = io->writeULongint(b, rid);

		//写入每一列项是否为空
		uint nulls = 0;
		for (int i = 0; i < items.size(); ++i) {
			nulls += ((ull) items[i].isNull << (TABLE_ITEM_NULL_BIT * i));
			if (((i + 1) % 32 == 0) || (i == items.size() - 1)) {
				b = io->writeUInt(b, nulls);
				nulls = 0;
			}
		}

		//写入每一列项数据
		for (int i = 0; i < items.size(); ++i) {
			b = items[i].var->write(b, io);
		}
		return b;
	}

	void clear() {
		vector<Item> temp;
		temp.swap(items);
		rid = 0;
	}

	~Row() {
		clear();
	}
};

#endif
