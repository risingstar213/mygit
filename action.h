#pragma once
#define MAXNAMELENGTH 128
#define MAXACTIONNUM 128
#define MAXOPTNUM 128
#define MAXDESCLENGTH 1024

struct Optflags {
    char shortOpt;
    char longOpt[MAXNAMELENGTH];
    char desc[MAXDESCLENGTH];
    int parameter;
};
struct Ans {
    int actionInd;
    int optNum;
    int optInd[MAXACTIONNUM];
    int fstPara[MAXACTIONNUM];
    int lstPara[MAXACTIONNUM];
};
struct Actions {
    char name[MAXNAMELENGTH];
    char desc[MAXDESCLENGTH];
    int optNum;
    int parameter;
    Optflags opt[MAXOPTNUM];
    void (*func)(int& retc, char retv[][100], int argc, char* argv[], Ans ans);
};


int addAction(const char*, const char*, void (*)(int&, char[][100],int , char * [], Ans));
int addOption(const char*, char, const char*, int, const char*);
int findAction(const char*);
int findShortopt(int ind, char opt);
int findLongopt(int ind, const char* opt);
Ans parse(int, char**);
int prep();
void differ(const char*, const char*);
void _differ(int&, char[][100],int, char*[], Ans);
void _init(int&, char[][100], int, char* [], Ans);
void _test(int&, char[][100], int, char* [], Ans);
void _add(int&, char[][100], int, char* [], Ans);
void _commit(int&, char[][100], int, char* [], Ans);
void _rm(int&, char[][100], int, char* [], Ans);
void _mv(int&, char[][100], int, char* [], Ans);
void _status(int&, char[][100], int, char* [], Ans);
void _branch(int&, char[][100], int, char* [], Ans);
void _log(int&, char[][100], int, char* [], Ans);
void _checkout(int&, char[][100], int, char* [], Ans);
void _reset(int&, char[][100], int, char* [], Ans);
void _merge(int&, char[][100], int, char* [], Ans);
std::string merge(Repository& repo, std::string branch1, std::string branch2);
void transport(Repository& local, Repository& remote, std::string sha1);
void _remote(int&, char[][100], int, char* [], Ans);
void _push(int&, char[][100], int, char* [], Ans);
void _pull(int&, char[][100], int, char* [], Ans);
void _fetch(int&, char[][100], int, char* [], Ans);
void _diff(int&, char[][100], int, char* [], Ans);
void _tag(int&, char[][100], int, char* [], Ans);
void _config(int&, char[][100], int, char* [], Ans);
int cat_file(Repository& repo, std::string sha1);
void _cat_file(int&, char[][100], int, char* [], Ans);
void _rev_parse(int&, char[][100], int, char* [], Ans);
void _list_refs(int&, char[][100], int, char* [], Ans);
void _stash(int&, char[][100], int, char* [], Ans);