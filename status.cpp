#include "definitions.h"
//get the status for directory
int status_dir(Repository& repo, char* name, char * sha1)
{
	int n;
	char * ans = hash_folder(repo, name, n, 0);
	Object obj(&repo, "blob", ans);
	char* temp = object_write(&obj, 0);
	if (strcmp(temp, sha1) == 0) {
		return 1;
	}
	else return 0;
}
//get status
int status()
{
	set<string> seen;
	Repository* repo = repo_find();
	int len1 = strlen(repo->gitdir) + strlen("\\refs\\heads\\");
	char* mm = get_local_master(*repo);
	if (mm == NULL) {
		printf("On no branch\n\n");
	}
	else printf("On branch %s\n\n", mm + len1);
	int n;
	char* temp;
	char* data = new char[10000];
	IndexEntry *entries = read_index(*repo, n);
	for (int i = 0; i < n; i++) {
		seen.insert(string(entries[i].path));
		if (CheckFolderExist(entries[i].path) == 0){
			FILE* fp = fopen(entries[i].path, "r");
			int num = fread(data, 1, 10000, fp);
			data[num] = '$'; fclose(fp);
			Object obj(repo, "blob", data);
			temp = object_write(&obj, 0);
		
			if (strcmp(temp, entries[i].sha1) == 0) {
				//printf("%s %s\n", temp, entries[i].sha1);
				printf("A  %s\n", entries[i].path);
			}
			else {
				//printf("%s %s\n", temp, entries[i].sha1);
				printf("AZ %s\n", entries[i].path);
			}
		}
		else {
			if (status_dir(*repo,entries[i].path, entries[i].sha1)) {
				printf("A  %s\n", entries[i].path);
			}
			else printf("AZ %s\n", entries[i].path);
		}
	}
	char _name[100];
	char __name[100];
	strcat(strcpy(_name, repo->workplace), "\\*.*");
	long handle; int ans = 0;
	struct _finddata_t ffblk1, ffblk2;
	if ((handle = _findfirst(_name, &ffblk1)) == -1) {
		return 0;
	}
	while (ans >= 0) {
		ffblk2 = ffblk1;
		ans = _findnext(handle, &ffblk1);
		if (!strcmp(ffblk2.name, ".") || !strcmp(ffblk2.name, "..") || !strcmp(ffblk2.name, ".git"))continue;
		//printf("write_tree1 %d\n", i++);
		strcat(strcat(strcpy(__name, repo->workplace), "\\"), ffblk2.name);
		if (seen.find(string(__name)) == seen.end() && seen.find(string(ffblk2.name)) == seen.end()) {
			printf("?? %s\n", ffblk2.name);
		}
	}
	_findclose(handle);
	return 1;
}
void _status(int& retc, char retv[][100], int argc, char* argv[], Ans ans)
{
	status();
	retc = 0;
}
