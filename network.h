#pragma once
struct transLines {
	std::string lines[100];
	int len[100] = { 0 };
	int num = 0;
};
struct treeEntries {
	unsigned long mode = 0;
	std::string path = std::string("\0");
	std::string sha1 = std::string("\0");
};
treeEntries* read_tree(Repository& repo, std::string sha1, int& num);
void push(std::string git_url, std::string username, std::string password);
std::set <std::string> find_missing_objects(Repository& repo, std::string local_sha1, std::string remote_sha1);
std::set <std::string> find_commit_object(Repository& repo, std::string local_sha1);