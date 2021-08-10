#include "definitions.h"
void _add(int& retc, char retv[][100], int argc, char* argv[], Ans ans)
{
	if (ans.optNum == 0) {
		printf("error");
		exit(0);
	}
	Repository * repo = repo_find();
	write_index(*repo, argv[ans.fstPara[0]]);
	printf("Add successfully");
	retc = 0;
}