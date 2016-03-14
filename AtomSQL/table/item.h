#ifndef DBMS_ITEM_H_
#define DBMS_ITEM_H_

#include "../var/varfactory.h"

class Item {

public:
	Var *var;
	bool isNull;

	Item() {
		var = NULL;
	}

};

#endif
