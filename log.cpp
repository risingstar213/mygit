#include "definitions.h"
char sha_t[50];
//read commit objects
CommitEntry* read_commit(Repository& repo,string sha)
{
	CommitEntry* centries = new CommitEntry;
	centries->rownum = 0; centries->fanum = 0;
	int& row = centries->rownum;
	//string full_name = string(repo.gitdir) + string("\\") + string("objects") + string("\\") + sha.substr(0, 2) + string("\\");
	//full_name += sha.substr(sha.length() - 38);
	//printf("%s\n", full_name.c_str());
	strcpy(sha_t, sha.c_str());
	Object * obj = object_read(repo, sha_t, 1);
	int n = 0;
	while (obj->data[n] != '$') n++;
	string cdata = string(obj->data, obj->data + n);
	//printf("cdata:%s\n", cdata.c_str());
	int start = 0, spc, nl, end;
	while (true) {
		spc = cdata.find(' ', start);
		nl = cdata.find('\n', start);
		//printf("%d %d\n", spc, nl);
		if (spc < 0 || nl < spc) {
			if (nl != start) {
				printf("The commit data is not correct");
				exit(0);
			}
			centries->name[row] = string("message");
			centries->para[row] = cdata.substr(nl + 1, cdata.size() - nl - 1);
			row++;
			break;
		}
		centries->name[row] = cdata.substr(start, spc - start);
		end = start;
		while (1) {
			end = cdata.find('\n', end + 1);
			if (cdata[end + 1] != ' ')break;
		}
		centries->para[row] = cdata.substr(spc + 1, end - spc - 1);
		if (centries->name[row] == string("parent")) {
			centries->fanum++;
		}
		row++;
		start = end + 1;
	}
	/*for (int i = 0; i < row; i++) {
		printf("%d %s %s\n", i, centries->name[i].c_str(), centries->para[i].c_str());
	}*/
	return centries;
}
//dfs through reading commits and print information
void log_graphviz(Repository& repo, string sha, set<string>& seen, int ind)
{
	if (seen.find(sha) != seen.end()) {
		return;
	}
	seen.insert(sha);
	CommitEntry* centries = read_commit(repo, sha);
	if (ind == 0) {
		printf("commit: %s\n", sha.c_str());
		for (int i = 0; i < centries->rownum; i++) {
			if (centries->name[i] == string("author")) {
				printf("Author:  ");
				int n, nn;
				n = centries->para[i].find('>', 0);
				printf("%s\n", centries->para[i].substr(0, n + 1).c_str());
				printf("Date;    ");
				nn = centries->para[i].find(' ', n + 2);
				time_t lt = 0;
				for (int j = n + 2; j < nn; j++) {
					lt = lt * 10 + centries->para[i][j] - '0';
				}
				char* c_time = ctime(&lt);
				int k = 0;
				while (c_time[k] != '\n')putchar(c_time[k++]);
				int len = centries->para[i].length();
				printf("%s\n", centries->para[i].substr(nn, len - nn).c_str());
			}
			if (centries->name[i] == string("message")) {
				printf("\n%s\n", centries->para[i].c_str());
			}
		}
	}
	if (centries->fanum == 0) {
		delete centries;
		return;
	}
	for (int i = 0; i < centries->rownum; i++) {
		if (centries->name[i] == string("parent")) {
			if(ind == 1)printf("c_%s -> c_%s\n", sha.c_str(), centries->para[i].c_str());
			log_graphviz(repo, centries->para[i], seen, ind);
		}
	}
	delete centries;
}
int log(int ind)
{
	Repository* repo = repo_find();
	set<string> seen;
	char* t = get_local_master_hash(*repo);
	if (t == NULL) {
		printf("There is no commit now");
		exit(0);
	}
	if(ind)printf("digraph wyaglog{\n");
	log_graphviz(*repo, string(get_local_master_hash(*repo)), seen, ind);
	if(ind)printf("}\n");
	return 1;
}
void _log(int& retc, char retv[][100], int argc, char* argv[], Ans ans)
{
	if (ans.optNum > 1) {
		printf("Option error");
		exit(0);
	}
	log(ans.optNum);
	retc = 0;
}
