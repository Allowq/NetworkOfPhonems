#ifndef _PHONEMNODE_H_
#define _PHONEMNODE_H_

#pragma once

#include <stdint.h>

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>

enum PHONEM_TYPE {
	NODE_TYPE_BEGIN = 0, NODE_TYPE_PHONE, NODE_TYPE_WORD
};

//#define VERBOSE_MODE

class PHONEM_NODE_STRUCT {

private:
	static uint32_t phonem_id;

	const std::string phonem_value;
	const PHONEM_TYPE type;
	std::vector<std::shared_ptr<PHONEM_NODE_STRUCT>> childrens;

	bool PHONEM_NODE_STRUCT::tree_traverssal(std::ofstream &file_out,
		std::vector<std::string> &phonems_line);
	void write_line_to_dictionary(std::ofstream &file_out, std::vector<std::string> &phonems_line) const;

public:
	PHONEM_NODE_STRUCT();
	PHONEM_NODE_STRUCT(const PHONEM_TYPE &_type, const std::string &_phonem_value);

	void add_phonem_sequence(std::vector<std::string>::iterator &it, const std::vector<std::string>::iterator &end);
	void add_phonem_sequence(const std::vector<std::string> &sequence, PHONEM_NODE_STRUCT &head_node);
	bool dictionary_to_file(std::ofstream &file_out);
	void get_id_for_phonems(std::vector<std::string>::iterator &it, 
		const std::vector<std::string>::iterator &end, 
		std::shared_ptr<PHONEM_NODE_STRUCT> &local_tail,
		int32_t &id_value);
	uint32_t get_last_id() const;
	void get_tail_for_phonems(std::vector<std::string>::iterator &start, 
		const std::vector<std::string>::iterator &end, 
		std::shared_ptr<PHONEM_NODE_STRUCT> &local_tail);

	~PHONEM_NODE_STRUCT();
};

#endif
