#include "definitions.h"
extern Actions gitActions[MAXACTIONNUM];
int main(int argc, char* argv[])
{
    prep();
    Ans ans = parse(argc, argv);
    if (ans.actionInd < 0) printf("Action error");
    else if (ans.optNum < 0) printf("Option error");
    else {
        int retc;
        char retv[100][100];
        //printf("%s", gitActions[ans.actionInd].name);
        //printf("%d %d %d %d", ans.optInd[0], ans.optNum, ans.fstPara, ans.lstPara);
        gitActions[ans.actionInd].func(retc, retv, argc, argv, ans);
    }
}