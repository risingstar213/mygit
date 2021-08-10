#pragma once
#define MAXCATENUM 128
#define MAXNAMELEN 128
#define MAXELENUM 128
class Config {
public:

	int cateNum = 0; int eleNum = 0;
	char cateName[MAXCATENUM][MAXNAMELEN];
	int fstEle[MAXCATENUM];
	int nxtEle[MAXELENUM];//¡¥ ΩœÚ«∞–«¥Ê¥¢
	char eleName[MAXELENUM][MAXNAMELEN];
	int valType[MAXELENUM];
	int numVal[MAXELENUM]; int numNum = 0;
	char strVal[MAXELENUM][MAXNAMELEN]; int strNum = 0;
	Config();
	void readConfig(char*);
	int findCate(const char*);
	int addCate(const char*);
	int findEle(int,const char*);
	int addEle(const char*,const char*,const char*);
	char* get(const char*, const char*);
	void default_config();
	void writeConfig(FILE*);
	void showConfig();
};
