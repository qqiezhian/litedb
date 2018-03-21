#ifndef LITEDB_DBFILTER_H
#define LITEDB_DBFILTER_H
#endif

#include "dbtype.h"
#include "Slice.h"
#include <iostream>
using namespace std;
namespace litedb {

class DbFilter {
public:
	DbFilter():m_(0), k_(0), bits_(NULL), seeds_(0) {}
	DbFilter(uint m, uint k);
	void insertEle(Key& key);
	bool checkEle(Key& key);
private:
	uint m_;
	uint k_;
	uchar* bits_;
	uint* seeds_;
	void init_seeds();
	void setBit(uint i);
	bool getBit(uint i);
	uint BKDR_Hash(uint hashIdx, Key& key);
};


inline void DbFilter::setBit(uint i)
{
    //uint byteSeq = i / 8;
	//uint bitSeq = i % 8;
	bits_[i / 8] |= 0x1 << (i % 8);
}

inline bool DbFilter::getBit(uint i)
{
    //uint byteSeq = i / 8;
	//uint bitSeq = i % 8;
	return (0 != (bits_[i / 8] & (0x1 << (i % 8))));
}


}
