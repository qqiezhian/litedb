#ifndef LITEDB_SKIPNODE_H
#define LITEDB_SKIPNODE_H
#endif

#include "dbtype.h"
#include "MemBlock.h"

namespace litedb {

#define SKIP_MAX_LEVEL (8)
#define SKIP_STEP (4)
#define META_BLOCK_SIZE (32 * 1024)
#define MAX_MEM_SIZE (10 * 1024 * 1024)


typedef enum {
	KTYPE_INSERT = 0,
	KTYPE_DELETE
}KTYPE;

#define SEQ_MASK   (0x0000ffffffffffff)
#define KTYPE_MASK (0xffff000000000000)
#define KTYPE_BIT (48)

typedef struct SkipListNode {
    Key* key;
	uint64* seqNumber;
    Value* data;
    uint32 level;
	bool isMeta;
	uint32 offset;
    SkipListNode **next_nodes;
} SkipListNode;

class SkipList {
public:
    SkipList(uint maxLevel);
    ~SkipList(void);
	void clean();
	bool canInsert(Key* key, Value* data);
    bool insertNode(Key* key, Value* data);
    void deleteNode(Key* key);
    Value* getData(Key* key);
    void displayList(void);
	bool write2File(char* fileName);
	bool readFile(char* fileName);
private:
	uint64 getSeqNum(KTYPE type);	
	void modSeqType(KTYPE type,uint64* seq);
	uint32 getSeqType(uint64 seq);
private:
    uint32 MAX_LEVEL;
	uint64 seq_;
	uint32 len_;
    uint RandomLevel(void);
    SkipListNode *head;
    SkipListNode *tail;
	MemBlock mem_;
};
//get new seq num
inline uint64 SkipList::getSeqNum(KTYPE type)
{
    return (((uint64)type) << KTYPE_BIT) | (++seq_);
}
//modify seq type
inline void SkipList::modSeqType(KTYPE type,uint64* seq)
{
    *(seq) = (((uint64)type) << KTYPE_BIT | (*(seq) & SEQ_MASK));
}
inline uint32 SkipList::getSeqType(uint64 seq)
{
    return (seq & KTYPE_MASK) >> KTYPE_BIT;
}
}


