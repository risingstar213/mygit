#include "definitions.h"
//find sha1 in folders
char* find_in_file(Repository& repo,char* sha1, char* name)
{
	static int k = 0;
	//printf("find in file %d\n", k);
	//printf("%s %s\n", sha1, name);
	Object* obj = object_read(repo, sha1, 0);
	//printf("object complete");
	char _name[100];
	char* sha = new char[50];
	int j = 0;
	while (obj->data[j] != '$') {
		strcpy(_name, obj->data + j);
		j += strlen(_name) + 1;
		//printf("\n%s %s %d\n", _name, name, strcmp(_name, name));
		if (!strcmp(_name, name)) {
			for (int i = 0; i < 40; i++, j++) {
				sha[i] = obj->data[j];
			}
			sha[40] = '\0';
			return sha;
		}
		j += 40;
	}
	printf("FIND_INFILE ERROR\n");
	exit(0);
}
//write tree object recursively
char * write_tree1(Repository& repo, char* name, char * sha1)
{
	char commit_data[10000];
	commit_data[0] = '0';
	int num = 0;
	char _name[100];
	char __name[100];
	strcat(strcpy(_name, name), "\\*.*");
	long handle; int ans = 0;
	struct _finddata_t ffblk1, ffblk2;
	int j = 0;
	if ((handle = _findfirst(_name, &ffblk1)) == -1) {
		commit_data[j] = '$';
		Object* obj = new Object(&repo, "tree", commit_data);
		char* anss = object_write(obj, true);
		delete obj;
		return anss;
	}
	int i = 0;
	while (ans >= 0) {
		ffblk2 = ffblk1;
		ans = _findnext(handle, &ffblk1);
		if (!strcmp(ffblk2.name, ".") || !strcmp(ffblk2.name, ".."))continue;
		//printf("write_tree1 %d\n", i++);
		strcat(strcat(strcpy(__name, name), "\\"), ffblk2.name);
		struct _stat t;
		_stat(__name, &t);
		_itoa(t.st_mode, commit_data + j, 8);
		j += strlen(commit_data + j);
		commit_data[j++] = ' ';
		strcpy(commit_data + j, __name);
		j += strlen(commit_data + j); commit_data[j++] = '\0';
		if (CheckFolderExist(__name)) {

			char* insha1 = find_in_file(repo, sha1, __name);
			char* thissha1 = write_tree1(repo, __name, insha1);
			for (int k = 0; k < 40; k++, j++) {
				commit_data[j] = thissha1[k];
			}
			delete[] insha1;
		}
		else {
			char* thissha1 = find_in_file(repo, sha1, __name);
			for (int k = 0; k < 40; k++, j++) {
				commit_data[j] = thissha1[k];
			}
			delete[] thissha1;
		}
	}
	_findclose(handle);
	commit_data[j] = '$';
	Object* obj = new Object(&repo, "tree", commit_data);
	char* anss = object_write(obj, true);
	delete obj;
	return anss;
}
//write tree object
char * write_tree(Repository& repo)
{
	char commit_data[10000];
	commit_data[0] = '0';
	int num = 0;
	IndexEntry* entries = read_index(repo, num);
	int j = 0;
	for (int i = 0; i < num; i++) {
		//printf("write_tree %d\n", i);
		_itoa(entries[i].mode, commit_data + j, 8);
		//printf("write tree %ul\n", entries[i].mode);
		j += strlen(commit_data + j);
		commit_data[j++] = ' ';
		strcpy(commit_data + j, entries[i].path);
		j += strlen(commit_data + j); commit_data[j++] = '\0';
		if (CheckFolderExist(entries[i].path)) {
			char* sha1 = write_tree1(repo, entries[i].path, entries[i].sha1);
			for (int k = 0; k < 40; k++, j++) {
				commit_data[j] = sha1[k];
			}
		}
		else {
			for (int k = 0; k < 40; k++, j++) {
				commit_data[j] = entries[i].sha1[k];
			}
		}
	}
	commit_data[j] = '$';
	//for (int t = 0; t < j; t++)printf("%c", commit_data[t]);
	Object *obj = new Object(&repo, "tree", commit_data);
	char* ans = object_write(obj, true);
	delete obj;
	//printf("write tree successfully\n");
	return ans;
}

//write commit object
char* commit(const char* message, const char* author = "star <2262238460@qq.com>")
{
	char commit_data[10000];
	Repository* repo = repo_find();
	char* tree = write_tree(*repo);
	char* parent = get_local_master_hash(*repo);
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
	if (parent) {
		strcpy(commit_data + i, "parent ");
		i += strlen(commit_data + i);
		for (int k = 0; k < 40; k++, i++)  commit_data[i] = parent[k];
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
	Object* obj = new Object(repo, "commit", commit_data);
	char* ans = object_write(obj, true);
	delete obj;
	char* path = get_local_master(*repo);
	if (path) {
		//printf("???");
		FILE* fp = fopen(path, "w");
		fwrite(ans, 1, 40, fp);
		fclose(fp);
	}
	int num;
	IndexEntry* entries = read_index(*repo, num);
	write_index(*repo, entries, -1);
	printf("commit to master: %s", ans);
	return ans;
}
void _commit(int& retc, char retv[][100], int argc, char* argv[], Ans ans)
{
	if (ans.optNum == 0) {
		printf("error");
		exit(0);
	}
	if (ans.optInd[0] == 0) {
		char message[100];
		printf("Please enter the message for this commit:");
		cin.getline(message, 100);
		commit(message);
	}
}