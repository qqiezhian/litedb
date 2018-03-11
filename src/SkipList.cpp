#include <iostream>
#include "SkipList.h"

using namespace std;
using namespace litedb;

SkipList::SkipList(uint maxLevel):mem_(MAX_MEM_SIZE)
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

void SkipList::insertNode(Key& key, Value& value)
{
    SkipListNode* update[MAX_LEVEL];
    SkipListNode* curNode = head;

    for (int i = MAX_LEVEL - 1; i >= 0; i--)
    {
        while(curNode->next_nodes[i] != tail && curNode->next_nodes[i]->key < key)
            curNode = curNode->next_nodes[i];
        if (curNode->next_nodes[i] != tail && curNode->next_nodes[i]->key == key)
        {
            curNode->next_nodes[i]->data = mem_.put(value);
            return;
        }
        update[i] = curNode;
    }

    // construct skiplistnode
    SkipListNode* node = new SkipListNode;
    node->key = mem_.put(key);
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

Value SkipList::getData(Key& key)
{
    SkipListNode* curNode = head;

    for (int i = MAX_LEVEL - 1; i >= 0; i--)
    {
        while(curNode->next_nodes[i] != tail && curNode->next_nodes[i]->key < key)
            curNode = curNode->next_nodes[i];
        if (curNode->next_nodes[i] != tail && curNode->next_nodes[i]->key == key)
        {
            return curNode->next_nodes[i]->data;
        }
    }

    return Value();
}


void SkipList::deleteNode(Key & key)
{
	SkipListNode* update[MAX_LEVEL];
    SkipListNode* curNode = head;

    for (int i = MAX_LEVEL - 1; i >= 0; i--)
    {
        while(curNode->next_nodes[i] != tail && curNode->next_nodes[i]->key < key)
            curNode = curNode->next_nodes[i];
        if (curNode->next_nodes[i] != tail && curNode->next_nodes[i]->key == key)
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
        delete tmp->next_nodes;
        delete tmp;
    }
    return;
}

void SkipList::displayList()
{
	uint num = 0;
    SkipListNode* curNode = head->next_nodes[0];
    while(tail != curNode)
    {
		num++;
        curNode->key.print();
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