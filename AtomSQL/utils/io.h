#ifndef DBMS_IO_H_
#define DBMS_IO_H_

#include "../filesystem/filesystem.h"
#include <stdlib.h>
#include <string.h>

class IO {
public:
	BufType writeInt(BufType b, int value) {
		memcpy(b, &value, sizeof(int));
		return (uint*) b + 1;
	}
	BufType readInt(BufType b, int &value) {
		value = b[0];
		return (uint*) b + 1;
	}
	BufType writeUInt(BufType b, uint value) {
		memcpy(b, &value, sizeof(uint));
		return (uint*) b + 1;
	}
	BufType readUInt(BufType b, uint &value) {
		value = b[0];
		return (uint*) b + 1;
	}
	BufType writeLongint(BufType b, ll value) {
		ull long_value = (ull) value;
		uint low = long_value & (((ull) 1 << 32) - 1);
		uint high = long_value >> 32;
		b = writeUInt(b, high);
		b = writeUInt(b, low);
		return b;
	}
	BufType readLongint(BufType b, ll &value) {
		uint high, low;
		b = readUInt(b, high);
		b = readUInt(b, low);
		value = (ll) (((ull) high << 32) + low);
		return b;
	}
	BufType writeULongint(BufType b, ull value) {
		uint low = value & (((ull) 1 << 32) - 1);
		uint high = value >> 32;
		b = writeUInt(b, high);
		b = writeUInt(b, low);
		return b;
	}
	BufType readULongint(BufType b, ull &value) {
		uint high, low;
		b = readUInt(b, high);
		b = readUInt(b, low);
		value = (((ull) high << 32) + low);
		return b;
	}
	BufType writeString(BufType b, string str, int length = 0) {
		if (length == 0) {
			length = str.length();
		}
		uint x = 0;
		for (int i = 0; i < (length + 3) / 4 * 4; ++i) {
			x = x << 8;
			if (i < length) {
				if (i < str.length()) {
					x = x + (uint) str[i];
				}
			}
			if ((i & 3) == 3) {
				memcpy(b, &x, sizeof(uint));
				b = (uint*) b + 1;
				x = 0;
			}
		}
		return b;
	}
	BufType readString(BufType b, string &str, int length) {
		int i = 0;
		int len = length;
		str = string(length, '\0');
		while (true) {
			uint x = b[0];
			for (int d = 3; d >= 0; --d) {
				str[i + d] = x & ((1 << 8) - 1);
				if ((str[i + d] == 0) && (len > i + d)) {
					len = i + d;
				}
				x = x >> 8;
			}
			b = (uint*) b + 1;
			i = i + 4;
			if (i >= length) {
				str = str.substr(0, len);
				return b;
			}
		}
	}
	void print(BufType b) {
		cout << endl << "================" << endl;
		for (int i = 0; i < 2048; ++i) {
			cout << b[i] << ' ';
			if ((i + 1) % 60 == 0) {
				cout << endl;
			}
		}
		cout << endl << "================" << endl;
	}
};

#endif
