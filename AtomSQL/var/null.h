#ifndef DBMS_NULL_H_
#define DBMS_NULL_H_

#include "var.h"

class Null : public Var {

public:

	uint null_type;

	Null(uint length_max, uint nul_type) {
		length = length_max;
		null_type = nul_type;
	}

	~Null() {
	}

	uint size() {
		return length;
	}

	uint type() {
		return TYPE_NULL;
	}

	void print() {
		cout << "null";
	}

	BufType write(BufType b, IO *io) {
		if (null_type == TYPE_LONGINT) {
			return io->writeLongint(b, 0);
		}
		return io->writeString(b, "", length);
	}

	BufType read(BufType b, IO *io) {
		if (null_type == TYPE_LONGINT) {
			ll l;
			return io->readLongint(b, l);
		}
		string s;
		return io->readString(b, s, length);
	}
};

#endif
