#include "definitions.h"
//find the full sha1 if it is a short one
string object_resolve(Repository& repo, string name)
{
	if (name == "HEAD") {
		char* temp = get_local_master_hash(repo);
		if (temp == NULL) {
			printf("There isn't any commit yet");
			exit(0);
		}
		return string(temp);
	}
	if (name.length() < 4) {
		printf("The length is so short");
		exit(0);
	}
	for (int i = 0; i < name.length(); i++) {
		if (name[i] >= 'A' && name[i] <= 'Z') {
			name[i] = name[i] - 'A' + 'a';
		}
	}
	string full_name = string(repo.gitdir) + string("\\objects\\") + name.substr(0, 2) + string("\\*.*");
	long handle; int ans = 0;
	struct _finddata_t ffblk1, ffblk2;
	if ((handle = _findfirst(full_name.c_str(), &ffblk1)) == -1) {
		printf("Cannot find such file.");
		exit(0);
	}
	int flag = -1;
	while (ans >= 0) {
		ffblk2 = ffblk1;
		ans = _findnext(handle, &ffblk1);
		if (!strcmp(ffblk2.name, ".") || !strcmp(ffblk2.name, ".."))continue;
		flag = -1;
		for (int i = 2; i < name.length(); i++) {
			if (name[i] != ffblk2.name[i - 2]) {
				flag = i;
				break;
			}
		}
		if (flag == -1) {
			_findclose(handle);
			return name.substr(0, 2) + string(ffblk2.name);
		}
	}
	_findclose(handle);
	printf("Cannot find such file");
	exit(0);
}
//show the data of objects
int cat_file(Repository& repo, string sha1)
{
	string sha_r = object_resolve(repo, sha1);
	char sha[100];
	strcpy(sha, sha_r.c_str());
	Object* obj = object_read(repo, sha, 1);
	int i = 0;
	while (obj->data[i] != '$')putchar(obj->data[i++]);
	return 1;
}
void _cat_file(int& retc, char retv[][100], int argc, char* argv[], Ans ans)
{
	if (ans.optNum == 0) {
		printf("Option error");
		exit(0);
	}
	Repository* repo = repo_find();
	cat_file(*repo, argv[ans.fstPara[0]]);
	retc = 0;
}
void _rev_parse(int& retc, char retv[][100], int argc, char* argv[], Ans ans)
{
	if (ans.optNum == 0) {
		printf("Option error");
		exit(0);
	}
	Repository* repo = repo_find();
	string sha = object_resolve(*repo, argv[ans.fstPara[0]]);
	printf("sha1:%s", sha.c_str());
	retc = 0;
}