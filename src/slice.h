#ifndef LITEDB_SLICE_H
#define LITEDB_SLICE_H
#endif

#include "dbtype.h"
#include <iostream>
using namespace std;
namespace litedb {

typedef struct {
	uint32 size_;
	char data_[0];
}Slice;

typedef Slice Key;
typedef Slice Value;

//Slice* constructSlice(char* s);
//bool empty(Slice* s);
//int sliceCompare(Slice* a, Slice* b);
inline Slice* constructSlice(uint32 len, char* s)
{
    Slice* p;
	
	p = (Slice*)new(nothrow) char[sizeof(uint32) + len];
	if (NULL == p) return NULL;

	*(uint32*)p = len;
	
	strncpy(p->data_, s, len);
	return p;
}


inline Slice* constructSlice(char* s)
{
    Slice* p;
	uint len = strlen(s);
	
	p = (Slice*)new(nothrow) char[sizeof(uint32) + len];
	if (NULL == p) return NULL;

	*(uint32*)p = len;
	
	strncpy(p->data_, s, len);
	return p;
}

inline bool empty(Slice* s)
{
    return 0 == s->size_;
}

inline int sliceCompare(Slice* a, Slice* b)
{
    if (a->size_ < b->size_)
		return -1;
	else if (a->size_ > b->size_)
		return 1;
	return strncmp(a->data_,b->data_,a->size_);
}

inline bool isGreater(Slice* a, Slice* b)
{
    return (0 < sliceCompare(a,b));
}

inline bool isLesser(Slice* a, Slice* b)
{
    return (0 > sliceCompare(a,b));
}

inline bool isEqual(Slice* a, Slice* b)
{
    return (0 == sliceCompare(a,b));
}

}
