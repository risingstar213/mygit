#include "definitions.h"
Config::Config()
{
	eleNum = 0; cateNum = 0;
	memset(fstEle, -1, sizeof(int));
	memset(nxtEle, -1, sizeof(int));
	memset(cateName, 0, sizeof(char)); memset(eleName, 0, sizeof(char));
	memset(strVal, 0, sizeof(char));
}
//read configurations from config
void Config::readConfig(char* name)
{
	char temp[200];
	FILE* fp = fopen(name, "r");
	cateNum = eleNum = 0;
	while (feof(fp) == 0) {
		fgets(temp, 200, fp);
		//fputs(temp, stdout);
		if (temp[0] == '[') {
			int j;
			for (j = 1; temp[j] != ']'; j++) {
				cateName[cateNum][j - 1] = temp[j];
			}
			cateName[cateNum][j - 1] = '\0';
			fstEle[cateNum] = -1;
			cateNum++;
		}
		else {
			int j = 0;
			while (temp[j] == ' ' || temp[j] == '\t') j++;
			int k = 0;
			for (; temp[j] != '='; j++, k++) {
				eleName[eleNum][k] = temp[j];
			}
			eleName[eleNum][k] = '\0';
			if (findEle(cateNum - 1, eleName[eleNum]) >= 0) {
				continue;
			}
			nxtEle[eleNum] = fstEle[cateNum - 1];
			//printf("%d %d\n",eleNum, nxtEle[eleNum]);
			fstEle[cateNum - 1] = eleNum;
			j++;
			int flag = 0;
			int jj = j;
			while (temp[jj] != ' ' && temp[jj] != '\n' && temp[jj] != '\0') {
				if (temp[jj] < '0' || temp[jj] > '9') {
					flag = 1;
					break;
				}
				jj++;
			}
			if (flag == 0) {
				valType[eleNum] = ++numNum;
				numVal[numNum] = 0;
				while (temp[j] >= '0' && temp[j] <= '9') {
					numVal[numNum] = numVal[numNum] * 10 + temp[j++] - '0';
				}
			}
			else {
				valType[eleNum] = -(++strNum);
				k = 0;
				for (; temp[j] != ' ' && temp[j] != '\n' && temp[j] != '\0'; j++, k++) {
					strVal[strNum][k] = temp[j];
				}
				strVal[strNum][k] = '\0';
			}
			eleNum++;
		}
		//fputs(temp, stdout);
	}
	//printf("read successfullly");
	fclose(fp);
}
int Config::findCate(const char* cat)
{
	for (int i = 0; i < cateNum; i++) {
		if (!strcmp(cat, cateName[i])) return i;
	}
	return -1;
}
int Config::addCate(const char* cat)
{
	int ind = findCate(cat);
	if (ind >= 0) return -1;
	strcpy(cateName[cateNum], cat);
	fstEle[cateNum] = -1;
	cateNum++;
	return 1;
}
int Config::findEle(int ind, const char* ele)
{
	int now = fstEle[ind];
	while (now != -1) {
		//printf("%s\n", eleName[now]);
		if (!strcmp(ele, eleName[now])) return now;
		now = nxtEle[now];
	}
	return -1;
}
int Config::addEle(const char* cat, const char* ele,const char* val) {
	int ind = findCate(cat);
	if (ind < 0) return -1;
	int _ind = findEle(ind, ele);
	if (_ind >= 0) return -1;
	strcpy(eleName[eleNum], ele);
	nxtEle[eleNum] = fstEle[ind];
	fstEle[ind] = eleNum;
	int flag = 0;
	for (int i = 0; val[i] != '\0'; i++) {
		if (val[0] < '0' && val[0] > '9') {
			flag = -1;
			break;
		}
	}
	if (flag == 0) {
		valType[eleNum] = ++numNum;
		numVal[numNum] = atoi(val);
	}
	else {
		valType[eleNum] = -(++strNum);
		strcpy(strVal[strNum], val);
	}
	eleNum++;
	return 1;
}
char _conf[100];
char* Config::get(const char* cat, const char* ele)
{
	//printf("1\n");
	int ind = findCate(cat);
	//printf("2\n");
	if (ind < 0) return NULL;
	int _ind = findEle(ind, ele);
	//printf("3\n");
	if (_ind < 0) return NULL;
	if (valType[_ind] < 0) return strVal[-valType[_ind]];
	else return _itoa(numVal[valType[_ind]], _conf, 10);
}
void Config::default_config()
{
	addCate("core");
	//printf("[1]");
	addEle("core", "repositoryformatversion", "0");
	//printf("[1]");
	addEle("core", "filemode", "false");
	//printf("[1]");
	addEle("core", "bare", "false");
	//printf("[1]");
}
void Config::writeConfig(FILE* fp)
{
	char __tt[100];
	for (int i = 0; i < cateNum; i++) {
		fprintf(fp, "[%s]\n", cateName[i]);
		//printf("%d \n", i);
		int now = fstEle[i];
		while (now != -1) {
			//printf("%d %d\n", i, now);
			if (valType[now] < 0) {
				fprintf(fp, "\t%s=%s\n", eleName[now], strVal[-valType[now]]);
			}
			else fprintf(fp, "\t%s=%s\n", eleName[now], _itoa(numVal[valType[now]], __tt, 10));
			now = nxtEle[now];
		}
	}
}
void Config::showConfig()
{
	char __tt[100];
	//printf("num:%d\n", cateNum);
	for (int i = 0; i < cateNum; i++) {
		printf("[%s]\n", cateName[i]);
		//printf("%d \n", i);
		int now = fstEle[i];
		while (now != -1) {
			//printf("%d %d\n", i, now);
			if (valType[now] < 0) {
				printf("\t%s=%s\n", eleName[now], strVal[-valType[now]]);
			}
			else printf("\t%s=%s\n", eleName[now], _itoa(numVal[valType[now]], __tt, 10));
			now = nxtEle[now];
		}
	}
}
void _config(int& retc, char retv[][100], int argc, char* argv[], Ans ans)
{
	if (ans.optNum == 0) {
		printf("Option error");
		exit(0);
	}
	Repository* repo = repo_find();
	char name[100];
	char name1[100];
	strcat(strcpy(name, repo->gitdir), "\\config");
	repo->conf.readConfig(name);
	for (int i = 0; i < ans.optNum; i++) {
		if (ans.optInd[i] == 1) {
			repo->conf.showConfig();
		}
		else if (ans.optInd[i] == 2) {
			int j = 0;
			while (argv[ans.fstPara[i]][j] != '.') {
				name[j] = argv[ans.fstPara[i]][j];
				j++;
			}
			name[j++] = '\0';
			repo->conf.addCate(name);
			strcpy(name1, argv[ans.fstPara[i]] + j);
			repo->conf.addEle(name, name1, argv[ans.lstPara[i]]);
			strcat(strcpy(name, repo->gitdir), "\\config");
			FILE* fp = fopen(name, "w");
			repo->conf.writeConfig(fp);
			fclose(fp);
		}
	}
	retc = 0;
}