#ifndef __EXECUTE_H__
#define __EXECUTE_H__

#ifndef __cplusplus
extern "C" {
#endif

#include "../dbms/dbms.h"

Dbms *dbms;

void exe_start() {
	dbms = new Dbms();
}
int exe_create_database(const char *db_name) {
	return dbms->createDatabase(db_name);
}
int exe_drop_database(const char *db_name) {
	return dbms->dropDatabase(db_name);
}
int exe_use_database(const char *db_name) {
	return dbms->useDatabase(db_name);
}
int exe_create_table(const char *db_name, Columns &header) {
	return dbms->createTable(db_name, header);
}
int exe_drop_table(const char *db_name) {
	return dbms->dropTable(db_name);
}
int exe_desc_table(const char *db_name) {
	return dbms->descTable(db_name);
}
int exe_show_table() {
	return dbms->showTables();
}
int exe_insert(const char *db_name, Rows &records) {
	return dbms->insertRows(db_name, records);
}
int exe_delete(const char *db_name, Condition &condition) {
	return dbms->deleteRows(db_name, condition);
}
int exe_select(vector<string> db_names, Expressions &expressions, Condition &condition) {
	return dbms->selectRows(db_names, expressions, condition);
}
int exe_update(const char *db_name, Update &update, Condition &condition) {
	return dbms->updateRows(db_name, update, condition);
}
void exe_stop() {
	if (dbms != NULL) {
		delete dbms;
	}
}

#endif
