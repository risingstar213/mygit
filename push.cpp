#include "definitions.h"
//transport objects between repositories
void transport(Repository& local, Repository& remote, string sha1)
{
	string path = string(local.gitdir) + string("\\objects\\") + sha1.substr(0, 2) + string("\\");
	path += sha1.substr(2);
	char data[10000];
	FILE* fp = fopen(path.c_str(), "rb");
	int n = fread(data, 1, 10000, fp); fclose(fp);
	path = string(remote.gitdir) + string("\\objects\\") + sha1.substr(0, 2);
	if (CheckFolderExist(path.c_str()) == 0) {
		CreateDirectory(path.c_str(), NULL);
	}
	path += string("\\") + sha1.substr(2);
	fp = fopen(path.c_str(), "wb");
	fwrite(data, 1, n, fp); fclose(fp);
	return;
}
//push "locally"
int push1(string name, string branch)
{
	Repository* local = repo_find();
	string path = string(local->gitdir) + string("\\refs\\remotes\\") + name;
	path += string("\\name");
	FILE* fp = fopen(path.c_str(), "r");
	char temp[100] = { 0 };
	fscanf(fp, "%s", temp); fclose(fp);
	Repository* remote = repo_find(temp);
	string local_sha1 = string(get_local_hash(*local, branch));
	set<string> pushing = find_commit_object(*local, local_sha1);
	for (auto& i : pushing) {
		transport(*local, *remote, i);
	}
	path = string(local->gitdir) + string("\\refs\\remotes\\");
	path += name + string("\\") + branch;
	fp = fopen(path.c_str(), "w");
	fprintf(fp, "%s", local_sha1.c_str()); fclose(fp);
	char* tempp = get_local_hash(*remote, branch);
	string remot_sha1;
	if (tempp == NULL) {
		remot_sha1 = string("\0");
	}
	else remot_sha1 = string(tempp);
	string mergesha1 = merge(*remote, remot_sha1, local_sha1);
	path = string(remote->gitdir) + string("\\refs\\heads\\") + branch;
	fp = fopen(path.c_str(), "w");
	fprintf(fp, "%s", mergesha1.c_str()); fclose(fp);
	printf("Push successfully");
	return 1;
}
//add remote respository
int remote(string name, string path)
{
	Repository* local = repo_find();
	Repository* remote = repo_find(path.c_str());
	string full_name = string(local->gitdir) + string("\\refs\\remotes\\") + name;
	CreateDirectory(full_name.c_str(), NULL);
	full_name += string("\\name");
	FILE* fp = fopen(full_name.c_str(), "w");
	fprintf(fp, "%s", remote->workplace);
	fclose(fp);
	printf("Add remote repository successfully");
	return 1;
}
void _remote(int& retc, char retv[][100], int argc, char* argv[], Ans ans)
{
	if (ans.optNum == 0) {
		printf("Option error");
		exit(0);
	}
	remote(string(argv[ans.fstPara[0]]), string(argv[ans.lstPara[0]]));
}
void _push(int& retc, char retv[][100], int argc, char* argv[], Ans ans)
{
	if (ans.optNum == 0) {
		printf("Option error");
		exit(0);
	}
	push1(string(argv[ans.fstPara[0]]), string(argv[ans.lstPara[0]]));
}