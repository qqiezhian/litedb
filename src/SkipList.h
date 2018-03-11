#ifndef LITEDB_SKIPNODE_H
#define LITEDB_SKIPNODE_H
#endif

#include "dbtype.h"
#include "MemBlock.h"

namespace litedb {

#define SKIP_STEP (4)
#define MAX_MEM_SIZE (100 * 1024)

typedef struct SkipListNode {
    Key key;
    Value data;
    uint level;
    SkipListNode **next_nodes;
} SkipListNode;

class SkipList {
public:
    SkipList(uint maxLevel);
    ~SkipList(void);
    void insertNode(Key& key, Value& data);
    void deleteNode(Key& key);
    Value getData(Key& key);
    void displayList(void);
private:
    uint MAX_LEVEL;
    uint RandomLevel(void);
    SkipListNode *head;
    SkipListNode *tail;
	MemBlock mem_;
};
}


