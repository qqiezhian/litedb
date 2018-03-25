#include <iostream>
#include <fstream>
#include "SkipList.h"

using namespace std;
using namespace litedb;

DbInstance::DbInstance(char* dbname):list_(SKIP_MAX_LEVEL), fileSeq(0)
{
    if (NULLPTR == dbname || strlen(dbname) > MAX_DB_FILE_NAME_LEN-1)
        return;

    strncpy(dbName_, dbname, strlen(dbname));
    dbName_[strlen(dbname)] = '\0';
}

DbInstance::~DbInstance()
{
    
}

bool DbInstance::create(char* dbname)
{

}

void DbInstance::create(char* dbname)
{

}
char* DbInstance::getFName(char* dbname, uint32 seq)
{
    char* fName;
    uint32 len = strlen(dbname);
	//12 char for safe 
    len += sizeof('_') + sizeof('.dat') + 12;

    fName = new(nothrow) char[len + 1];
    
    sprintf(fName, "%s_%d.dat", daname, seq);
    return fName;
}

bool DbInstance::insert(uint32 kLen, char* kData, uint32 vLen, char* vData)
{
    if (0 == kLen || NULLPTR == kData || 0 == vLen || NULLPTR == vData)
        return false;
    Key k = constructSlice(kLen, kData);
    Value v = constructSlice(vLen, vData);

    if (!list_.canInsert(k,v))
    {
        char* fName = getFName(dbName_,fileSeq_);
        list_.write2File(fName);
        list_.clean();
        fileSeq_++;
    }
    list_.insertNode(k,v);
    delete k;
    delete v;
    return true;
}

bool DbInstance::delete(uint32 kLen,char* kData)
{
    if (0 == kLen || NULLPTR == kData)
        return false;
    Key k = constructSlice(kLen, kData);

    list_.deleteNode(k);
    delete k;

    return true;
}

bool DbInstance::get(uint32 kLen,char* kData,uint32* vLen,char** vData)
{
    if (0 == kLen || NULLPTR == kData || NULLPTR == vLen || NULLPTR == *vData)
        return false;
    Value* v;
    Key* k = constructSlice(kLen, kData);
    
    v = list_.getData(Key * key);
    if (NULLPTR == *vData)
        return false;
    
    *vLen = v->size_;
    *vData = new(nothrow) char[*vLen];
    if (NULLPTR == *vData)
        return false;
        
    memcpy(*vData, v->data_, *vLen);

    delete v;
    delete k;
    
    return true;
}

