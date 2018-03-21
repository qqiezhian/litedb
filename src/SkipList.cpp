#include <iostream>
#include <fstream>
#include "SkipList.h"

using namespace std;
using namespace litedb;

SkipList::SkipList(uint maxLevel):mem_(MAX_MEM_SIZE),seq_(0)
{
    MAX_LEVEL = maxLevel;
	head = new SkipListNode();
	tail = new SkipListNode();

	head->next_nodes = new SkipListNode *[MAX_LEVEL];
    for(uint i = 0; i < MAX_LEVEL; ++i)
        head->next_nodes[i] = tail;
}

SkipList::~SkipList()
{
    SkipListNode* curNode;

	curNode = head->next_nodes[0];
	while(curNode != tail)
	{
		head->next_nodes[0] = curNode->next_nodes[0];
		delete curNode->next_nodes;
		delete curNode;
		curNode = head->next_nodes[0];
	}
	delete head->next_nodes;
	delete head;
	delete tail;
}

void SkipList::insertNode(Key* key, Value* value)
{
	SkipListNode* tmp;

    SkipListNode* update[MAX_LEVEL];
    SkipListNode* curNode = head;

    for (int i = MAX_LEVEL - 1; i >= 0; i--)
    {
        while(curNode->next_nodes[i] != tail && 0 < sliceCompare(curNode->next_nodes[i]->key, key))
            curNode = curNode->next_nodes[i];
        if (curNode->next_nodes[i] != tail && 0 == sliceCompare(curNode->next_nodes[i]->key, key))
        {
	        tmp = curNode->next_nodes[i];
            *(tmp->seqNumber) = (uint64)(((uint64)KTYPE_DELETE) << KTYPE_BIT | (*(tmp->seqNumber) & SEQ_MASK));
            
	        tmp->key = mem_.put(key);
	        tmp->seqNumber = mem_.putSeq(((uint64)KTYPE_INSERT) << KTYPE_BIT | ++seq_);
            tmp->data = mem_.put(value);
            return;
        }
        update[i] = curNode;
    }

    // construct skiplistnode
    SkipListNode* node = new SkipListNode;
    node->key = mem_.put(key);
    node->seqNumber = mem_.putSeq(((uint64)KTYPE_INSERT) << KTYPE_BIT | ++seq_);
    node->data = mem_.put(value);
    node->level = RandomLevel();
    node->next_nodes = new SkipListNode* [node->level + 1];

    for (uint i=0; i<= node->level; i++)
    {
        node->next_nodes[i] = update[i]->next_nodes[i];
        update[i]->next_nodes[i] = node;
    }

    return;
}

Value* SkipList::getData(Key* key)
{
    SkipListNode* curNode = head;

    for (int i = MAX_LEVEL - 1; i >= 0; i--)
    {
        while(curNode->next_nodes[i] != tail && 0 < sliceCompare(curNode->next_nodes[i]->key, key))
            curNode = curNode->next_nodes[i];
        if (curNode->next_nodes[i] != tail && 0 == sliceCompare(curNode->next_nodes[i]->key, key))
        {
            return curNode->next_nodes[i]->data;
        }
    }

    return NULL;
}


void SkipList::deleteNode(Key* key)
{
	SkipListNode* update[MAX_LEVEL];
    SkipListNode* curNode = head;

    for (int i = MAX_LEVEL - 1; i >= 0; i--)
    {
        while(curNode->next_nodes[i] != tail && 0 < sliceCompare(curNode->next_nodes[i]->key, key))
            curNode = curNode->next_nodes[i];
        if (curNode->next_nodes[i] != tail && 0 == sliceCompare(curNode->next_nodes[i]->key, key))
        {
            update[i] = curNode;
        }
        else
        {
            update[i] = NULL;
        }
    }
    SkipListNode* tmp;
    for (uint i = 0; i < MAX_LEVEL; i++)
    {
        if (NULL != update[i])
        {
            tmp = update[i]->next_nodes[i];
            update[i]->next_nodes[i] = tmp->next_nodes[i];
        }
    }

    if (NULL != tmp)
    {
	    *(tmp->seqNumber) = (((uint64)KTYPE_DELETE) << KTYPE_BIT | (*(tmp->seqNumber) & SEQ_MASK));
        delete tmp->next_nodes;
        delete tmp;
    }
    return;
}

void SkipList::displayList()
{
	uint num = 0;
	uint32 len;
	char* ptr;
	char outStr[20] = {0};
    SkipListNode* curNode = head->next_nodes[0];
    while(tail != curNode)
    {
		num++;
		ptr = (char*)curNode->key;
		len = *(uint32*)ptr;
		cout << "key len:" << len << endl;
		ptr += sizeof(uint32);
		strncpy(outStr, ptr, len);
		outStr[len] = '\0';
		cout << "key data:" << outStr << endl;

		ptr = (char*)curNode->seqNumber;
		cout << "seqNum:" << *(uint64*)ptr << endl;

		ptr = (char*)curNode->data;
		len = *(uint32*)ptr;
		cout << "data len:" << len << endl;
		ptr += sizeof(uint32);
		strncpy(outStr, ptr, len);
		outStr[len] = '\0';
		cout << "data data:" << outStr << endl;
		
        curNode = curNode->next_nodes[0];
    }
    cout<<"num:"<<num<<endl;
}

uint SkipList::RandomLevel(void)
{
	uint level = 0;
	while(rand() % SKIP_STEP && level < MAX_LEVEL - 1)
	    ++level;
	return level;
}

bool SkipList::write2File(char* fileName)
{
    if (NULL == fileName)
        return false;
    if (NULL == mem_.getMemBlock())
        return false;
    uint32 len = 0;
    char* ptr;
    ofstream fout;
    fout.open(fileName, ios::binary);

    SkipListNode* curNode = head->next_nodes[0];
    while(tail != curNode)
    {
        len = sizeof(uint32) + curNode->key->size_
	        + sizeof(uint64) + sizeof(uint32) + curNode->data->size_;
        ptr = (char*)curNode->key;
        fout.write(ptr,len);
        curNode = curNode->next_nodes[0];
    }
    //fout.write(mem_.getMemBlock(), mem_.activeSize());
    fout.close();
    return true;
}

/*
bool SkipList::readFile(char* fileName)
{
    if (NULL == fileName)
        return false;
    if (NULL != mem_.getMemBlock())
        return false;
    SkipListNode* tmp;
    Key key;
    Value value;
    char* ptr;
    ifstream fin;
    fin.open(fileName, ios::binary);

    while (!fin.eof())
    {
        ptr = mem_.curPtr_;
        fin.read(ptr, sizeof(key.size_));
        key.size_ = *(uint32*)ptr;
        ptr += sizeof(key.size_);

        fin.read(ptr, key.size_);
        key.data_ = ptr;
        ptr += key.size_;


        fin. read(ptr, sizeof(value.size_));
        value.size_ = *(uint32*)ptr;
        ptr += sizeof(value.size_);

        fin.read(ptr, value.size_);
        value.data_ = ptr;
        ptr += value.size_;
        
        mem_.curPtr_ = ptr;
        mem_.leftSize_ -= sizeof(key.size_) + key.size_ + sizeof(value.size_) + value.size_;

        
    }
    
    //fin.write(mem_.getMemBlock(), mem_.activeSize());
    fin.close();

    
    return true;
}
*/
