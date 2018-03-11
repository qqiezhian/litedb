#ifndef LITEDB_SLICE_H
#define LITEDB_SLICE_H
#endif

#include "dbtype.h"
#include <iostream>
using namespace std;
namespace litedb {

class Slice {

private:
	size_t size_;
	char* data_;
public:
	Slice() : size_(0), data_((char*)""){}
	Slice(char* d) {size_ = strlen(d); data_ = d; }
	Slice(size_t s, char* d) : size_(s), data_(d){}
	bool empty() {return 0 == size_;}
	size_t size() {return size_;}
	char* data() {return data_;}
	int compare(Slice &s);
	bool operator<(Slice& s) {return (0 > compare(s));}
	bool operator>(Slice& s) {return (0 < compare(s));}
	bool operator==(Slice& s) {return (0 == compare(s));}
	void print();
};

inline int Slice::compare(Slice &s)
{
	if (size_ < s.size())
		return -1;
	else if (size_ > s.size())
		return 1;
	return strncmp(data_,s.data(),size_);
}

inline void Slice::print()
{
	char tmp[10];
	strncpy(tmp, data_, size_);
	tmp[size_] ='\0';
	cout << "key len:" <<size_<<", key:"<<tmp<<endl;
}

typedef Slice Key;
typedef Slice Value;

}
