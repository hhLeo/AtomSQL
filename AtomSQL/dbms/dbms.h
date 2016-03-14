#ifndef DBMS_DBMS_H_
#define DBMS_DBMS_H_

#include "../filesystem/filesystem.h"
#include "../table/columns.h"
#include "../table/rows.h"
#include "../utils/document.h"
#include "../utils/io.h"
#include "../condition/condition.h"
#include "../expression/expressions.h"
#include "../expression/update.h"

class Dbms {

	Document *document;

	void error(int type, string dbName, string tableName) {
		if (type == 0) {
			return;
		}
		if (type == -1) {
			cout << dbName << " does not exist." << endl;
		}
		if (type == -2) {
			cout << "Please specify an existing database." << endl;
		}
		if (type == -3) {
			cout << tableName + dbtype << " does not exist." << endl;
		}
		if (type == -4) {
			cout << tableName + dbtype << " already exists." << endl;
		}
		if ((type < -5) && (type > -30)) {
			cout << "Error " << type << "." << endl;
		}
		if (type == -30) {
			cout << "Update error." << endl;
		}
	}

public:

	Dbms() {
		MyBitMap::initConst();
		document = new Document();
	}
	~Dbms() {
		if (document != NULL) {
			delete document;
		}
	}

	//创建数据库
	int createDatabase(string dbName) {
		int result = document->createDirectory(dbName);
		error(result, dbName, "");
		return result;
	}

	//删除数据库
	int dropDatabase(string dbName) {
		int result = document->removeDirectory(dbName);
		error(result, dbName, "");
		return result;
	}

	//切换数据库
	int useDatabase(string dbName) {
		int result = document->useDirectory(dbName);
		error(result, dbName, "");
		return result;
	}

	//列出当前数据库所有表
	int showTables() {
		int result = document->showDirectory();
		error(result, document->getDirectory(), "");
		return result;
	}

	//列出表的模式信息
	int descTable(string tableName) {
		int result = document->showFile(tableName);
		error(result, document->getDirectory(), tableName);
		return result;
	}

	//创建表
	int createTable(string tableName, Columns &header) {
		int result = document->createFile(tableName, header);
		error(result, document->getDirectory(), tableName);
		return result;
	}

	//删除表
	int dropTable(string tableName) {
		int result = document->dropFile(tableName);
		error(result, document->getDirectory(), tableName);
		return result;
	}

	//插入记录
	int insertRows(string tableName, Rows &rows) {
		int result = document->insertRows(tableName, rows);
		error(result, document->getDirectory(), tableName);
		return result;
	}

	//删除记录
	int deleteRows(string tableName, Condition &condition) {
		int result = document->deleteRows(tableName, condition);
		error(result, document->getDirectory(), tableName);
		return 0;
	}

	//更新记录
	int updateRows(string tableName, Update &update, Condition &condition) {
		int result = document->updateRows(tableName, update, condition);
		error(result, document->getDirectory(), tableName);
		return 0;
	}

	//查找记录
	int selectRows(vector<string> &tableNames, Expressions &expressions, Condition &condition) {
		int result = document->selectRows(tableNames, expressions, condition);
		error(result, document->getDirectory(), tableNames[0]);
		return 0;
	}

};

#endif
