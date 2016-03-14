#ifndef DBMS_LONGINT_H_
#define DBMS_LONGINT_H_

#include "var.h"

class Longint : public Var {

	ll value;

public:

	Longint(ll value_longint, uint length_max) {
		value = value_longint;
		length = length_max;
	}

	~Longint() {
	}

	uint size() {
		return TYPE_LONGINT_BYTE;
	}

	uint type() {
		return TYPE_LONGINT;
	}

	void print() {
		cout << value;
	}

	BufType write(BufType b, IO *io) {
		return io->writeLongint(b, value);
	}

	BufType read(BufType b, IO *io) {
		return io->readLongint(b, value);
	}

	ll get() {
		return value;
	}
};

#endif
