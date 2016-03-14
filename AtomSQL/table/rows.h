#ifndef DBMS_ROWS_H_
#define DBMS_ROWS_H_

#include "row.h"
#include "columns.h"

class Rows {

public:
	unsigned int counts;
	vector<Row> rows;

	//申请新的页面，该操作会修改元数据
	void newPage(BufPageManager *bpm, IO *io, int fileID, int pageID, Columns &header) {
		int index;
		//修改元数据
		BufType b = bpm->getPage(fileID, 0, index);
		bpm->markDirty(index);
		header.pages = header.pages + 1;
		header.writeColumns(b, io);
		//申请新的页面
		b = bpm->allocPage(fileID, pageID, index, true);
		bpm->markDirty(index);
		counts = header.num;
		b = io->writeUInt(b, counts);
		for (int i = 0; i < header.num; ++i) {
			Row row;
			row.next = 1;
			row.rid = (pageID - 1) * header.num + i;
			b = row.write(b, io, header);
			rows.push_back(row);
		}
	}

	//在已有页面上写记录
	void write(BufPageManager *bpm, IO *io, int fileID, int pageID, Columns &header) {
		if (pageID > header.pages) {
			return;
		}
		int index;
		BufType b = bpm->getPage(fileID, pageID, index);
		bpm->markDirty(index);
		b = io->writeUInt(b, counts);
		for (int i = 0; i < header.num; ++i) {
			b = rows[i].write(b, io, header);
		}
	}

	//在已有页面上读记录
	void read(BufPageManager *bpm, IO *io, int fileID, int pageID, Columns &header) {
		if (pageID > header.pages) {
			return;
		}
		int index;
		BufType b = bpm->getPage(fileID, pageID, index);
		bpm->access(index);
		b = io->readUInt(b, counts);
		rows.clear();
		for (int i = 0; i < header.num; ++i) {
		 	Row row;
		 	b = row.read(b, io, header);
		 	rows.push_back(row);
		}
	}

	void release() {
		for (int i = 0; i < rows.size(); ++i) {
			rows[i].release();
		}
	}

	void print(BufPageManager *bpm, IO *io, int fileID, int pageID) {
		int index;
		BufType b = bpm->getPage(fileID, pageID, index);
		io->print(b);
	}

	void clear() {
		vector<Row> temp;
		temp.swap(rows);
	}

	~Rows() {
		clear();
	}
};

#endif
