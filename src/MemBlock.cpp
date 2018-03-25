
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
void MemBlock::clean()
{
    curPtr_ = ptr_;
    leftSize_ = size_;
    memset(ptr_, 0, size_);
}
bool MemBlock::canInsert(uint32 len)
{
    return (leftSize_ >= len);
}
Value* MemBlock::put(Value* v)
{
    size_t len = v->size_ + sizeof(uint32);
    char* data = v->data_;
    
    if (0 == len || NULLPTR == data)
		return NULLPTR;
	
	if (len > leftSize_)
		return NULLPTR;

	char* ptr = curPtr_;

	memcpy(curPtr_, v, len);

	curPtr_ += len;
	leftSize_ -= len;
	
	return (Value*)ptr;
}

uint64* MemBlock::putSeq(uint64 seq)
{
	if (sizeof(uint64) > leftSize_)
		return NULLPTR;

	char* ptr = curPtr_;

	*(uint64*)ptr = seq;

	curPtr_ += sizeof(uint64);
	leftSize_ -= sizeof(uint64);
	
	return (uint64*)ptr;
}

