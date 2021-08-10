#include "definitions.h"
//meaningless files...
transLines* extract_lines(string data)
{
	int i = 0;
	transLines* lines = new transLines;
	lines->num = 0;
	int& num = lines->num;
	int n;
	while (true) {
		lines->len[num] = 0;
		for (int j = 0; j < 4; j++) {
			if (data[i + j] >= '0' && data[i + j] <= '9') n = data[i + j] - '0';
			else n = data[i + j] - 'a' + 10;
			lines->len[num] = lines->len[num] * 16 + n;
		}
		if (lines->len[num] != 0) {
			lines->lines[num] = data.substr(i + 4, lines->len[num] - 4);
			i += lines->len[num];
		}
		else {
			lines->lines[num] = string("\0");
			i += 4;
		}
		//printf("%d:%d %s\n", i, lines->len[num],lines->lines[num].c_str());
		num++;
		if ((unsigned)i >= data.length())break;
	}
	return lines;
}
string build_lines_data(transLines* lines)
{
	string result;
	char temp[10];
	int n;
	for (int i = 0; i < lines->num; i++) {
		n = lines->len[i] + 5;
		for (int j = 3; j >= 0; j--) {
			if (n % 16 > 9) temp[j] = (n % 9) - 10 + 'a';
			else temp[j] = (n % 9) + '0';
			n /= 16;
		}
		result += string(temp, temp + 4);
		result += lines->lines[i];
		result += string("\n");
	}
	result += string("0000");
	//printf("%s", result.c_str());
	//exit(0);
	return result;
}
string http_request(string url, string username, string password)
{
	Py_Initialize();
	if (!Py_IsInitialized())
	{
		printf("初始化失败！");
		exit(0);
	}
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('C:/Users/star/Desktop/test/')");//这一步很重要，修改Python路径
	PyObject* pModule = NULL;//声明变量
	PyObject* pFunc1 = NULL;// 声明变量
	if (_access("C:/Users/star/Desktop/test/requestbat.py", 0) == -1) {
		cout << "Bat don't exist";
	}
	pModule = PyImport_ImportModule("requestbat");//这里是要调用的文件名hello.py
	if (pModule == NULL)
	{
		cout << "没找到文件" << endl;
		exit(0);
	}
	pFunc1 = PyObject_GetAttrString(pModule, "http_request");//这里是要调用的函数名
	if (pFunc1 == NULL) {
		cout << "没找到函数" << endl;
		exit(0);
	}
	PyObject* args2 = Py_BuildValue("sss", url.c_str(), username.c_str(), password.c_str());//给python函数参数赋值
	PyObject* pRet = PyObject_CallObject(pFunc1, args2);//调用函数
	//printf("OK"); exit(0);
	if (!pRet) {
		printf("Python error");
		exit(0);
	}
	/*char* res = NULL;
	PyArg_Parse(pRet, "s", &res);//转换返回类型
	if (res == NULL) {
		printf("return error;");
		exit(0);
	}*/
	char res[1000];
	FILE* fp = fopen("C:/Users/star/Desktop/test/test", "r");
	int num = fread(res, 1, 1000, fp); fclose(fp);

	Py_Finalize(); // 与初始化对应
	return string(res, res + num);
}

string get_remote_master_hash(string git_url, string username, string password)
{
	string url = git_url + string("/info/refs?service=git-receive-pack");
	string response = http_request(url, username, password);
	//string response = string("001f# service=git-receive-pack\n000000b10000000000000000000000000000000000000000 capabilities^{} report-status report-status-v2 delete-refs side-band-64k quiet atomic ofs-delta object-format=sha1 agent=git/2.31.1\n0000");
	transLines * lines = extract_lines(response);
	//printf("test"); exit(0);
	if (lines->lines[0] != string("# service=git-receive-pack\n")) {
		printf("http error");
		exit(0);
	}
	if (lines->lines[1].length() != 0) {
		printf("http error");
		exit(0);
	}
	string temp;
	for (int i = 0; i < 40; i++)temp += string("0");
	if (lines->lines[2].substr(0, 40) == temp) {
		return string("\0");
	}
	int i = lines->lines[2].find(' ', 0);
	string master_sha1 = lines->lines[2].substr(0, i);
	int j = lines->lines[2].find('\x00');
	string master_ref = lines->lines[2].substr(i + 1, j);
	if (master_sha1.length() != 40) {
		printf("The length of remote_master_hash is not correct");
		exit(0);
	}
	if (master_ref != string("refs/heads/master")) {
		printf("The branch of remote respository is not correct:%s", master_ref.c_str());
		exit(0);
	}
	//printf("remote hash:%s\n", master_sha1.c_str());
	return master_sha1;
}
treeEntries* read_tree(Repository & repo,string sha1, int& num)
{
	treeEntries* trees = new treeEntries[100];
	num = 0;
	int n = 0;
	char sha[50]; strcpy(sha, sha1.c_str());
	Object* obj = object_read(repo, sha, 1);
	while (obj->data[n] != '$')n++;
	string sdata = string(obj->data, obj->data + n);
	n = 0; int i, j, t;
	while (true) {
		i = sdata.find(' ', n);
		if (i < 0) break;
		t = 0;
		for (int k = n; k < i; k++) {
			t = t * 8 + sdata[k] - '0';
			//printf("%c", sdata[k]);
		}
		//printf("\n");
		trees[num].mode = t;
		n = i + 1;
		j = sdata.find('\x00', n);
		if (j < 0) {
			printf("Tree object Error");
			exit(0);
		}
		trees[num].path = sdata.substr(n, j - n);
		n = j + 1;
		if (n + 39 > (signed)sdata.length()) {
			printf("Tree object Error");
			exit(0);
		}
		trees[num].sha1 = sdata.substr(n, 40);
		num++;
		n += 40;
	}
	/*for (int i = 0; i < num; i++) {
		printf("%lu %s %s\n", trees[i].mode, trees[i].path.c_str(), trees[i].sha1.c_str());
	}*/
	//exit(0);
	return trees;
}
set <string> find_tree_object(Repository& repo,string tree_sha1)
{
	set <string> objects, temp, temp1;
	objects.insert(tree_sha1);
	int num;
	//printf("read tree begin");
	treeEntries *trees = read_tree(repo, tree_sha1, num);
	//printf("read tree end");
	for (int i = 0; i < num; i++) {
		if (trees[i].mode == 040000) {
			temp = find_tree_object(repo, trees[i].sha1);
			set_union(objects.begin(), objects.end(), temp.begin(), temp.end(),inserter(temp1, temp1.begin()));
			objects = temp1;
			temp.clear(); temp1.clear();
		}
		else {
			objects.insert(trees[i].sha1);
		}
	}
	//printf("tree end");
	delete[] trees;
	return objects;
}
set <string> find_commit_object(Repository& repo, string commit_sha1)
{
	set <string> objects, temp, temp1;
	objects.insert(commit_sha1);
	char sha[50]; strcpy(sha, commit_sha1.c_str());
	Object* obj = object_read(repo, sha, 1);
	//printf("%s\n", commit_sha1.c_str());
	if (strcmp(obj->type, "commit") != 0) {
		printf("find_commit_object error");
		exit(0);
	}
	CommitEntry * centries = read_commit(repo, commit_sha1);
	//printf("search begin");
	for (int i = 0; i < centries->rownum; i++) {
		if (centries->name[i] == string("tree")) {
			temp = find_tree_object(repo, centries->para[i]);
			set_union(objects.begin(), objects.end(), temp.begin(), temp.end(), inserter(temp1, temp1.begin()));
			objects = temp1;
			temp.clear(); temp1.clear();
		}
		else if (centries->name[i] == string("parent")) {
			temp = find_commit_object(repo, centries->para[i]);
			set_union(objects.begin(), objects.end(), temp.begin(), temp.end(), inserter(temp1, temp1.begin()));
			objects = temp1;
			temp.clear(); temp1.clear();
		}
	}
	delete centries;
	return objects;
}
set <string> find_missing_objects(Repository & repo,string local_sha1, string remote_sha1)
{
	//printf("commit begin");
	set <string> local_objects = find_commit_object(repo, local_sha1);
	//printf("commit finish");
	if (remote_sha1.length() == 0) {
		return local_objects;
	}
	set <string> remote_objects = find_commit_object(repo, remote_sha1);
	set <string> temp;
	set_difference(local_objects.begin(), local_objects.end(), remote_objects.begin(), remote_objects.end(), inserter(temp, temp.begin()));
	return temp;
}

string encode_pack_object(Repository& repo, string sha1)
{
	Object* obj = object_read(repo, (char*)sha1.c_str(), 1);
	int type_num = 0;
	if (strcmp(obj->type, "commit") == 0)type_num = 1;
	else if (strcmp(obj->type, "tree") == 0)type_num = 2;
	else if (strcmp(obj->type, "blob") == 0)type_num = 3;
	else {
		printf("encode error");
		exit(0);
	}
	unsigned size = 0, size0;
	while (obj->data[size] != '$')size++;
	size0 = size;
	unsigned char byte = (type_num << 4) | (size & 0x0f);
	size >>= 4;
	string header;
	unsigned char temp;
	while (size) {
		temp = byte | 0x80;
		header.append(1, temp);
		byte = size & 0x7f;
		size >>= 7;
	}
	temp = byte;
	header.append(1, temp);
	Bytef* data = new Bytef[10000];
	uLongf len = 10000;
	compress(data, &len, (const Bytef*)obj->data, size0);
	header += string(data, data + len);
	delete[] data;
	return header;
}
string create_pack(Repository& repo,set<string> objects)
{
	string header;
	header.append("PACK");
	header.append(3, (char)0);
	header.append(1, (char)2);
	unsigned temp = objects.size();
	char temp1[4];
	for (int i = 3; i >= 0; i--) {
		temp1[i] = temp % 256;
		temp >>= 8;
	}
	for (int i = 0; i < 4; i++) {
		header.append(1, temp1[i]);
	}
	string body;
	for (auto& i : objects) {
		body += encode_pack_object(repo, i);
	}
	string contents = header + body; header.clear(); body.clear();
	string sha11 = sha1(contents);
	string data = contents + sha11; contents.clear(); sha11.clear();
	return data;
}
string http_request1(string url, string username, string password, string data)
{
	Py_Initialize();
	if (!Py_IsInitialized())
	{
		printf("初始化失败！");
		exit(0);
	}
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('C:/Users/star/Desktop/test/')");//这一步很重要，修改Python路径
	PyObject* pModule = NULL;//声明变量
	PyObject* pFunc1 = NULL;// 声明变量
	if (_access("C:/Users/star/Desktop/test/requestbat.py", 0) == -1) {
		cout << "Bat don't exist";
	}
	pModule = PyImport_ImportModule("requestbat");//这里是要调用的文件名hello.py
	if (pModule == NULL)
	{
		cout << "没找到文件http1" << endl;
		exit(0);
	}
	pFunc1 = PyObject_GetAttrString(pModule, "http_request1");//这里是要调用的函数名
	if (pFunc1 == NULL) {
		cout << "没找到函数http1" << endl;
		exit(0);
	}
	//printf("data:");
	//for (int i = 0; i < data.length(); i++) {
		//printf("%c", data[i]);
	//}
	FILE* fp1 = fopen("C:/Users/star/Desktop/test/data", "wb");
	char datac[10000];
	for (int i = 0; i < data.length(); i++) {
		datac[i] = data[i];
	}
	fwrite(datac, 1, data.length(), fp1); fclose(fp1);
	PyObject* args2 = Py_BuildValue("sss", url.c_str(), username.c_str(), password.c_str());//给python函数参数赋值
	PyObject* pRet = PyObject_CallObject(pFunc1, args2);//调用函数
	//printf("OK"); exit(0);
	if (!pRet) {
		printf("Python error1");
		exit(0);
	}
	int ans = 0;
	PyArg_Parse(pRet, "i", &ans);//转换返回类型
	if (ans == 0) {
		exit(0);
	}
	char res[1000];
	FILE* fp = fopen("C:/Users/star/Desktop/test/test", "r");
	int num = fread(res, 1, 1000, fp); fclose(fp);
	Py_Finalize(); // 与初始化对应
	return string(res, res + num);
}
void push(string git_url, string username, string password)
{
	Repository* repo = repo_find();
	string remote_sha1 = get_remote_master_hash(git_url, username, password);
	string local_sha1 = string(get_local_master_hash(*repo));
	set<string> missing = find_missing_objects(*repo, local_sha1, remote_sha1);
	printf("%d", missing.size());
	if (remote_sha1.length() == 0) {
		remote_sha1.append(40, '0');
	}
	string lines = remote_sha1 + string(" ") + local_sha1 + string(" ");
	lines += string("refs/heads/master");
	lines.append(1, '\x00');
	lines += string(" report-status");
	/*for (int i = 0; i < lines.length(); i++) {
		printf("%c", lines[i]);
	}*/
	//exit(0);
	transLines liness;
	liness.num = 1;
	liness.len[0] = lines.length();
	liness.lines[0] = lines;
	string data = build_lines_data(&liness) + create_pack(*repo, missing);
	string url = git_url + string("/git-receive-pack");
	//printf("http brgin");
	string response = http_request1(url, username, password, data);
	//printf("http1 successfully\n");
	transLines* ll = extract_lines(response);
	printf("ll successfully");
	if (ll->num < 2) {
		printf("expected at least 2 lines, got %d\n", ll->num);
		exit(0);
	}
	if (ll->lines[0] != string("unpack ok\n")) {
		printf("expected line 1 unpack ok, got: %s", ll->lines[0].c_str());
		exit(0);
	}
	if (ll->lines[1] != string("ok refs/heads/master\n")) {
		printf("expected line 2 ok refs/heads/master\n, got: %s", ll->lines[1].c_str());
		exit(0);
	}
	printf("Push successfully");
}