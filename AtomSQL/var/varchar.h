#ifndef DBMS_VARCHAR_H_
#define DBMS_VARCHAR_H_

#include "var.h"

class Varchar : public Var {

	string value;

public:

	Varchar(string value_string, uint length_max) {
		value = value_string;
		length = length_max;
	}

	~Varchar() {
	}

	uint size() {
		return TYPE_CHAR_BYTE * length;
	}

	uint type() {
		return TYPE_VARCHAR;
	}

	void print() {
		cout << value;
	}

	BufType write(BufType b, IO *io) {
		return io->writeString(b, value, length);
	}

	BufType read(BufType b, IO *io) {
		return io->readString(b, value, length);
	}

	string get() {
		return value;
	}
};

#endif
