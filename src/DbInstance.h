#ifndef LITEDB_DBINSTANCE_H
#define LITEDB_DBINSTANCE_H
#endif

#include "dbtype.h"
#include "SkipList.h"

namespace litedb {
#define MAX_DB_FILE_NAME_LEN (128)

class DbInstance {
private:
	SkipList list_;
	uint32 fileSeq_;
	char dbName_[MAX_DB_FILE_NAME_LEN];
public:
	DbInstance(char* dbname);
	~DbInstance();
	bool create(char* dbname);
	void rename(char* dbname);
	bool insert(uint32 kLen, char* kData, uint32 vLen, char* vData);
	bool delete(uint32 kLen, char* kData);
	bool get(uint32 kLen, char* kData, uint32* vLen, char** vData);
};

}
