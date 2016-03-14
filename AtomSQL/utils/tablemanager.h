#ifndef DBMS_TABLEMANAGER_H_
#define DBMS_TABLEMANAGER_H_

#include "io.h"
#include "../table/tabledef.h"
#include "../table/columns.h"
#include "../condition/condition.h"
#include "../expression/expressions.h"
#include "../expression/update.h"

class TableManager {

	BufPageManager *bpm;
	IO *io;

public:

	TableManager (BufPageManager *bpm, IO *io) {
		this->bpm = bpm;
		this->io = io;
	}

	~TableManager() {
	}

	//写入表的元数据
	void writeHeader(int fileID, int pageID, Columns &header, bool alloc = true) {
		int index;

		//申请或者获得元数据页
		BufType b;
		if (alloc) {
			b = bpm->allocPage(fileID, pageID, index, true);
		}
		else {
			b = bpm->getPage(fileID, pageID, index);
		}
		bpm->markDirty(index);
		b = header.writeColumns(b, io);
	}

	//读入表的元数据
	Columns readHeader(int fileID, int pageID) {
		int index;

		//读取元数据页
		BufType b = bpm->getPage(fileID, pageID, index);\
		bpm->access(index);
		Columns header;
		b = header.readColumns(b, io);
		return header;
	}

	//插入记录
	int insertRecord(int fileID, Columns &header, Rows &records) {
		int pageID = 1;
		int record = 0;
		while (record < records.rows.size()) {
			Rows page;
			if (pageID > header.pages) {
				//需要申请新的页面
				page.newPage(bpm, io, fileID, pageID, header);
				writeHeader(fileID, 0, header, false);
			}
			else {
				page.read(bpm, io, fileID, pageID, header);
			}
			//当前页面已满
			if (page.counts == 0) {
				page.release();
				pageID++;
				continue;
			}
			//寻找空槽
			for (int i = 0; i < header.num; ++i) {
				if (page.rows[i].next != 1) {
					continue;
				}
				//当前槽为空槽
				page.rows[i] = records.rows[record];
				page.rows[i].rid = (pageID - 1) * header.num + i;
				int j = i;
				do {
					if ((j < header.num - 1) && (page.rows[j + 1].next != 0)) {
						page.rows[j].next = page.rows[j + 1].next + 1;
					}
					else {
						page.rows[j].next = 0;
					}
					j--;
				}
				while ((j >= 0) && (page.rows[j].next != 1));
				page.counts--;
				record++;
				if (record >= records.rows.size()) {
					break;
				}
			}
			page.write(bpm, io, fileID, pageID, header);
		}
		return 0;
	}

	//删除记录
	int deleteRecord(int fileID, Columns &header, vector<int> &delta) {
		int pageID = 1;
		int record = 0;
		while ((record < delta.size()) && (pageID <= header.pages)) {
			int pageDel = delta[record] / header.num + 1;
			//当前页没有要删除的记录
			if (pageDel != pageID) {
				pageID++;
				continue;
			}
			//获取当前页的记录
			Rows page;
			page.read(bpm, io, fileID, pageID, header);
			int del = delta[record] % header.num;
			//待删除记录为空
			if (page.rows[del].next == 1) {
				record++;
				continue;
			}
			//删除记录
			page.rows[del].next = 1;
			//更新next指针
			for (int i = del - 1; i >= 0; --i) {
				if (page.rows[i].next == 1) {
					break;
				}
				page.rows[i].next = page.rows[i + 1].next + 1;
			}
			page.counts++;
			page.write(bpm, io, fileID, pageID, header);
			record++;
		}
		return 0;
	}

	//更新记录
	int updateRecord(int fileID, Columns &header, vector<int> &delta, Update &update) {
		int pageID = 1;
		int record = 0;
		while ((record < delta.size()) && (pageID <= header.pages)) {
			int pageUpd = delta[record] / header.num + 1;
			//当前页没有要更新的记录
			if (pageUpd != pageID) {
				pageID++;
				continue;
			}
			//获取当前页的记录
			Rows page;
			page.read(bpm, io, fileID, pageID, header);
			int upd = delta[record] % header.num;
			//待更新记录为空
			if (page.rows[upd].next == 1) {
				record++;
				continue;
			}
			//更新记录
			update.cal(page.rows[upd]);
			page.write(bpm, io, fileID, pageID, header);
			record++;
		}
		return 0;
	}

	int aggRecord(int fileID, Columns &header, Expressions expressions) {
		int pageID = 1;
		while (pageID <= header.pages) {
			Rows page;
			page.read(bpm, io, fileID, pageID, header);
			for (int i = 0; i < header.num; ++i) {
				if (page.rows[i].next != 1) {
					expressions.agg(page.rows[i]);
				}
			}
			pageID++;
		}
		expressions.aggprint();
		return 0;
	}

	//选择记录
	int selectRecord(int fileID, Columns &header, vector<int> &delta, Expressions expressions) {
		int pageID = 1;
		int record = 0;
		while ((record < delta.size()) && (pageID <= header.pages)) {
			int pageSel = delta[record] / header.num + 1;
			//当前页没有要选择的记录
			if (pageSel != pageID) {
				pageID++;
				continue;
			}
			//获取当前页的记录
			Rows page;
			page.read(bpm, io, fileID, pageID, header);
			int sel = delta[record] % header.num;
			//待选择记录为空
			if (page.rows[sel].next == 1) {
				record++;
				continue;
			}
			expressions.cal(page.rows[sel]);
			record++;
		}
		return 0;
	}

	//选择记录
	int selectRecord(int fileID1, int fileID2, Columns &header1, Columns &header2, vector<int> &delta1, vector<int> &delta2, Expressions expressions) {
		int record = 0;
		while (record < delta1.size()) {
			int pageID1 = delta1[record] / header1.num + 1;
			if (pageID1 > header1.pages) {
				break;
			}
			int pageID2 = delta2[record] / header2.num + 1;
			if (pageID2 > header2.pages) {
				break;
			}
			//获取当前页的记录
			Rows page1;
			Rows page2;
			page1.read(bpm, io, fileID1, pageID1, header1);
			page2.read(bpm, io, fileID2, pageID2, header2);
			int sel1 = delta1[record] % header1.num;
			int sel2 = delta2[record] % header2.num;
			//待选择记录为空
			if (page1.rows[sel1].next == 1) {
				page1.release();
				page2.release();
				record++;
				continue;
			}
			if (page2.rows[sel2].next == 1) {
				page1.release();
				page2.release();
				record++;
				continue;
			}
			expressions.cal(page1.rows[sel1], page2.rows[sel2]);
			page1.release();
			page2.release();
			record++;
		}
		return 0;
	}

	//选择记录
	int selectRecord(int fileID1, int fileID2, int fileID3, Columns &header1, Columns &header2, Columns &header3, vector<int> &delta1, vector<int> &delta2, vector<int> &delta3, Expressions expressions) {
		int record = 0;
		while (record < delta1.size()) {
			int pageID1 = delta1[record] / header1.num + 1;
			if (pageID1 > header1.pages) {
				break;
			}
			int pageID2 = delta2[record] / header2.num + 1;
			if (pageID2 > header2.pages) {
				break;
			}
			int pageID3 = delta3[record] / header3.num + 1;
			if (pageID3 > header3.pages) {
				break;
			}
			//获取当前页的记录
			Rows page1;
			Rows page2;
			Rows page3;
			page1.read(bpm, io, fileID1, pageID1, header1);
			page2.read(bpm, io, fileID2, pageID2, header2);
			page3.read(bpm, io, fileID3, pageID3, header3);
			int sel1 = delta1[record] % header1.num;
			int sel2 = delta2[record] % header2.num;
			int sel3 = delta3[record] % header3.num;
			//待选择记录为空
			if (page1.rows[sel1].next == 1) {
				page1.release();
				page2.release();
				page3.release();
				record++;
				continue;
			}
			if (page2.rows[sel2].next == 1) {
				page1.release();
				page2.release();
				page3.release();
				record++;
				continue;
			}
			expressions.cal(page1.rows[sel1], page2.rows[sel2], page3.rows[sel3]);
			page1.release();
			page2.release();
			page3.release();
			record++;
		}
		return 0;
	}

	//查找待删除/更新/选择记录
	int recordFind(int fileID, Columns &header, Condition &condition, vector<int> &delta) {
		int pageID = 1;
		int record = 0;
		delta.clear();
		while (pageID <= header.pages) {
			//获取当前页的记录
			Rows page;
			page.read(bpm, io, fileID, pageID, header);
			for (int i = 0; i < header.num; ++i) {
				if (page.rows[i].next == 1) {
					record++;
					continue;
				}
				bool term;
				int result = condition.cal(page.rows[i], term);
				if (result) {
					return result;
				}
				if (term) {
					delta.push_back(record);
				}
				record++;
			}
			page.release();
			pageID++;
		}
		return 0;
	}

	//查找待删除/更新/选择记录
	int recordFind(int fileID1, int fileID2, Columns &header1, Columns &header2, Condition &condition, vector<int> &delta1, vector<int> &delta2) {
		delta1.clear();
		delta2.clear();
		int pageID1 = 1;
		int record1 = 0;
		while (pageID1 <= header1.pages) {
			// cout << "page1 = " << pageID1 << endl;
			//获取当前页的记录
			Rows page1;
			page1.read(bpm, io, fileID1, pageID1, header1);
			for (int i = 0; i < header1.num; ++i) {
				if (page1.rows[i].next == 1) {
					record1++;
					continue;
				}
				int pageID2 = 1;
				int record2 = 0;
				while (pageID2 <= header2.pages) {
					Rows page2;
					page2.read(bpm, io, fileID2, pageID2, header2);
					for (int j = 0; j < header2.num; ++j) {
						if (page2.rows[j].next == 1) {
							record2++;
							continue;
						}
						bool term;
						int result = condition.cal(page1.rows[i], page2.rows[j], term);
						if (result) {
							return result;
						}
						if (term) {
							delta1.push_back(record1);
							delta2.push_back(record2);
						}
						record2++;
					}
					page2.release();
					pageID2++;
				}
				record1++;
			}
			page1.release();
			pageID1++;
		}
		return 0;
	}

	//查找待删除/更新/选择记录
	int recordFind(int fileID1, int fileID2, int fileID3, Columns &header1, Columns &header2, Columns &header3, Condition &condition, vector<int> &delta1, vector<int> &delta2, vector<int> &delta3) {
		delta1.clear();
		delta2.clear();
		delta3.clear();
		int pageID1 = 1;
		int record1 = 0;
		while (pageID1 <= header1.pages) {
			// cout << "page1 = " << pageID1 << endl;
			//获取当前页的记录
			Rows page1;
			page1.read(bpm, io, fileID1, pageID1, header1);
			for (int i = 0; i < header1.num; ++i) {
				if (page1.rows[i].next == 1) {
					record1++;
					continue;
				}
				int pageID2 = 1;
				int record2 = 0;
				while (pageID2 <= header2.pages) {
					Rows page2;
					page2.read(bpm, io, fileID2, pageID2, header2);
					for (int j = 0; j < header2.num; ++j) {
						if (page2.rows[j].next == 1) {
							record2++;
							continue;
						}
						int pageID3 = 1;
						int record3 = 0;
						while (pageID3 <= header3.pages) {
							Rows page3;
							page3.read(bpm, io, fileID3, pageID3, header3);
							for (int k = 0; k < header3.num; ++k) {
								if (page3.rows[k].next == 1) {
									record3++;
									continue;
								}
							
								bool term;
								int result = condition.cal(page1.rows[i], page2.rows[j], page3.rows[k], term);
								if (result) {
									return result;
								}
								if (term) {
									delta1.push_back(record1);
									delta2.push_back(record2);
									delta3.push_back(record3);
								}
								record3++;
							}
							page3.release();
							pageID3++;
						}
						record2++;
					}
					page2.release();
					pageID2++;
				}
				record1++;
			}
			page1.release();
			pageID1++;
		}
		return 0;
	}
};

#endif
