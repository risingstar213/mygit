#include "definitions.h"
//judge if it is a folder
BOOL CheckFolderExist(const char * strPath)
{
    WIN32_FIND_DATA  FindFileData;
    BOOL bValue = false;
    HANDLE hFind = FindFirstFile(strPath, &FindFileData);
    if ((hFind != INVALID_HANDLE_VALUE) && (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        bValue = TRUE;
    }
    FindClose(hFind);
    return bValue;
}
char _temp[100];
//get full path
char* repo_path(Repository& repo, char path[][100], int num)
{
    memset(_temp, 0, sizeof(int));
    strcpy(_temp, repo.gitdir);
    //printf("%d", num);
    for (int i = 0; i < num; i++) {
        strcat(_temp, path[i]);
    }
    return _temp;
}
//judge whether the path is correct
char* repo_dir(Repository& repo, char path[][100],int num, int mkdir = false)
{
    char __t[100];
    strcpy(__t, repo_path(repo, path, num));
    if (_access(__t, 0) != -1) {
        if (CheckFolderExist(__t)) {
            memset(_temp, 0, sizeof(int));
            return strcpy(_temp, __t);
        }
        else {
            printf("Not a directory %s\n", __t);
            exit(0);
        }
    }
    if (mkdir) {
        bool flag = CreateDirectory(__t, NULL);
        if (!flag) {
            printf("Fail to create directory.\n");
            printf("%s\n",__t);
            exit(0);
        }
        memset(_temp, 0, sizeof(int));
        return strcpy(_temp, __t);
    }
    else return NULL;
}
//judge whether the file or folder can be created
char* repo_file(Repository& repo, char path[][100], int num, int mkdir = false)
{
    if (repo_dir(repo, path, num - 1, mkdir)) {
        return repo_path(repo, path, num);
    }
    return NULL;
}
//init the struct repository 
Repository::Repository(const char* path, int force = false)
{
	strcpy(workplace, path);
	strcat(strcpy(gitdir, path), "\\.git");
    if (!force && !CheckFolderExist(gitdir)) {
        printf("Not a Git repository %s\n", workplace);
        exit(0);
    }
    char _t[1][100] = { "\\config" };
   // printf("repo_file start\n");
    char* cf = repo_file(*this, _t, 1);
   // printf("repo_file end %s\n", cf);
    if (cf != NULL && _access(cf, 0) != -1) {
        conf.readConfig(cf);
    }
    else if (!force) {
        printf("Configuration file missing\n");
        exit(0);
    }
    if (!force) {
        char* cc;
        //printf("start get\n");
        if((cc = conf.get("core", "repositoryformatversion")) == NULL){
            printf("error");
            exit(0);
        }
        //printf("get end\n");
        if (atoi(cc) != 0) {
            printf("unsupported repositoryformatversion %d\n", atoi(cc));
        }
        //printf("init end\n");
    }
}
//check whether folder is empty
int CheckDirEmpty(char * path)
{
    HANDLE hFind;
    WIN32_FIND_DATA FindFileData;
    char _tt[100];
    strcat(strcpy(_tt, path), "\\*.*");
    if ((hFind = FindFirstFileA(_tt, &FindFileData)) != INVALID_HANDLE_VALUE)
    {
        BOOL bFind = TRUE;
        BOOL EmptyDirectory = TRUE;
        while (bFind)
        {
            if (strcmp(FindFileData.cFileName, ".") == 0
                || strcmp(FindFileData.cFileName, "..") == 0)
            {
                bFind = FindNextFile(hFind, &FindFileData);
            }
            else
            {
                EmptyDirectory = FALSE;
                break;
            }
        }
        if (EmptyDirectory)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return -1;
    }
}
//create new repository
Repository * repo_create(const char * path)
{
    Repository * repo = new Repository(path, true);
    if (_access(repo->workplace, 0) != -1) {
        int __i;
        if (!CheckFolderExist(repo->workplace)) {
            printf("%s is not s directory\n", repo->workplace);
            exit(0);
        }
        if (__i = CheckDirEmpty(repo->workplace)) {
            if (__i == 1)printf("%s is not empty\n", repo->workplace);
            else printf("error1");
            exit(0);
        }
    }
    else {
        CreateDirectory(repo->workplace, NULL);
    }
    CreateDirectory(repo->gitdir, NULL);
    char str[2][100];
    strcpy(str[0], "\\branches");
    if (repo_dir(*repo, str, 1, 1) == NULL)exit(0);
    strcpy(str[0], "\\objects");
    if (repo_dir(*repo, str, 1, 1) == NULL)exit(0);
    //strcpy(str[0], "\\index");
    //if (repo_dir(*repo, str, 1, 1) == NULL)exit(0);
    strcpy(str[0], "\\refs");
    if (repo_dir(*repo, str, 1, 1) == NULL)exit(0);
    strcpy(str[0], "\\refs"); strcpy(str[1], "\\tags");
    if (repo_dir(*repo, str, 2, 1) == NULL)exit(0);
    strcpy(str[0], "\\refs"); strcpy(str[1], "\\heads");
    if (repo_dir(*repo, str, 2, 1) == NULL)exit(0);
    strcpy(str[0], "\\refs"); strcpy(str[1], "\\remotes");
    if (repo_dir(*repo, str, 2, 1) == NULL)exit(0);
    strcpy(str[0], "\\description");
    FILE* fp = fopen(repo_file(*repo, str, 1), "w");
    fprintf(fp, "Unnamed repository; edit this file 'description' to name the repository.\n");
    fclose(fp);
    strcpy(str[0], "\\HEAD");
    fp = fopen(repo_file(*repo, str, 1), "w");
    fprintf(fp, "ref: refs/heads/master\n");
    fclose(fp);
    strcpy(str[0], "\\config");
    fp = fopen(repo_file(*repo, str, 1), "w");
    repo->conf.default_config();
    repo->conf.writeConfig(fp);
    fclose(fp);
    return repo;
}
//find a correct respository
Repository * repo_find(const char* path , int required)
{
    char rpath[100];
    if (_fullpath(rpath, path, 100) == NULL) {
        printf("fullpath error");
        exit(0);
    }
    //printf("%s", rpath);
    //return NULL;
    memset(_temp, 0, sizeof(char));
    if (CheckFolderExist(strcat(strcpy(_temp, rpath), "\\.git") )){
        //printf("find path\n");
        memset(_temp, 0, sizeof(int));
        Repository* re = new Repository(strcpy(_temp, rpath));
        return re;
    }
    if (!strcmp(rpath, "C:\\") || !strcmp(rpath, "D:\\")) {
        if (required) {
            printf("No git directory.");
            exit(0);
        }
        else return NULL;
    }
    memset(_temp, 0, sizeof(char));
    return repo_find(strcat(strcpy(_temp, rpath), "\\.."), required);
}
//debug
void _test(int& retc, char retv[][100], int argc, char* argv[], Ans ans)
{
    
    
}