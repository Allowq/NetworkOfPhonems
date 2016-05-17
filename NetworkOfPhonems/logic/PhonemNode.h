#ifndef _PHONEMNODE_H_
#define _PHONEMNODE_H_

#pragma once

#include <stdint.h>

#include <string>
#include <vector>

enum PHONEM_TYPE {
	NODE_TYPE_BEGIN = 0, NODE_TYPE_PHONE, NODE_TYPE_WORD
};

struct PHONEM_NODE_STRUCT {

private:
	PHONEM_TYPE type;
	std::vector<std::shared_ptr<PHONEM_NODE_STRUCT *>> childrens;

public:
	const std::string phonem_value;

public:
	PHONEM_NODE_STRUCT() 
		: phonem_value(""), type(PHONEM_TYPE::NODE_TYPE_BEGIN)
	{
	}

	PHONEM_NODE_STRUCT(const PHONEM_TYPE &_type, const std::string &_phonem_value) 
		: phonem_value(_phonem_value), type(_type)
	{
	}

	std::shared_ptr<PHONEM_NODE_STRUCT *> add_children(PHONEM_NODE_STRUCT *ptr) {
		childrens.push_back(std::make_shared<PHONEM_NODE_STRUCT *>(ptr));
		return childrens.back();
	}

	bool is_head() {
		return this->childrens.empty();
	}

	~PHONEM_NODE_STRUCT() {
		std::vector<std::shared_ptr<PHONEM_NODE_STRUCT *>>::iterator it = childrens.begin();
		while (it != childrens.end()) {
			it->reset();
		}
	}
};

#endif
