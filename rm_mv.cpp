#include "definitions.h"
//remove files
int remove_file(char* name,int del = true)
{
	FILE* fp = fopen(name, "r");
	if (fp == NULL) {
		printf("No such file or directory");
		exit(0);
	}
	fclose(fp);
	Repository* repo = repo_find();
	int n;
	IndexEntry* entries = read_index(*repo, n);
	for (int i = 0; i < n; i++) {
		if (strcmp(name, entries[i].path) == 0) {
			for (int j = i + 1; j < n; j++) {
				entries[j - 1] = entries[j];
			}
			write_index(*repo, entries, n - 2);
			if (del) {
				if (remove(name) == EOF) {
					printf("Cannot remove the file:%s", name);
				}
			}
			return 1;
		}
	}
	printf("There is no such file in the index");
	return 0;
}
//remove directorys
int remove_dir1(char* name)
{
	char _name[100];
	char __name[100];
	strcat(strcpy(_name, name), "\\*.*");
	long handle; int ans = 0;
	struct _finddata_t ffblk1, ffblk2;
	if ((handle = _findfirst(_name, &ffblk1)) == -1) {
		return 1;
	}
	while (ans >= 0) {
		ffblk2 = ffblk1;
		ans = _findnext(handle, &ffblk1);
		if (!strcmp(ffblk2.name, ".") || !strcmp(ffblk2.name, ".."))continue;
		strcat(strcat(strcpy(__name, name), "\\"), ffblk2.name);
		if (CheckFolderExist(__name)) {
			remove_dir1(__name);
			RemoveDirectory(__name);
		}
		else remove(__name);
	}
	_findclose(handle);
	return 1;
}
int remove_dir(char* name, int del = true)
{
	Repository* repo = repo_find();
	int n;
	IndexEntry* entries = read_index(*repo, n);
	for (int i = 0; i < n; i++) {
		if (strcmp(name, entries[i].path) == 0) {
			for (int j = i + 1; j < n; j++) {
				entries[j - 1] = entries[j];
			}
			write_index(*repo, entries, n - 2);
			if (del) {
				remove_dir1(name);
				RemoveDirectory(name);
			}
			return 1;
		}
	}
	printf("There is no such directory in the index");
	return 0;
}
void _rm(int& retc, char retv[][100], int argc, char*argv [], Ans ans)
{
	if (ans.optNum == 0) {
		printf("Option error");
		return;
	}
	int ind = ans.optInd[0] == 0 ? 1 : 0;
	if (CheckFolderExist(argv[ans.fstPara[0]])) {
		remove_dir(argv[ans.fstPara[0]], ind);
	}
	else {
		remove_file(argv[ans.fstPara[0]], ind);
	}
	retc = 0;
}
//move or rename
int move(char* ol, char* ne, int force = false)
{

	Repository* repo = repo_find();
	int n;
	int flag = -1;
	IndexEntry* entries = read_index(*repo, n);
	for (int i = 0; i < n; i++) {
		if (strcmp(ol, entries[i].path) == 0) {
			flag = i;
		}
	}
	if (flag == -1) {
		printf("There is no such file or directory in the index");
		exit(0);
	}
	if (!force && _access(ne, 0) == 0) {
		printf("The file has existed");
		exit(0);
	}
	FILE* fp = fopen(ol, "r");
	if (fp == NULL) {
		printf("The file has been removed");
		exit(0);
	}
	char temp[10000]; int num = fread(temp, 1, 10000, fp); fclose(fp);
	//printf("??");
	fp = fopen(ne, "w");
	fwrite(temp, 1, num, fp); fclose(fp);
	//printf("??");
	strcpy(entries[flag].path, ne);
	struct _stat t;
	_stat(ne, &t);
	//printf("??");
	entries[flag].m_time = t.st_mtime;
	entries[flag].mode = t.st_mode;
	remove(ol);
	write_index(*repo, entries, n - 1);
	return 1;
}
void _mv(int& retc, char retv[][100], int argc, char* argv[], Ans ans)
{
	if (ans.optNum == 0) {
		printf("Option error");
		exit(0);
	}
	if (CheckFolderExist(argv[ans.fstPara[0]])) {
		printf("A airectory cannot be moved");
		exit(0);
	}
	move(argv[ans.fstPara[0]], argv[ans.lstPara[0]], ans.optInd[0]);
	printf("move sucessfully");
	retc = 0;
}
