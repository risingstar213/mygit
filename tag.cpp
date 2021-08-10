#include "definitions.h"
//create light-weight tags
int create_tag(string name, string branch)
{
	Repository* repo = repo_find();
	string sha1 = string(get_local_hash(*repo, branch));
	string fullname = string(repo->gitdir) + string("\\refs\\tags\\") + name;
	FILE* fp = fopen(fullname.c_str(), "w");
	fprintf(fp, "%s", sha1.c_str());
	fclose(fp);
	return 1;
}
//create tag object
int create_tag_r(string name, string branch, char * message, const char* author = "star <2262238460@qq.com>")
{
	char commit_data[10000] = { 0 };
	Repository* repo = repo_find();
	char * sha1 = get_local_hash(*repo, branch);
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
	strcpy(commit_data, "object ");
	i += strlen(commit_data);
	for (int k = 0; k < 40; k++, i++) {
		commit_data[i] = sha1[k];
	}
	commit_data[i++] = '\n';
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
	Object* obj = new Object(repo, "tag", commit_data);
	char* ans = object_write(obj, true);
	delete obj;
	string fullname = string(repo->gitdir) + string("\\refs\\tags\\") + name;
	FILE* fp = fopen(fullname.c_str(), "w");
	fprintf(fp, "%s", ans);
	fclose(fp);
	printf("Create tag object:%s", ans);
	return 1;
}
//show tag refs
int checkout_tag(string name)
{
	Repository* repo = repo_find();
	string fullname = string(repo->gitdir) + string("\\refs\\tags\\") + name;
	if (_access(fullname.c_str(), 0) == -1) {
		printf("There isn't such tag");
		exit(0);
	}
	FILE* fp = fopen(fullname.c_str(), "r");
	char data[50];
	fscanf(fp, "%s", data);
	printf( "refs/tags/%s: %s", name.c_str(), data);
	fclose(fp);
	return 1;
}
//show all tags now exsited
int show_tag()
{
	Repository* repo = repo_find();
	char name[100];
	strcat(strcpy(name,repo->gitdir),"\\refs\\tags\\*.*");
	long handle; int ans = 0;
	struct _finddata_t ffblk1, ffblk2;
	if ((handle = _findfirst(name, &ffblk1)) == -1) {
		return 0;
	}
	while (ans >= 0) {
		ffblk2 = ffblk1;
		ans = _findnext(handle, &ffblk1);
		if (!strcmp(ffblk2.name, ".") || !strcmp(ffblk2.name, ".."))continue;
		printf("%s\n", ffblk2.name);
	}
	_findclose(handle);
	return 1;
}
void _tag(int& retc, char retv[][100], int argc, char* argv[], Ans ans)
{
	if (ans.optNum == 0) {
		show_tag();
	}
	else if (ans.optInd[0] == 0 && ans.optNum == 1) {
		create_tag(string(argv[ans.fstPara[0]]), string(argv[ans.lstPara[0]]));
	}
	else if (ans.optInd[0] == 0 && ans.optNum == 2 && ans.optInd[1] == 2)
	{
		create_tag_r(string(argv[ans.fstPara[0]]), string(argv[ans.lstPara[0]]), argv[ans.fstPara[1]]);
	}
	else if(ans.optInd[0] == 1 && ans.optNum == 1){
		checkout_tag(string(argv[ans.fstPara[0]]));
	}
	retc = 0;
}