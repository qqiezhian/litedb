#ifndef LITEDB_MEMBLOCK_H
#define LITEDB_MEMBLOCK_H
#endif

#include "dbtype.h"
#include "Slice.h"

namespace litedb {

class MemBlock {

private:
	uint size_;
	uint leftSize_;
	char* ptr_;
	char* curPtr_;
public:
	MemBlock() : size_(0), leftSize_(0), ptr_(0), curPtr_(0) {}
	MemBlock(size_t s);
	~MemBlock();
	uint size() {return size_;}
	Slice put(Slice& s); 
};
}
