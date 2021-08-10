#include "definitions.h"
//fetch or pull from "remote repository"
int fetch_pull(string name, string branch,int ind)
{
	Repository* local = repo_find();
	string path = string(local->gitdir) + string("\\refs\\remotes\\") + name;
	path += string("\\name");
	FILE* fp = fopen(path.c_str(), "r");
	char temp[100] = { 0 };
	fscanf(fp, "%s", temp); fclose(fp);
	Repository* remote = repo_find(temp);
	string remot_sha1 = string(get_local_hash(*remote, branch));
	set<string> pulling = find_commit_object(*remote, remot_sha1);
	for (auto& i : pulling) {
		transport(*remote, *local, i);
	}
	path = string(local->gitdir) + string("\\refs\\remotes\\");
	path += name + string("\\") + branch;
	fp = fopen(path.c_str(), "w");
	fprintf(fp, "%s", remot_sha1.c_str()); fclose(fp);
	if (ind) {
		string local_sha1;
		char* tempp = get_local_hash(*remote, branch);
		if (tempp == NULL) {
			local_sha1 = string("\0");
		}
		else local_sha1 = string(tempp);
		string mergesha1 = merge(*local, local_sha1, remot_sha1);
		path = string(local->gitdir) + string("\\refs\\heads\\") + branch;
		fp = fopen(path.c_str(), "w");
		fprintf(fp, "%s", mergesha1.c_str()); fclose(fp);
		printf("Pull successfully");
		return 1;
	}
	printf("Fetch successfully");
	return 1;
}
void _fetch(int& retc, char retv[][100], int argc, char* argv[], Ans ans)
{
	if (ans.optNum == 0) {
		printf("Option error");
		exit(0);
	}
	fetch_pull(string(argv[ans.fstPara[0]]), string(argv[ans.lstPara[0]]), 0);
}
void _pull(int& retc, char retv[][100], int argc, char* argv[], Ans ans)
{
	if (ans.optNum == 0) {
		printf("Option error");
		exit(0);
	}
	fetch_pull(string(argv[ans.fstPara[0]]), string(argv[ans.lstPara[0]]), 1);
}