#pragma once
struct CommitEntry {
	std::string name[100];
	std::string para[100];
	int fanum;
	int rownum;
};
CommitEntry* read_commit(Repository& repo, std::string sha);
