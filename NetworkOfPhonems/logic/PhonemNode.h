#ifndef _PHONEMNODE_H_
#define _PHONEMNODE_H_

#pragma once

#include <stdint.h>

#include <string>
#include <vector>
#include <memory>

enum PHONEM_TYPE {
	NODE_TYPE_BEGIN = 0, NODE_TYPE_PHONE, NODE_TYPE_WORD
};

class PHONEM_NODE_STRUCT {

private:
	static uint32_t phonem_id;

	const std::string phonem_value;
	const PHONEM_TYPE type;
	std::vector<std::shared_ptr<PHONEM_NODE_STRUCT>> childrens;

public:
	PHONEM_NODE_STRUCT();
	PHONEM_NODE_STRUCT(const PHONEM_TYPE &_type, const std::string &_phonem_value);

	std::shared_ptr<PHONEM_NODE_STRUCT> add_children(PHONEM_NODE_STRUCT &ptr);
	void add_phonem_sequence(std::vector<std::string>::iterator &it, const std::vector<std::string>::iterator &end);
	void add_phonem_sequence(const std::vector<std::string> &sequence, PHONEM_NODE_STRUCT &head_node);
	bool get_tail_for_phonems(std::vector<std::string>::iterator &start, 
		const std::vector<std::string>::iterator &end, 
		std::shared_ptr<PHONEM_NODE_STRUCT> &local_tail);

	~PHONEM_NODE_STRUCT();
};

#endif
