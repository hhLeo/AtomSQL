#ifndef DBMS_VAR_H_
#define DBMS_VAR_H_

#include "../utils/io.h"
#include "typedef.h"

class Var {

public:
	uint length;

	virtual ~Var() {

	};

	virtual uint size() = 0;

	virtual uint type() = 0;

	virtual void print() = 0;

	virtual uint len() {
		return length;
	}

	virtual BufType write(BufType b, IO *io) = 0;

	virtual BufType read(BufType b, IO *io) = 0;
};

#endif
