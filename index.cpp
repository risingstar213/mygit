#include "definitions.h"

char indexdata[10000];
//read the information in index
IndexEntry* read_index(Repository& repo, int& num)
{
	num = 0;
	char indexpath[100];
	strcat(strcpy(indexpath, repo.gitdir), "\\index");
	FILE* fp = fopen(indexpath, "rb");
	if (!fp) {
		//printf("There is no index\n");
		//exit(0);
		num = 0;
		return new IndexEntry[3];
	}
	int n = fread(indexdata, 1, 10000, fp);
	fclose(fp);
	char _sha1[45] = { 0 };
	strcpy(_sha1, sha1(string(indexdata, indexdata + n - 40)).c_str());
	for (int i = 0; i < 40; i++) {
		if (_sha1[i] != indexdata[n - 40 + i]) {
			printf("invalid index checksum");
		}
	}
	char signature[5] = { 0 };
	for (int i = 0; i < 4; i++) {
		signature[i] = indexdata[i];
	}
	if (strcmp(signature, "DIRC")) {
		printf("invalid index signature %s\n", signature);
		exit(0);
	}
	int version = 0;
	for (int i = 4; i < 8; i++) {
		version = version * 256 + indexdata[i] ;
	}
	if (version != 2) {
		printf("unknown index version %u", version);
		exit(0);
	}
	num = 0;
	for(int i = 8; i < 12; i++){
		num = num * 256 + indexdata[i];
	}
	int i = 12, j = 0, k;
	IndexEntry* entries = new IndexEntry[num + 3];
	while (i + 40 < n) {
		entries[j].c_time = 0;
		for (k = 0; k < 8; k++, i++) {
			entries[j].c_time = entries[j].c_time * 256 + (unsigned char)indexdata[i];
		}
		entries[j].m_time = 0;
		for (k = 0; k < 8; k++, i++) {
			entries[j].m_time = entries[j].m_time * 256 + (unsigned char)indexdata[i];
		}
		entries[j].dev = 0;
		for (k = 0; k < 4; k++, i++) {
			entries[j].dev = entries[j].dev * 256 + (unsigned char)indexdata[i];
		}
		entries[j].ino = 0;
		for (k = 0; k < 4; k++, i++) {
			entries[j].ino = entries[j].ino * 256 + (unsigned char)indexdata[i];
		}
		entries[j].mode = 0;
		for (k = 0; k < 4; k++, i++) {
			entries[j].mode = entries[j].mode * 256 + (unsigned char)indexdata[i];
		}
		entries[j].uid = 0;
		for (k = 0; k < 4; k++, i++) {
			entries[j].uid = entries[j].uid * 256 + (unsigned char)indexdata[i];
		}
		entries[j].gid = 0;
		for (k = 0; k < 4; k++, i++) {
			entries[j].gid = entries[j].gid * 256 + (unsigned char)indexdata[i];
		}
		entries[j].size = 0;
		for (k = 0; k < 4; k++, i++) {
			entries[j].size = entries[j].size * 256 + (unsigned char)indexdata[i];
		}
		for (k = 0; k < 40; k++, i++) {
			entries[j].sha1[k] = indexdata[i];
		}
		entries[j].sha1[40] = '\0';
		k = 0;
		while (indexdata[i]) {
			entries[j].path[k++] = indexdata[i++];
		}
		entries[j].path[k++] = indexdata[i++];
		//printf("read index %d %s %s\n", j, entries[j].path, entries[j].sha1);
		j++;
	}
	if (j != num) {
		printf("index num error\n");
		exit(0);
	}
	//printf("Read index successfully \n", num);
	return entries;
}
/*string LastTime(string fileName)
{
	std::filesystem::path path(fileName); //fileName中是文件路径
	if (std::filesystem::exists(path)) //判断文件是否存在
	{
		struct _stat t;
		if (_stat(fileName.c_str(), &t) != 0)
		{
			return string("");
		}
		::tm tm;
		::_localtime64_s(&tm, &t.st_mtime);
		char buf[30];
		::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
		return string(buf);
	}
	return string("");
}*/
//calculate hash for folders recursively
char* hash_folder(Repository& repo, char * name, int& n, int ind)
{
	n = 0;
	char _name[100];
	char __name[100];
	strcat(strcpy(_name, name), "\\*.*");
	char* data = new char[5000];
	data[0] = '\0';
	long handle; int ans = 0;
	struct _finddata_t ffblk1, ffblk2;
	if ((handle = _findfirst(_name, &ffblk1)) == -1) {
		data[n] = '$';
		return data;
	}
	while (ans >= 0) {
		ffblk2 = ffblk1;
		ans = _findnext(handle, &ffblk1);
		if (!strcmp(ffblk2.name, ".") || !strcmp(ffblk2.name, ".."))continue;
		strcat(strcat(strcpy(__name, name), "\\"), ffblk2.name);
		strcpy(data + n, __name);
		n += strlen(data + n); data[n++] = '\0';
		if (CheckFolderExist(__name)) {
			int num;
			char* temp = hash_folder(repo, __name, num, ind);
			Object obj(&repo, "blob", temp);
			char * sha = object_write(&obj, ind);
			for (int i = 0; i < 40; i++, n++) data[n] = sha[i];
			delete []temp;
		}
		else {
			FILE* fp = fopen(__name, "r");
			char *temp = new char[10000];
			int nn = fread(temp, 1, 10000, fp); fclose(fp);
			temp[nn] = '$';
			Object obj(&repo, "blob", temp);
			char* sha = object_write(&obj, ind);
			for (int i = 0; i < 40; i++, n++) data[n] = sha[i];
			delete []temp;
		}
	}
	_findclose(handle);
	data[n] = '$';
	data[n + 1] = '\0';
	return data;
}
//write index straightly
int write_index(Repository& repo, IndexEntry* entries, int num)
{
	strcpy(indexdata, "DIRC");
	indexdata[4] = indexdata[5] = indexdata[6] = 0;
	indexdata[7] = 2;
	unsigned long long temp = (long long)num + 1;
	for (int i = 11; i >= 8; i--) {
		indexdata[i] = temp % 256;
		temp >>= 8;
	}
	int i = 12, j = 0, k;
	while (j <= num) {
		temp = entries[j].c_time;
		for (k = 0; k < 8; k++, i++) {
			indexdata[i - 2 * k + 7] = temp % 256;
			temp >>= 8;
		}
		temp = entries[j].m_time;
		for (k = 0; k < 8; k++, i++) {
			indexdata[i - 2 * k + 7] = temp % 256;
			temp >>= 8;
		}
		temp = entries[j].dev;
		for (k = 0; k < 4; k++, i++) {
			indexdata[i - 2 * k + 3] = temp % 256;
			temp >>= 8;
		}
		temp = entries[j].ino;
		for (k = 0; k < 4; k++, i++) {
			indexdata[i - 2 * k + 3] = temp % 256;
			temp >>= 8;
		}
		temp = entries[j].mode;
		for (k = 0; k < 4; k++, i++) {
			indexdata[i - 2 * k + 3] = temp % 256;
			temp >>= 8;
		}
		temp = entries[j].uid;
		for (k = 0; k < 4; k++, i++) {
			indexdata[i - 2 * k + 3] = temp % 256;
			temp >>= 8;
		}
		temp = entries[j].gid;
		for (k = 0; k < 4; k++, i++) {
			indexdata[i - 2 * k + 3] = temp % 256;
			temp >>= 8;
		}
		temp = entries[j].size;
		for (k = 0; k < 4; k++, i++) {
			indexdata[i - 2 * k + 3] = temp % 256;
			temp >>= 8;
		}
		for (k = 0; k < 40; k++, i++) {
			indexdata[i] = entries[j].sha1[k];
		}
		k = 0;
		while (entries[j].path[k]) {
			indexdata[i++] = entries[j].path[k++];
		}
		indexdata[i++] = '\0';
		j++;
	}
	strcpy(indexdata + i, sha1(string(indexdata, indexdata + i)).c_str());
	char indexpath[100];
	strcat(strcpy(indexpath, repo.gitdir), "\\index");
	FILE* fp = fopen(indexpath, "wb");
	//for (int t = 0; t < i + 40; t++) printf("%c",indexdata[t] );
	fwrite(indexdata, 1, i + 40, fp); fclose(fp);
	return 1;
}
//add an element ans write in index
int write_index(Repository& repo, char* name)
{
	int num = 0;
	IndexEntry * entries = read_index(repo, num);
	char* dd; int n;
	if (CheckFolderExist(name)) {
		dd = hash_folder(repo, name, n, 1);
	}
	else {
		FILE* fp = fopen(name, "r");
		if (!fp) {
			printf("No such file or directory!");
			exit(0);
		}
		dd = new char[10000];
		n = fread(dd, 1, 10000, fp); fclose(fp);
		dd[n] = '$';
	}
	Object obj(&repo, "blob", dd);
	strcpy(entries[num].sha1, object_write(&obj, 1));
	//printf("\n %s %s\n",name, entries[num].sha1);
	delete []dd;
	//string ss = LastTime(string(name, name + strlen(name)));
	struct _stat t;
	_stat(name, &t);
	entries[num].c_time = t.st_ctime;
	entries[num].m_time = t.st_mtime;
	entries[num].dev = t.st_dev;
	entries[num].ino = t.st_ino;
	entries[num].mode = t.st_mode;
	entries[num].uid = t.st_uid;
	entries[num].gid = t.st_gid;
	strcpy(entries[num].path, name);
	entries[num].size = n;
	int flag = -1;
	for (int i = 0; i < num; i++) {
		//printf("%lld %d %s %s \n",entries[i].m_time, entries[i].size, entries[i].path, entries[i].sha1);
		if (strcmp(entries[i].path, name) == 0) {
			flag = i;
			break;
		}
	}
	if (flag == -1) {
		sort(entries, entries + num + 1, [](IndexEntry& s1, IndexEntry&  s2) {
			return strcmp(s1.path, s2.path) < 0;
			});
	}
	else {
		swap(entries[flag], entries[num]);
		printf("Updata %s\n", name);
		num--;
	}
	strcpy(indexdata, "DIRC");
	indexdata[4] = indexdata[5] = indexdata[6] = 0;
	indexdata[7] = 2;
	unsigned long long temp = (long long)num + 1;
	for (int i = 11; i >= 8; i--) {
		indexdata[i] = temp % 256;
		temp >>= 8;
	}
	int i = 12, j = 0, k;
	while (j <= num) {
		temp = entries[j].c_time;
		for (k = 0; k < 8; k++, i++) {
			indexdata[i - 2 * k + 7] = temp % 256;
			temp >>= 8;
		}
		temp = entries[j].m_time;
		for (k = 0; k < 8; k++, i++) {
			indexdata[i - 2 * k + 7] = temp % 256;
			temp >>= 8;
		}
		temp = entries[j].dev;
		for (k = 0; k < 4; k++, i++) {
			indexdata[i - 2 * k + 3] = temp % 256;
			temp >>= 8;
		}
		temp = entries[j].ino;
		for (k = 0; k < 4; k++, i++) {
			indexdata[i - 2 * k + 3] = temp % 256;
			temp >>= 8;
		}
		temp = entries[j].mode;
		for (k = 0; k < 4; k++, i++) {
			indexdata[i - 2 * k + 3] = temp % 256;
			temp >>= 8;
		}
		temp = entries[j].uid;
		for (k = 0; k < 4; k++, i++) {
			indexdata[i - 2 * k + 3] = temp % 256;
			temp >>= 8;
		}
		temp = entries[j].gid;
		for (k = 0; k < 4; k++, i++) {
			indexdata[i - 2 * k + 3] = temp % 256;
			temp >>= 8;
		}
		temp = entries[j].size;
		for (k = 0; k < 4; k++, i++) {
			indexdata[i - 2 * k + 3] = temp % 256;
			temp >>= 8;
		}
		for (k = 0; k < 40; k++, i++) {
			indexdata[i] = entries[j].sha1[k];
		}
		k = 0;
		while (entries[j].path[k]) {
			indexdata[i++] = entries[j].path[k++];
		}
		indexdata[i++] = '\0';
		j++;
	}
	strcpy(indexdata + i, sha1(string(indexdata, indexdata + i)).c_str());
	char indexpath[100];
	strcat(strcpy(indexpath, repo.gitdir), "\\index");
	FILE * fp = fopen(indexpath, "wb");
	//for (int t = 0; t < i + 40; t++) printf("%c",indexdata[t] );
	fwrite(indexdata, 1, i + 40, fp); fclose(fp);
	return 1;
}