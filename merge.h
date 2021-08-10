#pragma once
treeEntries* comread_tree(Repository& repo, std::string sha1, int& num, std::string ancestor);
void find1(Repository& repo, std::string sha1, std::set <std::string>& objects);