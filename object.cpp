#include "definitions.h"
Object::Object(Repository* _repo, const char * name, const char* _data = NULL)
{
	repo = _repo;
	strcpy(type, name);
    data = NULL;
	if (_data != NULL) {
		deserialize(_data);
	}
}
Bytef file_data[10000];
//meaningless functions...
const char * Object::serialize()
{
	return data;
}
void Object::deserialize(const char* _data)
{
    data = _data;
}
//read object in objects
Object* object_read(Repository& repo, char sha[100], int num)
{
    Bytef * raw_data = new Bytef[20000];
    char path[100];
	char temp[1][100];
	strcpy(temp[0], "\\objects\\");
	temp[0][9] = sha[0]; temp[0][10] = sha[1];
	temp[0][11] = '\\';
	int i = 12, j = 2;
	while (sha[j]) {
		temp[0][i++] = sha[j++];
	}
	temp[0][i] = '\0';
	strcpy(path, repo_file(repo, temp, 1, 0));
	FILE* fp = fopen(path, "rb");
	int _l = fread(file_data, 1, 10000, fp);
	fclose(fp);
    
	uLongf len = 10000;
	uncompress(raw_data, &len, file_data,(uLong) _l);
    raw_data[len] = '$';
    //printf("read :%s %d %d\n", sha,(int)len, _l);
    //for (int i = 0; i < (int)len; i++)printf("%c", (char)raw_data[i]);
	int x = 0, y = 0;
	for (int i = 0; i < (int)len; i++) {
		if ((int)raw_data[i] - (int)' ' == 0) x = i;
		if ((int)raw_data[i] - (int)'\x00' == 0){
		    y = i;
		    break;
		}
	}
    int lenn = 0;
    for (int ii = x + 1; ii < y; ii++) {
        lenn = lenn * 10 + raw_data[ii] - '0';
    }
	if (lenn != len - y - 1) {
		printf("Malformed object %s: bad length\n", sha);
        //printf("%d %d %d %d", lenn, len - y - 1, x, y);
        exit(0);
	}
	char name[20];
	for (int i = 0; i < x; i++) {
		name[i] = raw_data[i];
	}
	name[x] = '\0';
	if (strcmp(name, "commit") && strcmp(name, "tree")
		&& strcmp(name, "tag") && strcmp(name, "blob")) {
		printf("Unknown type %s for object %s\n", name, sha);
		exit(0);
	}
	Object* c = new Object(&repo, name, (char *)(raw_data + y + 1));
	return c;
}
//basic functions for sha1
unsigned circleShift(const unsigned& word, const int& bits) {
    return (word << bits) | ((word) >> (32 - bits));
}

unsigned sha1Fun(const unsigned& B, const unsigned& C, const unsigned& D, const unsigned& t) {

    switch (t / 20) {
    case 0:     return (B & C) | ((~B) & D);
    case 2:     return (B & C) | (B & D) | (C & D);
    case 1:
    case 3:     return B ^ C ^ D;
    }

    return t;
}
//calculate the sha1
string sha1(const string strRaw) {

    string str(strRaw);
    str += (unsigned char)(0x80);
    // 每个字节8位,所以要乘8,左移3位
    //printf("size:%d", str.size());
    //cout << str << endl;
    while ((str.size() << 3) % 512 != 448) {
        
        str += (char)0;
    }
    //printf("sha1  1\n");
    // 写入原始数据长度
    for (int i(56); i > 0; i -= 8) {
       // printf("\n%d\n", str.size());
        str += (char)0;
    }
    str += (unsigned char)(strRaw.size() << 3);
    const unsigned K[4] = { 0x5a827999,0x6ed9eba1,0x8f1bbcdc,0xca62c1d6 };
    unsigned A(0x67452301), B(0xefcdab89), C(0x98badcfe), D(0x10325476), E(0xc3d2e1f0), T(0);
    unsigned W[80] = { 0 };

    // 每次处理64字节,共512位
    for (unsigned i(0); i != str.size(); i += 64) {
        // 前16个字为原始数据
        for (unsigned t(0); t != 16; ++t) {
            // 将4个8位数据放入一个32位变量中
            W[t] = ((unsigned)str[i + 4 * t] & 0xff) << 24 |
                ((unsigned)str[i + 4 * t + 1] & 0xff) << 16 |
                ((unsigned)str[i + 4 * t + 2] & 0xff) << 8 |
                ((unsigned)str[i + 4 * t + 3] & 0xff);
        }

        // 填充
        for (unsigned t(16); t != 80; ++t) {
            W[t] = circleShift(W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16], 1);
        }

        for (unsigned t(0); t != 80; ++t) {
            T = circleShift(A, 5) + sha1Fun(B, C, D, t) + E + W[t] + K[t / 20];
            E = D;
            D = C;
            C = circleShift(B, 30);
            B = A;
            A = T;
        }

        A += 0x67452301;
        B += 0xefcdab89;
        C += 0x98badcfe;
        D += 0x10325476;
        E += 0xc3d2e1f0;
    }
    stringstream ss;
    ss << setw(8) << setfill('0') << hex << A << B << C << D << E;
    ss >> str;
    return str;
}
char result[20000];
/*void hexdigest(char* result, int len)
{
    char temp[100];
    int high, low;
    for (int i = 0; i < len; i++) {
        high = result[i] / 16;
        if (high >= 0 && high <= 9) temp[2 * i] = high + '0';
        else temp[2 * i] = high + 'a' - 10;
        low = result[i] % 16;
        if (low >= 0 && low <= 9) temp[2 * i + 1] = low + '0';
        else temp[2 * i + 1] = low + 'a' - 10;
    }
    for (int i = 0; i < 2 * len - 1; i++) result[i] = temp[i];
    result[2 * len] = '\0';
}*/
//write the object and return the sha1
char* object_write(Object* obj, int actually_write = true)
{
    char* sha = new char[50];
    const char* data = obj->serialize();
    int i = 0;
    while (obj->type[i]) {
        result[i] = obj->type[i];
        i++;
    }
    result[i++] = ' ';
    int len = 0;
    while (obj->data[len] != '$') len++;
    _itoa(len, result + i, 10);
    i = strlen(result) + 1; len = 0;
    while (obj->data[len] != '$') {
        result[i++] = obj->data[len++];
    }
    string a = sha1(string(result, result + i));
    len =  a.length();
    strcpy(sha ,a.c_str());
    sha[40] = '\0';
    if (actually_write) {
        char temp[2][100];
        strcpy(temp[0], "\\objects\\");
        temp[0][9] = sha[0]; temp[0][10] = sha[1];
        temp[0][11] = '\0'; temp[1][0] = '\\';
        int _i = 1, j = 2;
        while (sha[j]) {
            temp[1][_i++] = sha[j++];
        }
        temp[1][_i] = '\0';
        strcpy(temp[0], repo_file(*(obj->repo), temp, 2, 1));
        FILE* fp = fopen(temp[0], "wb");
        j = 10000;
        compress(file_data, (uLongf*)&j, (Bytef*)result, i);
        //printf("write:%d %d", i, j);
        fwrite(file_data,1, j, fp );
        fclose(fp);
    }
    return sha;
}