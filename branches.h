#pragma once
char* get_local_master_hash(Repository & repo);
char* get_local_master(Repository& repo);
char* get_local_hash(Repository& repo, std::string branch);