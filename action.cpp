#include "definitions.h"
Actions gitActions[MAXACTIONNUM];
int actNum = 0;

int findAction(const char* act) {
    for (int i = 0; i < actNum; i++) {
        if (!strcmp(act, gitActions[i].name)) return i;
    }
    return -1;
}
int addAction(const char* act, const char* desc, void (* func)(int&, char[][100], int, char* [], Ans)) {
    if (findAction(act) >= 0) return -1;
    strcpy(gitActions[actNum].name, act);
    strcpy(gitActions[actNum].desc, act);
    gitActions[actNum].optNum = 0;
    gitActions[actNum].func = func;
    gitActions[actNum].parameter = -1;
    actNum++;
    return 1;
}

//find option
int findLongopt(int ind, const char* opt) {
    //printf("%s", opt);
    for (int i = 0; i < gitActions[ind].optNum; i++) {
        if (!strcmp(opt, gitActions[ind].opt[i].longOpt)) return i;
    }
    return -1;
}
int findShortopt(int ind, char opt) {
    for (int i = 0; i < gitActions[ind].optNum; i++) {
        if (opt == gitActions[ind].opt[i].shortOpt) return i;
    }
    return -1;
}
int addOption(const char* Act, char shortOpt, const char* longOpt, int para, const char* desc) {
    int ind = findAction(Act);
    if (ind < 0) return -1;
    int ind_ = -1;
    if (shortOpt != 0)
        ind_ = findShortopt(ind, shortOpt);
    else if (longOpt == NULL)
    {
        if (gitActions[ind].parameter != -1) ind_ = gitActions[ind].parameter;
        else gitActions[ind].parameter = gitActions[ind].optNum;
    }
    else
        ind_ = findLongopt(ind, longOpt);
    if (ind_ >= 0) return -1;
    if (longOpt)
        strcpy(gitActions[ind].opt[gitActions[ind].optNum].longOpt, longOpt);
    else gitActions[ind].opt[gitActions[ind].optNum].longOpt[0] = '\0';
    gitActions[ind].opt[gitActions[ind].optNum].shortOpt = shortOpt;
    strcpy(gitActions[ind].opt[gitActions[ind].optNum].desc, desc);
    gitActions[ind].opt[gitActions[ind].optNum++].parameter = para;
    return 1;
}
//parse the argument
Ans parse(int argc, char* argv[]) {
    Ans ans;
    if (argc == 1 || argc == 0) {
        ans.actionInd = -1;
        return ans;
    }
    ans.actionInd = findAction(argv[1]);
    if (ans.actionInd < 0) return ans;
    ans.optNum = 0;
    int _i = ans.actionInd;
    for (int i = 2; i < argc; i++) {
        if (argv[i][0] != '-' || argv[i][1] == '-' || strlen(argv[i]) == 2) {
            int ind;
            if(argv[i][0] != '-')
                if (gitActions[_i].parameter == -1) {
                    ans.optNum = -1;
                    return ans;
                }
                else {
                    ind = ans.optInd[ans.optNum] = gitActions[_i].parameter;
                    i--;
                }
            else if (argv[i][1] == '-') ind = findLongopt(_i, argv[i] + 2);
            else ind = findShortopt(_i, argv[i][1]);
            //printf("%d\n", ind);
            ans.optInd[ans.optNum] = ind;
            if (ind < 0) {
                ans.optNum = -1;
                return ans;
            }
            if (gitActions[_i].opt[ind].parameter == 0) {
                ans.fstPara[ans.optNum] = i;
                ans.lstPara[ans.optNum] = i - 1;
                ++ans.optNum;
                continue;
            }
            else if (gitActions[_i].opt[ind].parameter > 0) {
                ++i;
                if (i >= argc || strlen(argv[i]) != gitActions[_i].opt[ind].parameter || argv[i][0] == '-') {
                    ans.optNum = -1;
                    return ans;
                }
                else {
                    ans.fstPara[ans.optNum] = ans.lstPara[ans.optNum] = i;
                    ++ans.optNum;
                    continue;
                }
            }
            else {
                i++;
                if (argv[i][0] == '-') {
                    ans.optNum = -1;
                    return ans;
                }
                ans.fstPara[ans.optNum] = i;
                while (i + 1 < argc && argv[i + 1][0] != '-') i++;
                ans.lstPara[ans.optNum] = i;
                if (ans.lstPara[ans.optNum] - ans.fstPara[ans.optNum] + 1 != -gitActions[_i].opt[ind].parameter) {
                    ans.optNum = -1;
                    return ans;
                }
            }
            ans.optNum++;
        }
        else {
            int len = strlen(argv[i]), ind;
            for (int j = 1; j < len; j++) {
                ind = findShortopt(_i, argv[i][j]);
                if (ind < 0) {
                    ans.optNum = -1;
                    return ans;
                }
                ans.fstPara[ans.optNum] = i;
                ans.lstPara[ans.optNum] = i - 1;
                ans.optInd[ans.optNum++] = ind;
            }
        }
    }
    return ans;
}
//initiate actions and options
int prep() {
    addAction("differ", "Show the differences between two files", _differ);
    addOption("differ", '\0', "git", -2, "Compare two file");
    addAction("init", "Initiating a new, empty repository", _init);
    addOption("init", 'd', "default", 0, "Create repository in the current direactory.");
    addOption("init", '\0', NULL, -1, "Where to create the repository.");
    addAction("test", "debug", _test);
    addOption("test", '\0', NULL, -1, "debug");
    addAction("add" ,"Add file into index", _add);
    addOption("add", '\0', NULL, -1, "add");
    addAction("commit", "Commit files in index to the repository.", _commit);
    addOption("commit", 'a', "all", 0, "commit all");
    addAction("rm",  "Remove the file", _rm);
    addOption("rm", '\0', NULL, -1, "Remove the file from the index and the workplace");
    addOption("rm", '\0', "cached", -1, "Remove the file only from the index");
    addAction("mv", "Move file", _mv);
    addOption("mv", '\0', NULL, -2, "Move file");
    addOption("mv", 'f',"force", -2, "Move file unconditioally");
    addAction("status", "Show status", _status);
    addOption("status", 's', "short", 0, "Show status");
    addAction("branch", "Add branch", _branch);
    addOption("branch", '\0', NULL, -1, "Add branch");
    addAction("log", "Login the history", _log);
    addOption("log", '\0', NULL, 0, "Login the history");
    addOption("log", 's', NULL, 0, "Login the history");
    addAction("checkout", "Change branch", _checkout);
    addOption("checkout", '\0', NULL, -1, "Change branch");
    addOption("checkout", 'b', "branch", -1, "Create branch and change");
    addAction("reset", "Reset the head", _reset);
    addOption("reset", '\0', NULL, -1, "Reset the head");
    addAction("merge", "Merge to master", _merge);
    addOption("merge", '\0', NULL, -1, "Merge to master");
    addAction("remote", "Add remote repository", _remote);
    addOption("remote", 'a', "add", -2, "Add remote repository");
    addAction("push", "Push to remote", _push);
    addOption("push", '\0', NULL, -2, "Push to remote");
    addAction("fetch", "fetch from remote", _fetch);
    addOption("fetch", '\0', NULL, -2, "fetch from remote");
    addAction("pull", "pull from remote", _pull);
    addOption("pull", '\0', NULL, -2, "pull from remote");
    addAction("diff", "Show the differences between index and files", _diff);
    addOption("diff", '\0', NULL, 0, "Compare index and files");
    addAction("tag", "Create tags", _tag);
    addOption("tag", '\0', NULL, -2, "Create tags");
    addOption("tag", '\0', "show", -1, "Checkout tags");
    addOption("tag", 'm', "message", -1, "Messages");
    addAction("config", "config management", _config);
    addOption("config", '\0', "local", 0, "domain");
    addOption("config", '\0', "list", 0, "List config");
    addOption("config", '\0', NULL, -2, "Add config");
    addAction("rev-parse", "Read short hashes", _rev_parse);
    addOption("rec-parse", '\0', NULL, -1, "Read short hashes");
    addAction("cat-file", "Show file", _cat_file);
    addOption("cat-file", '\0', NULL, -1, "Show file");
    addAction("list-refs", "Show refs", _list_refs);
    addOption("list-refs", '\0', NULL, 0, "Show refs");
    addAction("stash", "Restore the index", _stash);
    addOption("stash", '\0', "save", -1,"Save stash");
    addOption("stash", '\0', "list", 0, "List stash");
    addOption("stash", '\0', "pop", -1, "Recover stash");
    return 1;
}