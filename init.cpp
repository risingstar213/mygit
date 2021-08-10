#include "definitions.h"
void _init(int& retc, char retv[][100], int argc, char* argv[], Ans ans)
{
	if (!ans.optNum || ans.optInd[0] == 0) {
		repo_create(".");
	}
	else repo_create(argv[ans.fstPara[0]]);
	printf("init successfully");
	retc = 0;
}