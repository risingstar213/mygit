#include "definitions.h"
/*struct IndexEntry {
	unsigned long long c_time;
	unsigned long long m_time;
	unsigned long dev;
	unsigned long ino;
	unsigned long mode;
	unsigned long uid;
	unsigned long gid;
	unsigned long size;
	char sha1[45];
	char path[100];
};*/
int stash_add(Repository& repo, string message)
{
	IndexEntry* indexes[20];
	char messages[20][20];
	int num[20] = { 0 }; int numsum = 0;
	for (int i = 0; i < 20; i++) {
		indexes[i] = new IndexEntry[20];
	}
	string full_name = string(repo.gitdir) + string("//stash");
	FILE* fp = fopen(full_name.c_str(), "r");
	if (fp) {
		fscanf(fp, "%d", &numsum);
		for (int i = 0; i < numsum; i++) {
			fscanf(fp, "%d", &num[i]);
			for (int j = 0; j < num[i]; j++) {
				fscanf(fp, "%llu", &indexes[i][j].c_time);
				fscanf(fp, "%llu", &indexes[i][j].m_time);
				fscanf(fp, "%lu", &indexes[i][j].dev);
				fscanf(fp, "%lu", &indexes[i][j].ino);
				fscanf(fp, "%lu", &indexes[i][j].mode);
				fscanf(fp, "%lu", &indexes[i][j].uid);
				fscanf(fp, "%lu", &indexes[i][j].gid);
				fscanf(fp, "%lu", &indexes[i][j].size);
				fscanf(fp, "%s", indexes[i][j].sha1);
				fscanf(fp, "%s", indexes[i][j].path);
			}
			fscanf(fp, "%s", messages[i]);
		}
		fclose(fp);
	}
	int nownum;
	IndexEntry* now = read_index(repo, nownum);
	fp = fopen(full_name.c_str(), "w");
	fprintf(fp, "%d\n", numsum + 1);
	fprintf(fp, "%d\n", nownum);
	for (int i = 0; i < nownum; i++) {
		fprintf(fp, "%llu\n", now[i].c_time);
		fprintf(fp, "%llu\n", now[i].m_time);
		fprintf(fp, "%lu\n", now[i].dev);
		fprintf(fp, "%lu\n", now[i].ino);
		fprintf(fp, "%lu\n", now[i].mode);
		fprintf(fp, "%lu\n", now[i].uid);
		fprintf(fp, "%lu\n", now[i].gid);
		fprintf(fp, "%lu\n", now[i].size);
		fprintf(fp, "%s\n", now[i].sha1);
		fprintf(fp, "%s\n", now[i].path);
	}
	fprintf(fp, "%s\n", message.c_str());
	delete[] now;
	for (int i = 0; i < numsum; i++) {
		fprintf(fp, "%d\n", num[i]);
		for (int j = 0; j < num[i]; j++) {
			fprintf(fp, "%llu\n", indexes[i][j].c_time);
			fprintf(fp, "%llu\n", indexes[i][j].m_time);
			fprintf(fp, "%lu\n", indexes[i][j].dev);
			fprintf(fp, "%lu\n", indexes[i][j].ino);
			fprintf(fp, "%lu\n", indexes[i][j].mode);
			fprintf(fp, "%lu\n", indexes[i][j].uid);
			fprintf(fp, "%lu\n", indexes[i][j].gid);
			fprintf(fp, "%lu\n", indexes[i][j].size);
			fprintf(fp, "%s\n", indexes[i][j].sha1);
			fprintf(fp, "%s\n", indexes[i][j].path);
		}
		fprintf(fp, "%s\n", messages[i]);
	}
	fclose(fp);
	for (int i = 0; i < 20; i++) {
		delete[] indexes[i];
	}
	printf("Add to stash successfully");
	return 1;
}
int stash_list(Repository& repo)
{
	IndexEntry* indexes[20];
	char messages[20][20];
	int num[20] = { 0 }; int numsum = 0;
	for (int i = 0; i < 20; i++) {
		indexes[i] = new IndexEntry[20];
	}
	string full_name = string(repo.gitdir) + string("//stash");
	FILE* fp = fopen(full_name.c_str(), "r");
	if (fp) {
		fscanf(fp, "%d", &numsum);
		for (int i = 0; i < numsum; i++) {
			fscanf(fp, "%d", &num[i]);
			for (int j = 0; j < num[i]; j++) {
				fscanf(fp, "%llu", &indexes[i][j].c_time);
				fscanf(fp, "%llu", &indexes[i][j].m_time);
				fscanf(fp, "%lu", &indexes[i][j].dev);
				fscanf(fp, "%lu", &indexes[i][j].ino);
				fscanf(fp, "%lu", &indexes[i][j].mode);
				fscanf(fp, "%lu", &indexes[i][j].uid);
				fscanf(fp, "%lu", &indexes[i][j].gid);
				fscanf(fp, "%lu", &indexes[i][j].size);
				fscanf(fp, "%s", indexes[i][j].sha1);
				fscanf(fp, "%s", indexes[i][j].path);
			}
			fscanf(fp, "%s", messages[i]);
			printf("stash %d: %s\n", i, messages[i]);
		}
		fclose(fp);
		for (int i = 0; i < 20; i++) {
			delete[] indexes[i];
		}
		return 1;
	}
	else {
		printf("There is to stash now");
		return -1;
	}
}

void rec_folder(Repository& repo, char* sha1)
{
	Object* obj_f = object_read(repo, sha1, 1);
	Object* obj;
	FILE* fp;
	char _name[100];
	char sha[50];
	int j = 0;
	while (obj_f->data[j] != '$') {
		strcpy(_name, obj_f->data + j);
		j += strlen(_name) + 1;
		for (int i = 0; i < 40; i++)sha[i] = obj_f->data[i + j];
		sha[40] = '\0';
		if (CheckFolderExist(_name)) {
			CreateDirectory(_name, NULL);
			rec_folder(repo, sha);
		}
		else {
			obj = object_read(repo, sha, 1);
			fp = fopen(_name, "w");
			int t = 0;
			while (obj->data[t] != '$')fputc(obj->data[t++], fp);
			fclose(fp);
			delete obj;
		}
		j += 40;
	}
	delete[] obj_f;
}
void rec_file(Repository& repo, IndexEntry *entries, int num)
{
	Object* obj;
	FILE* fp;
	for (int i = 0; i < num; i++) {
		if (CheckFolderExist(entries[i].path)) {
			CreateDirectory(entries[i].path, NULL);
			rec_folder(repo, entries[i].sha1);
		}
		else {
			obj = object_read(repo, entries[i].sha1, 1);
			fp = fopen(entries[i].path, "w");
			int j = 0;
			while (obj->data[j] != '$')fputc(obj->data[j++], fp);
			fclose(fp);
			delete obj;
		}
	}
}
int stash_rev(Repository& repo, int ind = 0)
{
	IndexEntry* indexes[20];
	char messages[20][20];
	int num[20] = { 0 }; int numsum = 0;
	for (int i = 0; i < 20; i++) {
		indexes[i] = new IndexEntry[20];
	}
	string full_name = string(repo.gitdir) + string("//stash");
	FILE* fp = fopen(full_name.c_str(), "r");
	if (fp) {
		fscanf(fp, "%d", &numsum);
		for (int i = 0; i < numsum; i++) {
			fscanf(fp, "%d", &num[i]);
			for (int j = 0; j < num[i]; j++) {
				fscanf(fp, "%llu", &indexes[i][j].c_time);
				fscanf(fp, "%llu", &indexes[i][j].m_time);
				fscanf(fp, "%lu", &indexes[i][j].dev);
				fscanf(fp, "%lu", &indexes[i][j].ino);
				fscanf(fp, "%lu", &indexes[i][j].mode);
				fscanf(fp, "%lu", &indexes[i][j].uid);
				fscanf(fp, "%lu", &indexes[i][j].gid);
				fscanf(fp, "%lu", &indexes[i][j].size);
				fscanf(fp, "%s", indexes[i][j].sha1);
				fscanf(fp, "%s", indexes[i][j].path);
			}
			fscanf(fp, "%s", messages[i]);
		}
		fclose(fp);
	}
	else {
		printf("There is to stash now");
		return -1;
	}
	if (ind < 0 || ind >= numsum) {
		printf("Cannot recover");
		for (int i = 0; i < 20; i++) {
			delete[] indexes[i];
		}										
		return -1;
	}
	rec_file(repo, indexes[ind], num[ind]);
	write_index(repo, indexes[ind], num[ind] - 1);
	fp = fopen(full_name.c_str(), "w");
	fprintf(fp, "%d\n", numsum - 1);
	for (int i = 0; i < numsum; i++) {
		if (i == ind) continue;
		fprintf(fp, "%d\n", num[i]);
		for (int j = 0; j < num[i]; j++) {
			fprintf(fp, "%llu\n", indexes[i][j].c_time);
			fprintf(fp, "%llu\n", indexes[i][j].m_time);
			fprintf(fp, "%lu\n", indexes[i][j].dev);
			fprintf(fp, "%lu\n", indexes[i][j].ino);
			fprintf(fp, "%lu\n", indexes[i][j].mode);
			fprintf(fp, "%lu\n", indexes[i][j].uid);
			fprintf(fp, "%lu\n", indexes[i][j].gid);
			fprintf(fp, "%lu\n", indexes[i][j].size);
			fprintf(fp, "%s\n", indexes[i][j].sha1);
			fprintf(fp, "%s\n", indexes[i][j].path);
		}
		fprintf(fp, "%s\n", messages[i]);
	}
	fclose(fp);
	for (int i = 0; i < 20; i++) {
		delete[] indexes[i];
	}
	printf("Revover successfully");
	return 1;
}
void _stash(int& retc, char retv[][100], int argc, char* argv[], Ans ans)
{
	if (ans.optNum != 1) {
		printf("Option error");
		exit(0);
	}
	Repository* repo = repo_find();
	if (ans.optInd[0] == 0) {
		stash_add(*repo, string(argv[ans.fstPara[0]]));
	}
	else if (ans.optInd[0] == 1) {
		stash_list(*repo);
	}
	else {
		int ind = 0;
		int j = 0;
		while (argv[ans.fstPara[0]][j] != '\0') {
			ind = ind * 10 + argv[ans.fstPara[0]][j++] - '0';
		}
		stash_rev(*repo, ind);
	}
}