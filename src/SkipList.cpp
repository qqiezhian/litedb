#include <iostream>
#include <fstream>
#include "SkipList.h"

using namespace std;
using namespace litedb;

SkipList::SkipList(uint maxLevel):mem_(MAX_MEM_SIZE),seq_(0),len_(0)
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
    clean();
	delete head;
	delete tail;
}
void SkipList::clean()
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

	mem_.clean();
	len_ = 0;
}
bool SkipList::canInsert(Key* key, Value* data)
{
    uint32 len = sizeof(Key) + key->size_ 
		    + sizeof(uint64) + sizeof(data) + data->size_;
    
    return mem_.canInsert(len);
}


bool SkipList::insertNode(Key* key, Value* value)
{
	uint32 activeSize;
	SkipListNode* tmp;
    SkipListNode* update[MAX_LEVEL];
    SkipListNode* curNode = head;

    for (int i = MAX_LEVEL - 1; i >= 0; i--)
    {
        while(curNode->next_nodes[i] != tail && isLesser(curNode->next_nodes[i]->key, key))
            curNode = curNode->next_nodes[i];
        if (curNode->next_nodes[i] != tail && isEqual(curNode->next_nodes[i]->key, key))
        {
	        tmp = curNode->next_nodes[i];
            modSeqType(KTYPE_DELETE,tmp->seqNumber);
	        tmp->key = mem_.put(key);
	        tmp->seqNumber = mem_.putSeq(getSeqNum(KTYPE_INSERT));
            tmp->data = mem_.put(value);
            return true;
        }
        update[i] = curNode;
    }
    len_ += sizeof(Key) + key->size_ + sizeof(uint64) + sizeof(Value) + value->size_;
    activeSize = mem_.activeSize();

    // construct skiplistnode
    SkipListNode* node = new SkipListNode;
    node->key = mem_.put(key);
    node->seqNumber = mem_.putSeq(getSeqNum(KTYPE_INSERT));
    node->data = mem_.put(value);
    node->level = RandomLevel();
    node->isMeta = false;
	//set meta data index
	if (len_ > META_BLOCK_SIZE)
	{
		cout<<"meta node, seqNum:"<<*node->seqNumber<<endl;
	    node->isMeta = true;
	    node->offset = activeSize;
	    len_ = 0;
    }
    node->next_nodes = new SkipListNode* [node->level + 1];

    for (uint i=0; i<= node->level; i++)
    {
        node->next_nodes[i] = update[i]->next_nodes[i];
        update[i]->next_nodes[i] = node;
    }

    return true;
}

Value* SkipList::getData(Key* key)
{
    SkipListNode* curNode = head;

    for (int i = MAX_LEVEL - 1; i >= 0; i--)
    {
        while(curNode->next_nodes[i] != tail 
	        && isLesser(curNode->next_nodes[i]->key, key))
            curNode = curNode->next_nodes[i];
        if (curNode->next_nodes[i] != tail 
	        && isEqual(curNode->next_nodes[i]->key, key)
	        && KTYPE_INSERT == getSeqType(*(curNode->next_nodes[i]->seqNumber)))
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
        while(curNode->next_nodes[i] != tail && isLesser(curNode->next_nodes[i]->key, key))
            curNode = curNode->next_nodes[i];
        if (curNode->next_nodes[i] != tail && isEqual(curNode->next_nodes[i]->key, key))
        {
            update[i] = curNode;
        }
        else
        {
            update[i] = NULL;
        }
    }
    //no need to delete node in 0 level
    SkipListNode* tmp = update[0]->next_nodes[0];
    if (NULL != tmp)
    {
	    modSeqType(KTYPE_DELETE,tmp->seqNumber);
        //delete tmp->next_nodes;
        //delete tmp;
    }
    
    for (uint i = 1; i < MAX_LEVEL; i++)
    {
        if (NULL != update[i])
        {
            tmp = update[i]->next_nodes[i];
            update[i]->next_nodes[i] = tmp->next_nodes[i];
        }
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
    uint32 metaLen = 0;
    uint32 metaIdx = 0;
    char endFlag[sizeof(uint32)];
    char* ptr;
    char* metaPtr;
    ofstream fout;

    *(uint32*)endFlag = 0xffffffff;
    fout.open(fileName, ios::binary);

    SkipListNode* curNode = head->next_nodes[0];
    while(tail != curNode)
    {
        if (curNode->isMeta)
        {
	        //meta data index
            metaLen += sizeof(uint32) + sizeof(Key) + curNode->key->size_;
        }
        len = sizeof(uint32) + curNode->key->size_
	        + sizeof(uint64) + sizeof(uint32) + curNode->data->size_;
        ptr = (char*)curNode->key;
        fout.write(ptr,len);
        curNode = curNode->next_nodes[0];
    }
    fout.write(endFlag,sizeof(uint32));
    
    //index of meta data index
    metaIdx = mem_.activeSize() + sizeof(uint32);//endFlag length
    metaLen += sizeof(uint32) + sizeof(uint32);
    
    metaPtr = new(nothrow) char[metaLen];
    ptr = metaPtr;
    curNode = head->next_nodes[0];
    while(tail != curNode)
    {
	    if (curNode->isMeta)
	    {
	        *(uint32*)ptr = curNode->offset;
	        ptr += sizeof(uint32);
	        memcpy(ptr, (char*)curNode->key, sizeof(Key) + curNode->key->size_);
	        ptr += sizeof(Key) + curNode->key->size_;
        }
        curNode = curNode->next_nodes[0];
    }
    *(uint32*)ptr = *(uint32*)endFlag;
    ptr += sizeof(uint32);
    *(uint32*)ptr = metaIdx;
    fout.write(metaPtr,metaLen);
    fout.close();
    return true;
}


bool SkipList::readFile(char* fileName)
{
    if (NULL == fileName)
        return false;
    uint32 metaOffset = 0;
    uint32 offset = 0;
    uint32 num = 0;
    uint32 len = 0;
    char ptr[sizeof(uint32)];
    char seqNum[sizeof(uint64)];
    char outStr[20] = {0};
    ifstream fin;
    fin.open(fileName, ios::binary | ios::ate);

    fin.seekg(-4,ios::end);
    fin.read(ptr,sizeof(uint32));
    metaOffset = *(uint32*)ptr;
    cout<<"meta info index offset:"<<metaOffset<<endl;
    fin.seekg(metaOffset, ios::beg);
    cout<<"meta info index:"<<endl;
    while (!fin.eof())
    {
        fin.read(ptr, sizeof(uint32));
        offset = *(uint32*)ptr;
        if (offset == 0xffffffff)
            break;
        
        cout<<"key offset:"<<offset<<endl;
        fin.read(ptr, sizeof(uint32));
        len = *(uint32*)ptr;
        cout<<"key size:"<<len<<endl;

        fin.read(outStr, len);
        outStr[len] = '\0';
        cout<<"key data:"<<outStr<<endl;
        
    }
    fin.seekg(0, ios::beg);
    cout<<"data info:"<<endl;
    while (!fin.eof())
    {
        fin.read(ptr, sizeof(uint32));
        len = *(uint32*)ptr;
        if (len == 0xffffffff)
            break;
        cout<<"key size:"<<len<<endl;
        
        fin.read(outStr, len);
        outStr[len] = '\0';
        cout<<"key data:"<<outStr<<endl;

		fin.read(seqNum, sizeof(uint64));
		cout<<"seq Num:"<<*(uint64*)seqNum<<endl;

        fin.read(ptr, sizeof(uint32));
        len = *(uint32*)ptr;
        cout<<"data size:"<<len<<endl;
        
        fin.read(outStr, len);
        outStr[len] = '\0';
        cout<<"data data:"<<outStr<<endl;

        num++;
    }
    cout<<"num:"<<num<<endl;
    fin.close();
    
    return true;
}

