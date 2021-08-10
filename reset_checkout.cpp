#include "definitions.h"
//Checkout branch for HEAD
int checkout(Repository & repo,string name, int opt)
{
	if (opt == 0) {
		string full_name = string(repo.gitdir) + string("\\refs\\heads\\") + name;
		FILE* fp = fopen(full_name.c_str(), "r");
		if (fp == NULL) {
			printf("The branch %s does not exist.", name.c_str());
			exit(0);
		}
		fclose(fp);
		//printf("checkout?\n");
		string head = string(repo.gitdir) + string("\\HEAD");
		fp = fopen(head.c_str(), "w");
		fprintf(fp, "refs: refs/heads/%s", name.c_str());
		fclose(fp);
		//printf("checkout?\n");
		printf("Change to %s successfully", name.c_str());
	}
	else if (opt == 1) {
		string full_name = string(repo.gitdir) + string("\\refs\\heads\\") + name;
		FILE* fp = fopen(full_name.c_str(), "r");
		if (fp != NULL) {
			fclose(fp);
			printf("The branch %s has existed.", name.c_str());
			exit(0);
		}
		char* temp = get_local_master_hash(repo);
		fp = fopen(full_name.c_str(), "w");
		fprintf(fp, "%s", temp); fclose(fp);
		string head = string(repo.gitdir) + string("\\HEAD");
		fp = fopen(head.c_str(), "w");
		fprintf(fp, "refs: refs/heads/%s", name.c_str());
		fclose(fp);
		printf("Change to %s successfully", name.c_str());
	}
	return 1;
}
//reset to HEAD^
int reset(Repository & repo, int ind)
{
	if (ind == 0) {
		char* temp = get_local_master_hash(repo);
		char* heads = get_local_master(repo);
		if (temp == NULL || heads == NULL) {
			printf("Cannot reset to HEAD^");
			exit(0);
		}
		CommitEntry* centries = read_commit(repo, string(temp));
		if (centries->fanum != 1) {
			delete centries;
			printf("There is the first commit or this commit is burne out by git merge");
			exit(0);
		}
		for (int i = 0; i < centries->rownum; i++) {
			if (centries->name[i] == string("parent")) {
				FILE* fp = fopen(heads, "w");
				fprintf(fp, "%s", centries->para[i].c_str());
				fclose(fp);
				printf("Reset to %s", centries->para[i].c_str());
				int num1;
				treeEntries* trees = comread_tree(repo, string(temp), num1, centries->para[i]);
				IndexEntry entries[20];
				for (int i = 0; i < num1; i++) {
					strcpy(entries[i].sha1, trees[i].sha1.c_str());
					strcpy(entries[i].path, trees[i].path.c_str());
					entries[i].mode = trees[i].mode;
					struct _stat t;
					_stat(entries[i].path, &t);
					entries[i].c_time = t.st_ctime;
					entries[i].m_time = t.st_mtime;
					entries[i].dev = t.st_dev;
					entries[i].ino = t.st_ino;
					entries[i].uid = t.st_uid;
					entries[i].gid = t.st_gid;
				}
				sort(entries, entries + num1, [](IndexEntry& s1, IndexEntry& s2) {
					return strcmp(s1.path, s2.path) < 0;
					});
				write_index(repo, entries, num1 - 1);
				delete centries;
				return 1;
			}
		}
	}
	return -1;
}
//reset to a certain ancestor
int reset(Repository& repo, char* sha1)
{
	set<string> objects;
	string local = string(get_local_master_hash(repo));
	find1(repo, local, objects);
	if (objects.find(string(sha1)) == objects.end()) {
		printf("Cannot reset to such");
		exit(0);
	}
	string path = string(repo.gitdir) + string("\\objects\\") + string(sha1, sha1 + 2);
	path += string("\\") + string(sha1 + 2, sha1 + 40);
	if (_access(path.c_str(), 0) == -1) {
		printf("Cannot reset to %s", sha1);
		exit(0);
	}
	char* road = get_local_master(repo);
	FILE* fp = fopen(road, "w");
	fprintf(fp, "%s", sha1);
	fclose(fp);
	int num1;
	treeEntries *trees = comread_tree(repo, local, num1, string(sha1));
	IndexEntry entries[20];
	for (int i = 0; i < num1; i++) {
		strcpy(entries[i].sha1, trees[i].sha1.c_str());
		strcpy(entries[i].path, trees[i].sha1.c_str());
		entries[i].mode = trees[i].mode;
		struct _stat t;
		_stat(entries[i].path, &t);
		entries[i].c_time = t.st_ctime;
		entries[i].m_time = t.st_mtime;
		entries[i].dev = t.st_dev;
		entries[i].ino = t.st_ino;
		entries[i].uid = t.st_uid;
		entries[i].gid = t.st_gid;
	}
	sort(entries, entries + num1, [](IndexEntry& s1, IndexEntry& s2) {
		return strcmp(s1.path, s2.path) < 0;
	});
	write_index(repo, entries, num1 - 1);
	printf("Reset to %s", sha1);
	return 1;
}
void _checkout(int& retc, char retv[][100], int argc, char* argv[], Ans ans)
{
	if (ans.optNum != 1) {
		printf("Option error");
		exit(0);
	}
	Repository* repo = repo_find();
	checkout(*repo, string(argv[ans.fstPara[0]]), ans.optInd[0]);
	retc = 0;
}
void _reset(int& retc, char retv[][100], int argc, char* argv[], Ans ans)
{
	if (ans.optNum != 1) {
		printf("Option error");
		exit(0);
	}
	Repository* repo = repo_find();
	if (strcmp(argv[ans.fstPara[0]], "HEAD^") == 0)
		reset(*repo, 0);
	else reset(*repo, argv[ans.fstPara[0]]);
	retc = 0;
}