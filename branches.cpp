#include "definitions.h"
//get the hash for HEAD
char* get_local_master_hash(Repository& repo)
{
	char path[100];
	strcat(strcpy(path, repo.gitdir), "\\HEAD");
	FILE* fp = fopen(path, "r");
	char* data = new char[100];
	fscanf(fp, "%s", data);
	//printf("hash:%s\n", data);
	if (data[0] != 'r') {
		//printf("???");
		fclose(fp);
		return data;
	}
	//fgetc(fp);
	fscanf(fp, "%s", data); fclose(fp);
	//printf("hash:%s\n", data);
	strcat(strcpy(path, repo.gitdir), "\\");
	strcat(path, data);
	if ((fp = fopen(path, "r") )== NULL) {
		delete [] data;
		return NULL;
	}
	fscanf(fp, "%s", data);
	//printf("hash:%s\n", data);
	fclose(fp);
	return data;
}
//get the branch for HEAD
char* get_local_master(Repository& repo)
{
	char path[100];
	strcat(strcpy(path, repo.gitdir), "\\HEAD");
	FILE* fp = fopen(path, "r");
	char* data = new char[100];
	fscanf(fp, "%s", data);
	if (data[0] != 'r') {
		fclose(fp);
		return NULL;
	}
	fscanf(fp, "%s", path); fclose(fp);
	const char *temp = "refs/tags/";
	for (int i = 0; i < 10; i++) {
		if (path[i] != temp[i])break;
		if (i == 9)return NULL;
	}
	strcat(strcpy(data, repo.gitdir), "\\");
	strcat(data, path);
	return data;
}
//get the hash for branch
char* get_local_hash(Repository& repo, string branch)
{
	char path[100];
	char* data = new char[100];
	strcpy(data, "\\refs\\heads\\");
	strcat(data, branch.c_str());
	strcat(strcpy(path, repo.gitdir), "\\");
	strcat(path, data);
	FILE* fp;
	if ((fp = fopen(path, "r")) == NULL) {
		delete[] data;
		return NULL;
	}
	fscanf(fp, "%s", data);
	//printf("hash:%s\n", data);
	fclose(fp);
	return data;
}//add branch
int branch(char* name)
{
	Repository* repo = repo_find();
	char full_name[100];
	strcat(strcpy(full_name, repo->gitdir),"\\refs\\heads\\");
	strcat(full_name, name);
	if (_access(full_name, 0) == 0) {
		printf("This branch has existed!");
		exit(0);
	}
	char* hash = get_local_master_hash(*repo);
	FILE* fp = fopen(full_name, "w");
	fwrite(hash, 1, 40, fp);
	fclose(fp);
	printf("Create branch %s to %s", name, hash);
	return 1;
}
void _branch(int& retc, char retv[][100], int argc, char* argv[], Ans ans)
{
	if (ans.optNum == 0) {
		printf("Option error");
		exit(0);
	}
	else branch(argv[ans.fstPara[0]]);
}
void SetColorAndBackground(int ForgC, int BackC) {
	WORD wColor = ((BackC & 0x0F) << 4) + (ForgC & 0x0F);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wColor);
}
//list the existing refs 
int list_refs(Repository& repo)
{
	string full_name = string(repo.gitdir) + string("\\refs\\heads\\*.*");
	long handle; int ans = 0;
	struct _finddata_t ffblk1, ffblk2;
	if ((handle = _findfirst(full_name.c_str(), &ffblk1)) == -1) {
		printf("The format is not correct");
		exit(0);
	}
	printf("master\n");
	while (ans >= 0) {
		ffblk2 = ffblk1;
		ans = _findnext(handle, &ffblk1);
		if (!strcmp(ffblk2.name, ".") || !strcmp(ffblk2.name, "..") || !strcmp(ffblk2.name, "master"))continue;
		printf("%s\n", ffblk2.name);
	}
	_findclose(handle);
	full_name = string(repo.gitdir) + string("\\refs\\remotes\\*.*");
	if ((handle = _findfirst(full_name.c_str(), &ffblk1)) == -1) {
		exit(0);
	}
	ans = 0;
	SetColorAndBackground(4, 0);
	while (ans >= 0) {
		ffblk2 = ffblk1;
		ans = _findnext(handle, &ffblk1);
		if (!strcmp(ffblk2.name, ".") || !strcmp(ffblk2.name, ".."))continue;
		long handle1; int ans1 = 0;
		struct _finddata_t ffblk3, ffblk4;
		string full_name1 = string(repo.gitdir) + string("\\refs\\remotes\\") + string(ffblk2.name) + string("\\*.*");
		if ((handle1 = _findfirst(full_name1.c_str(), &ffblk3)) == -1) {
			SetColorAndBackground(15, 0);
			_findclose(handle);
			exit(0);
		}
		while (ans1 >= 0) {
			ffblk4 = ffblk3;
			ans1 = _findnext(handle1, &ffblk3);
			if (!strcmp(ffblk4.name, ".") || !strcmp(ffblk4.name, "..") ||!strcmp(ffblk4.name, "name"))continue;
			printf("remotes/%s/%s\n", ffblk2.name, ffblk4.name);
		}
		_findclose(handle1);
	}
	SetColorAndBackground(15, 0);
	return 1;
}
void _list_refs(int& retc, char retv[][100], int argc, char* argv [], Ans ans)
{
	if (ans.optNum != 0) {
		printf("Option error");
		exit(0);
	}
	Repository* repo = repo_find();
	list_refs(*repo);
}
