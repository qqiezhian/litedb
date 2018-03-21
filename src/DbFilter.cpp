
#include "DbFilter.h"

using namespace std;
using namespace litedb;

DbFilter::DbFilter(uint m, uint k): m_(m), k_(k)
{
    uint size_ = m_ / 8 + (0 == m_ % 8) ? 0 : 1;

	bits_ = new(nothrow) uchar [size_];
	if (NULL == bits_)
	{
	    return;
	}

	init_seeds();
}

void DbFilter::init_seeds()
{
    seeds_ = new uint [k_];
	seeds_[0] = 13;
	for (uint i = 1; i < k_; i++)
	{
	    uint j = (0 == i % 2) ? 1 : 3;
	    seeds_[i] += seeds_[i-1] * 10 + j;
	}
}

uint DbFilter::BKDR_Hash(uint hashIdx, Key& key)
{
	uint hashV = 0;
    char* data = key.data_;
	uint size = key.size_;

	for (uint i = 0; i < size; i++)
	{
		hashV += hashV * seeds_[hashIdx] + data[i];
	}
	return (hashV % m_);
}

void DbFilter::insertEle(Key& key)
{
	for (uint i = 0; i < k_; i++)
	{
		setBit(BKDR_Hash(i, key));
	}
}


bool DbFilter::checkEle(Key& key)
{
    for (uint i = 0; i < k_; i++)
	{
		if(!getBit(BKDR_Hash(i, key)))
		{
			return false;
		}
	}
	return true;
}




