#pragma once
class Repository {
public:
	char workplace[100] = "\0";
	char gitdir[100] = "\0";
	Config conf;
	Repository(const char* path, int force);
};
Repository *repo_create(const char* path);
Repository  * repo_find(const char* = ".", int = true);
char* repo_file(Repository&, char [][100], int, int );
BOOL CheckFolderExist(const char* strPath);
class Object {
public:
	Repository * repo;
	char type[20];
	const char* data;
	Object(Repository * ,const char * ,const char* );
	const char * serialize();
	void deserialize(const char* data);
};
Object* object_read(Repository&, char sha[100], int num);
char* object_write(Object*, int);
std::string sha1(const std::string);