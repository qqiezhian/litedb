
#include <new>
#include "dbtype.h"
#include "MemBlock.h"

using namespace std;
using namespace litedb;

MemBlock::MemBlock(size_t s)
{
    if (0 == s)
		return;
    ptr_ = new(nothrow) char[s];
	if (NULL == ptr_)
	{
		return;
	}
	curPtr_ = ptr_;
	size_ = s;
	leftSize_ = size_;

	return;
}

MemBlock::~MemBlock()
{
    if (NULL != ptr_)
	{
		delete [] ptr_;
	}
	ptr_ = NULL;
	curPtr_ = ptr_;
	size_ = 0;
	leftSize_ = 0;
}

Value MemBlock::put(Value& v)
{
    size_t len = v.size();
    char* data = v.data();
    
    if (0 == len || NULLPTR == data)
		return Value();
	
	if (len > leftSize_)
		return Value();

	char* ptr = curPtr_;

	*(uint*)curPtr_ = len;
	curPtr_ += sizeof(uint);
	
	memcpy(curPtr_, data, len);

	curPtr_ += len;
	leftSize_ -= len;
	
	return Value(len,ptr);
}

