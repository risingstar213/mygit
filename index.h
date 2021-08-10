#pragma once
struct IndexEntry {
	unsigned long long c_time;
	unsigned long long m_time;
	unsigned long dev;
	unsigned long ino;
	unsigned long mode;
	unsigned long uid;
	unsigned long gid;
	unsigned long size;
	char sha1[45];
	char path[100];
};
IndexEntry* read_index(Repository &, int& );
int write_index(Repository&, IndexEntry*, int n);
int write_index(Repository&, char*);
std::string LastTime(std::string fileName);
char* hash_folder(Repository&, char*, int&, int);