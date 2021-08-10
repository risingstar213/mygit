#include "definitions.h"
/*	思路：
	将两文件每一行读取出来并赋予一哈希值(采用ELFhash），
	然后该问题可转化为两哈希值序列的LCS（最长公共子序列）问题，
	然后不在LCS中的值则可贪心得认为是修改的行，
	该算法能较好地完成稀疏行修改/删除/增添的文件比较问题。
*/
void _differ(int &retc, char retv[][100],int argc, char* argv[], Ans ans)
{
	//printf("%s %s", argv[ans.fstPara[0]], argv[ans.lstPara[0]]);
	//return;
	for (int i = 0; i < ans.optNum; i++) {
		if (ans.optInd[i] == 0) {
			printf("%s %s\n", argv[ans.fstPara[i]], argv[ans.lstPara[i]]);
			differ(argv[ans.fstPara[i]], argv[ans.lstPara[i]]);
		}
	}
	retc = 0;
}
//ELFhash algorithm calculate hash
unsigned ELFhash(char *str)
{
	unsigned hash = 0, x = 0;
	while (*str != '\n' && *str != '\0') {
		hash = (hash << 4) + (*str++);
		if ((x = hash & 0xf0000000) != 0) {
			hash ^= (x >> 24);
			hash &= ~x;
		}
	}
	return (hash & 0x7fffffff);
}
char f1[1001][1001], f2[1001][1001];//假设文件为1000行以内，每行字符不超过1000
unsigned hash1[1001], hash2[1001];
int dp[1001][1001]; //计算LCS
int lst[1001][1001]; //记录从哪里转移过来的， -1为行转移过来， >=0则为下一行相应列转移过来
//diff two files
void differ(const char* name1,const char* name2)
{
	FILE * fp1, * fp2;
	fp1 = fopen(name1, "r");
	int len1 = 0, len2 = 0;
	for (; !feof(fp1); len1++) {
		fgets(f1[len1], 1001, fp1);
		hash1[len1] = ELFhash(f1[len1]);
	}fclose(fp1);
	fp2 = fopen(name2, "r");
	for (; !feof(fp2); len2++) {
		fgets(f2[len2], 1001, fp2);
		hash2[len2] = ELFhash(f2[len2]);
	}fclose(fp2); len1--; len2--;
	if (hash1[len1] == hash2[len2] && !strcmp(f1[len1], f2[len2])) dp[len1][len2] = 1, lst[len1][len2] = len2 + 1;
	else dp[len1][len2] = 0; dp[len1][len2 + 1] = 0;
	//printf("%d %d %d %s %s\n", dp[len1][len2], len1, len2, f1[len1], f2[len2]);
	for (int j = len2 - 1; j >= 0; j--) {
		dp[len1][j] = dp[len1][j + 1];
		lst[len1][j] = -1;
		if (hash1[len1] == hash2[j] && !strcmp(f1[len1], f2[j]) && dp[len1][j] == 0) {
			dp[len1][j] = 1; lst[len1][j] = j + 1;
		}
	}
	for (int i = len1 - 1; i >= 0; i--) {
		dp[i][len2 + 1] = 0;
		for (int j = len2; j >= 0; j--) {
			if (dp[i][j + 1] > dp[i + 1][j]) {
				dp[i][j] = dp[i][j + 1];
				lst[i][j] = -1;
			}
			else {
				dp[i][j] = dp[i + 1][j];
				lst[i][j] = j;
			}
			if (hash1[i] == hash2[j] && !strcmp(f1[i], f2[j]) && dp[i + 1][j + 1] >= dp[i][j]) {
				dp[i][j] = dp[i + 1][j + 1] + 1;
				lst[i][j] = j + 1;
			}
		}
	}
	int i = 0, j = 0;
	while (i <= len1 && j <= len2) {
		if (lst[i][j] == j + 1) {
			fputs(f1[i], stdout);
			i++; j++;
		}
		else if (lst[i][j] == j) {
			printf("---");
			fputs(f1[i], stdout);
			i++;
		}
		else {
			printf("+++");
			fputs(f2[j], stdout);
			j++;
		}
	}
}
//diff two edition
void differ1(string data1, string data2)
{
	int len1 = 0, len2 = 0;
	int l1 = 0, l2 = 0;
	int n;
	while(true) {
		n = data1.find('\n', l1);
		if (n < 0) {
			strcpy(f1[len1], data1.substr(l1, data1.length() - l1).c_str());
			hash1[len1] = ELFhash(f1[len1]);
			len1++;
			break;
		}
		strcpy(f1[len1], data1.substr(l1, n + 1 - l1).c_str());
		hash1[len1] = ELFhash(f1[len1]);
		l1 = n + 1;
		len1++;
	} 
	while (true) {
		n = data2.find('\n', l2);
		if (n < 0) {
			strcpy(f2[len2], data2.substr(l2, data2.length() - l2).c_str());
			hash2[len2] = ELFhash(f2[len2]);
			len2++;
			break;
		}
		strcpy(f2[len2], data2.substr(l2, n + 1 - l2).c_str());
		hash2[len2] = ELFhash(f2[len2]);
		l2 = n + 1;
		len2++;
	}
	len1--; len2--;
	if (hash1[len1] == hash2[len2] && !strcmp(f1[len1], f2[len2])) dp[len1][len2] = 1, lst[len1][len2] = len2 + 1;
	else dp[len1][len2] = 0; dp[len1][len2 + 1] = 0;
	//printf("%d %d %d %s %s\n", dp[len1][len2], len1, len2, f1[len1], f2[len2]);
	for (int j = len2 - 1; j >= 0; j--) {
		dp[len1][j] = dp[len1][j + 1];
		lst[len1][j] = -1;
		if (hash1[len1] == hash2[j] && !strcmp(f1[len1], f2[j]) && dp[len1][j] == 0) {
			dp[len1][j] = 1; lst[len1][j] = j + 1;
		}
	}
	for (int i = len1 - 1; i >= 0; i--) {
		dp[i][len2 + 1] = 0;
		for (int j = len2; j >= 0; j--) {
			if (dp[i][j + 1] > dp[i + 1][j]) {
				dp[i][j] = dp[i][j + 1];
				lst[i][j] = -1;
			}
			else {
				dp[i][j] = dp[i + 1][j];
				lst[i][j] = j;
			}
			if (hash1[i] == hash2[j] && !strcmp(f1[i], f2[j]) && dp[i + 1][j + 1] >= dp[i][j]) {
				dp[i][j] = dp[i + 1][j + 1] + 1;
				lst[i][j] = j + 1;
			}
		}
	}
	int i = 0, j = 0;
	while (i <= len1 && j <= len2) {
		if (lst[i][j] == j + 1) {
			fputs(f1[i], stdout);
			i++; j++;
		}
		else if (lst[i][j] == j) {
			printf("---");
			fputs(f1[i], stdout);
			i++;
		}
		else {
			printf("+++");
			fputs(f2[j], stdout);
			j++;
		}
	}
}
//diff the files in folders
int diff_folder(Repository & repo, string sha1)
{
	char sha[100];
	char data[10000];
	strcpy(sha, sha1.c_str());
	Object* obj2 = object_read(repo, sha, 1);
	int n = 0;
	while (obj2->data[n] != '$')n++;
	string folder = string(obj2->data, obj2->data + n);
	string path; string sha_r;
	char* sha_n;
	int i = 0;
	while (true) {
		n = folder.find('\0', i);
		if (n < 0)break;
		path = folder.substr(i, n);
		i = n + 1;
		sha_r = folder.substr(i, 40);
		if (CheckFolderExist(path.c_str())) {
			diff_folder(repo, sha_r);
		}
		else {
			FILE* fp = fopen(path.c_str(), "r");
			int num = fread(data, 1, 10000, fp);
			data[num] = '$'; fclose(fp);
			Object obj(&repo, "blob", data);
			sha_n = object_write(&obj, 0);
			if (strcmp(sha_r.c_str(), sha_n) != 0) {
				printf("%s\n\n", path.c_str());
				strcpy(sha, sha_r.c_str());
				Object* obj1 = object_read(repo, sha, num);
				int n = 0;
				while (obj1->data[n] != '$')n++;
				differ1(string(obj1->data, obj1->data + n), string(data, data + num));
				printf("\n\n");
			}
		}
		i += 40;
	}
	return 1;
}
//diff the files loaded in tree objects
int diff_tree(Repository& repo, string sha1)
{
	int n;
	char* data = new char[10000];
	char* sha_n;
	char sha[50];
	treeEntries *trees = read_tree(repo, sha1, n);
	for (int i = 0; i < n; i++) {
		if (CheckFolderExist(trees[i].path.c_str())) {
			diff_tree(repo, trees[i].sha1);
		}
		else {
			FILE* fp = fopen(trees[i].path.c_str(), "r");
			int num = fread(data, 1, 10000, fp);
			data[num] = '$'; fclose(fp);
			Object obj(&repo, "blob", data);
			sha_n = object_write(&obj, 0);
			if (string(sha_n) != trees[i].sha1) {
				printf("%s\n\n", trees[i].path.c_str());
				strcpy(sha, trees[i].sha1.c_str());
				Object* obj1 = object_read(repo, sha, num);
				int n = 0;
				while (obj1->data[n] != '$')n++;
				differ1(string(obj1->data, obj1->data + n), string(data, data + num));
				printf("\n\n");
			}
		}
	}
	delete[] data;
	return 1;
}
//judge whether diff function to choose
int diff()
{
	int num;
	Repository* repo = repo_find();
	char* sha1;
	char data[10000];
	IndexEntry* entries = read_index(*repo, num);
	if (num == 0) {
		string sha_c = string(get_local_master_hash(*repo));
		CommitEntry* centries = read_commit(*repo, sha_c);
		for (int i = 0; i < centries->rownum; i++) {
			if (centries->name[i] == string("tree")) {
				diff_tree(*repo, centries->para[i]);
				break;
			}
		}
		delete centries;
		return 1;
	}
	for (int i = 0; i < num; i++) {
		if (CheckFolderExist(entries[i].path)) {
			diff_folder(*repo, string(entries[i].sha1, entries[i].sha1 + 40));
			continue;
		}
		FILE* fp = fopen(entries[i].path, "r");
		int num = fread(data, 1, 10000, fp);
		data[num] = '$'; fclose(fp);
		Object obj(repo, "blob", data);
		sha1 = object_write(&obj, 0);
		if (strcmp(sha1, entries[i].sha1) != 0) {
			printf("%s\n\n", entries[i].path);
			Object* obj1 = object_read(*repo, entries[i].sha1, num);
			int n = 0;
			while (obj1->data[n] != '$')n++;
			differ1(string(obj1->data, obj1->data + n), string(data, data + num));
			printf("\n\n");
		}
	}
	return 1;
}
void _diff(int& retc, char retv[][100], int argc, char* argv[], Ans ans)
{
	if (ans.optNum != 0) {
		printf("Option error");
		exit(0);
	}
	diff();
}