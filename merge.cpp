#include "definitions.h"
//find all the ancestor commits
void find1(Repository& repo, string sha1, set <string>& objects)
{
	objects.insert(sha1);
	char sha[50]; strcpy(sha, sha1.c_str());
	Object* obj = object_read(repo, sha, 1);
	//printf("%s\n", commit_sha1.c_str());
	if (strcmp(obj->type, "commit") != 0) {
		printf("find_commit_object error");
		exit(0);
	}
	CommitEntry* centries = read_commit(repo, sha1);
	//printf("search begin");
	for (int i = 0; i < centries->rownum; i++) {
		if (centries->name[i] == string("parent")) {
			find1(repo, centries->para[i], objects);
		}
	}
	delete centries;
	return;
}

//find the lca for two commits
string find2(Repository& repo, string sha1, set <string>& objects)
{
	if (objects.find(sha1) != objects.end()) {
		return sha1;
	}
	char sha[50]; strcpy(sha, sha1.c_str());
	Object* obj = object_read(repo, sha, 1);
	//printf("%s\n", commit_sha1.c_str());
	if (strcmp(obj->type, "commit") != 0) {
		printf("find_commit_object error");
		exit(0);
	}
	CommitEntry* centries = read_commit(repo, sha1);
	//printf("search begin");
	for (int i = 0; i < centries->rownum; i++) {
		if (centries->name[i] == string("parent")) {
			return find2(repo, centries->para[i], objects);
		}
	}
	delete centries;
	return string("\0");
}
//find all the blobs between one commits and its certain ancestor
treeEntries* comread_tree(Repository& repo, std::string sha1, int& num, string ancestor)
{
	if (sha1 == ancestor) return NULL;
	treeEntries* ans1 = NULL,* ans2 = NULL; int num1, num2;
	CommitEntry* centries = read_commit(repo, sha1);
	for (int i = 0; i < centries->rownum; i++) {
		if (centries->name[i] == string("tree")) {
			ans1 = read_tree(repo, centries->para[i], num1);
		}
		if (centries->name[i] == string("parent")) {
			ans2 = comread_tree(repo, centries->para[i], num2, ancestor);
		}
	}
	if (ans2 == NULL) {
		num = num1;
		return ans1;
	}
	int flag = -1;
	for (int i = 0; i < num2; i++) {
		flag = -1;
		for (int j = 0; j < num1; j++) {
			if (ans1->path[j] == ans2->path[i]) {
				flag = j;
				break;
			}
		}
		if (flag == -1) {
			ans1[num1++] = ans2[i];
		}
	}
	num = num1;
	return ans1;
}
//write tree object for merge
char* write_tree_m(Repository& repo, treeEntries * ans, int num)
{
	char commit_data[10000];
	commit_data[0] = '0';
	int j = 0;
	for (int i = 0; i < num; i++) {
		//printf("write_tree %d\n", i);
		_itoa(ans[i].mode, commit_data + j, 8);
		//printf("write tree %ul\n", entries[i].mode);
		j += strlen(commit_data + j);
		commit_data[j++] = ' ';
		strcpy(commit_data + j, ans[i].path.c_str());
		j += strlen(commit_data + j); commit_data[j++] = '\0';
		for (int k = 0; k < 40; k++, j++) {
			commit_data[j] = ans[i].sha1[k];
		}
	}
	commit_data[j] = '$';
	//for (int t = 0; t < j; t++)printf("%c", commit_data[t]);
	Object* obj = new Object(&repo, "tree", commit_data);
	char* anss = object_write(obj, true);
	delete obj;
	//printf("write tree successfully\n");
	return anss;
}
//merge recursively
string merge_folder(Repository& repo, string dir1, string dir2)
{
	int num[3];
	treeEntries* tree[3];
	tree[0] = read_tree(repo, dir1, num[0]);
	tree[1] = read_tree(repo, dir2, num[1]);
	treeEntries* ans = new treeEntries[20];
	int ansnum = 0;
	int flag = -1;
	for (int i = 0; i < num[0]; i++) {
		flag = -1;
		for (int j = 0; j < num[1]; j++) {
			if (tree[0][i].path == tree[1][j].path) {
				flag = j;
				if (CheckFolderExist(tree[0][i].path.c_str())) {
					ans[ansnum] = tree[0][i];
					ans[ansnum++].sha1 = merge_folder(repo, tree[0][i].sha1, tree[1][j].sha1);
				}
				else {
					if (tree[0][i].sha1 != tree[1][j].sha1) {
						printf("Conflict :%s", tree[0][i].path.c_str());
						for (int k = 0; k < 2; k++)delete[] tree[k];
						delete[] ans;
						exit(0);
					}
					else {
						ans[ansnum++] = tree[0][i];
					}
				}
				break;
			}
		}
		if (flag == -1) {
			ans[ansnum++] = tree[0][i];
		}
	}
	for (int j = 0; j < num[1]; j++) {
		flag = -1;
		for (int i = 0; i < num[0]; i++) {
			flag = i; break;
		}
		if (flag == -1)ans[ansnum++] = tree[1][j];
	}
	string sha1 = string(write_tree_m(repo, ans, ansnum));
	for (int k = 0; k < 2; k++)delete[] tree[k];
	delete[] ans;
	return sha1;
}
//write commit for merge
char* commit_m(Repository& repo, string parent, string another, string tree,const char* message, const char* author = "star <2262238460@qq.com>")
{
	char commit_data[10000];
	unsigned long timestamp = (unsigned long)time(0);
	int utc_offset = _timezone;
	char author_time[100] = { 0 };
	_itoa(timestamp, author_time, 10);
	if (utc_offset >= 0) strcat(author_time, " +");
	else strcat(author_time, " -");
	int j = 0;
	while (author_time[j] != '+' && author_time[j] != '-') j++; j++;
	int t1 = (int)fabs(utc_offset) / 3600, t2 = ((int)fabs(utc_offset) / 60) % 60;
	author_time[j++] = t1 / 10 + '0'; author_time[j++] = t1 % 10 + '0';
	author_time[j++] = t2 / 10 + '0'; author_time[j++] = t2 % 10 + '0';
	author_time[j] = '\n';
	int i = 0;
	strcpy(commit_data, "tree ");
	i += strlen(commit_data);
	for (int k = 0; k < 40; k++, i++) {
		//printf("%c", tree[k]);
		commit_data[i] = tree[k];
	}
	commit_data[i++] = '\n';
	if (parent.length() != 0) {
		strcpy(commit_data + i, "parent ");
		i += strlen(commit_data + i);
		for (int k = 0; k < 40; k++, i++)  commit_data[i] = parent[k];
		commit_data[i++] = '\n';
	}
	if (another.length() != 0) {
		strcpy(commit_data + i, "parent ");
		i += strlen(commit_data + i);
		for (int k = 0; k < 40; k++, i++)  commit_data[i] = another[k];
		commit_data[i++] = '\n';
	}
	strcpy(commit_data + i, "author ");
	i += strlen(commit_data + i);
	strcpy(commit_data + i, author);
	i += strlen(commit_data + i); commit_data[i++] = ' ';
	strcpy(commit_data + i, author_time);
	i += strlen(commit_data + i);
	strcpy(commit_data + i, "committer ");
	i += strlen(commit_data + i);
	strcpy(commit_data + i, author);
	i += strlen(commit_data + i); commit_data[i++] = ' ';
	strcpy(commit_data + i, author_time);
	i += strlen(commit_data + i);
	commit_data[i++] = '\n';
	strcpy(commit_data + i, message);
	i += strlen(commit_data + i);
	commit_data[i++] = '\n';
	commit_data[i++] = '$';
	Object* obj = new Object(&repo, "commit", commit_data);
	char* ans = object_write(obj, true);
	delete obj;
	printf("merge to master: %s\n", ans);
	return ans;
}
//the main function of merge
string merge(Repository& repo,string branch1, string branch2){
	if (branch1.length() == 0) {
		CommitEntry* centries = read_commit(repo, branch2);
		for (int i = 0; i < centries->rownum; i++) {
			if (centries->name[i] == string("tree")) {
				char mm[100];
				printf("Please send your message:");
				scanf("%s", mm);
				string mergesha1 = string(commit_m(repo, branch1, branch2, centries->para[i], mm));
				delete centries;
				return mergesha1;
			}
		}
	}
	set<string> objects;
	find1(repo, branch1, objects);
	string parents = find2(repo, branch2, objects);
	int num[2];
	treeEntries* tree[2];
	tree[0] = comread_tree(repo, branch1, num[0], parents);
	tree[1] = comread_tree(repo, branch2, num[1], parents);
	treeEntries* ans = new treeEntries[20];
	int ansnum = 0;
	int flag = -1;
	for (int i = 0; i < num[0]; i++) {
		flag = -1;
		for (int j = 0; j < num[1]; j++) {
			if (tree[0][i].path == tree[1][j].path) {
				flag = j;
				if (CheckFolderExist(tree[0][i].path.c_str())) {
					ans[ansnum] = tree[0][i];
					ans[ansnum++].sha1 = merge_folder(repo, tree[0][i].sha1, tree[1][j].sha1);
				}
				else {
					if (tree[0][i].sha1 != tree[1][j].sha1) {
						printf("Conflict :%s", tree[0][i].path.c_str());
						for (int k = 0; k < 2; k++)delete[] tree[k];
						delete[] ans;
						exit(0);
					}
					else {
						ans[ansnum++] = tree[0][i];
					}
				}
				break;
			}
		}
		if (flag == -1) {
			ans[ansnum++] = tree[0][i];
		}
	}
	for (int j = 0; j < num[1]; j++) {
		flag = -1;
		for (int i = 0; i < num[0]; i++) {
			flag = i; break;
		}
		if(flag == -1)ans[ansnum++] = tree[1][j];
	}
	string treesha1 = string(write_tree_m(repo, ans, ansnum));
	char mm[100];
	printf("Please send your message:");
	scanf("%s", mm);
	string mergesha1 = string(commit_m(repo, branch1, branch2, treesha1, mm));
	for (int k = 0; k < 2; k++)delete[] tree[k];
	delete[] ans;
	return mergesha1;
}
void _merge(int& retc, char retv[][100], int argc, char* argv[], Ans ans)
{
	if (ans.optNum == 0) {
		printf("Option error");
		exit(0);
	}
	Repository* repo = repo_find();
	string branch1 = get_local_hash(*repo, "master");
	string branch2 = get_local_hash(*repo, argv[ans.fstPara[0]]);
	string mergesha1 = merge(*repo, branch1, branch2);
	string path = string(repo->gitdir) + string("\\refs\\heads\\") + string(argv[ans.fstPara[0]]);
	FILE* fp = fopen(path.c_str(), "w");
	fprintf(fp, "%s", mergesha1.c_str()); fclose(fp);
	path = string(repo->gitdir) + string("\\refs\\heads\\") + string("master");
	fp = fopen(path.c_str(), "w");
	fprintf(fp, "%s", mergesha1.c_str()); fclose(fp);
	retc = 0;
}