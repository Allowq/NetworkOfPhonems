#ifndef _PHONEMSNET_H_
#define _PHONEMSNET_H_

#pragma once

#include <fstream>
#include <unordered_set>
#include <sstream> 
#include <spdlog\spdlog.h>

#include "PhonemNode.h"

#define LOCALE_DICTIONARY

class PhonemsNet
{
private:
	std::shared_ptr<spdlog::logger> console;

	std::ifstream dict_file;

	bool is_interrupt;
	std::string path_to_dict;

	PHONEM_NODE_STRUCT head_of_net;
	static uint32_t word_count;
	std::unordered_set<std::string> phonem_table;

	bool init_phonem_table();
	void parse_and_add(const std::string &phonems_line);

public:
	explicit PhonemsNet();

	void create_network();
	bool dict_is_set();
	void generate_dictionary(const std::string &file_name) const;
	int32_t get_id_by_set(const std::string &phonems);
	void interrupt();
	bool is_interrupted() const;
	void set_dictionary_path(const std::string &path);

	virtual ~PhonemsNet();
};

#endif