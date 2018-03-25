#ifndef LITEDB_MEMBLOCK_H
#define LITEDB_MEMBLOCK_H
#endif

#include "dbtype.h"
#include "Slice.h"

namespace litedb {

class MemBlock {

private:
	uint32 size_;
	uint32 leftSize_;
	char* ptr_;
	char* curPtr_;
public:
	MemBlock() : size_(0), leftSize_(0), ptr_(0), curPtr_(0) {}
	MemBlock(size_t s);
	~MemBlock();
	void clean();
	bool canInsert(uint32 len);
	uint32 size() {return size_;}
	uint32 activeSize() {return size_ - leftSize_;}
	char* getMemBlock() {return ptr_;}
	Slice* put(Slice* s);
	uint64* putSeq(uint64 seq);
};
}
