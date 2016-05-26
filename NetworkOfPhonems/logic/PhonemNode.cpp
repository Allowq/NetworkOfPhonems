#include "PhonemNode.h"

uint32_t PHONEM_NODE_STRUCT::phonem_id = 0;

PHONEM_NODE_STRUCT::PHONEM_NODE_STRUCT()
	: phonem_value(""), type(PHONEM_TYPE::NODE_TYPE_BEGIN)
{
}

PHONEM_NODE_STRUCT::PHONEM_NODE_STRUCT(const PHONEM_TYPE &_type, const std::string &_phonem_value)
	: phonem_value(_phonem_value), type(_type)
{
}

void PHONEM_NODE_STRUCT::add_phonem_sequence(std::vector<std::string>::iterator &it, const std::vector<std::string>::iterator &end) {
	std::shared_ptr<PHONEM_NODE_STRUCT> tail;
	if (it != end)
	{
		this->childrens.push_back(std::make_shared<PHONEM_NODE_STRUCT>(PHONEM_NODE_STRUCT(PHONEM_TYPE::NODE_TYPE_PHONE, *it)));
		tail = this->childrens.back();
		++it;
	}

	while (it != end)
	{
		tail->childrens.push_back(std::make_shared<PHONEM_NODE_STRUCT>(PHONEM_NODE_STRUCT(PHONEM_TYPE::NODE_TYPE_PHONE, *it)));
		tail = tail->childrens.back();
		++it;
	}
	if (tail != nullptr)
	{
		tail->childrens.push_back(std::make_shared<PHONEM_NODE_STRUCT>(PHONEM_NODE_STRUCT(PHONEM_TYPE::NODE_TYPE_WORD, std::to_string(phonem_id++))));
	} else 
		this->childrens.push_back(std::make_shared<PHONEM_NODE_STRUCT>(PHONEM_NODE_STRUCT(PHONEM_TYPE::NODE_TYPE_WORD, std::to_string(phonem_id++))));
}

void PHONEM_NODE_STRUCT::add_phonem_sequence(const std::vector<std::string> &sequence, PHONEM_NODE_STRUCT &head_node) {
	std::vector<std::string>::const_iterator it = sequence.begin();
	std::shared_ptr<PHONEM_NODE_STRUCT> tail;
	if (it != sequence.end())
	{
		head_node.childrens.push_back(std::make_shared<PHONEM_NODE_STRUCT>(PHONEM_NODE_STRUCT(PHONEM_TYPE::NODE_TYPE_PHONE, *it)));
		tail = head_node.childrens.back();
		++it;
	}

	while (it != sequence.end())
	{
		tail->childrens.push_back(std::make_shared<PHONEM_NODE_STRUCT>(PHONEM_NODE_STRUCT(PHONEM_TYPE::NODE_TYPE_PHONE, *it)));
		tail = tail->childrens.back();
		++it;
	}
	tail->childrens.push_back(std::make_shared<PHONEM_NODE_STRUCT>(PHONEM_NODE_STRUCT(PHONEM_TYPE::NODE_TYPE_WORD, std::to_string(phonem_id++))));
}

void PHONEM_NODE_STRUCT::get_tail_for_phonems(std::vector<std::string>::iterator &start, 
	const std::vector<std::string>::iterator &end, 
	std::shared_ptr<PHONEM_NODE_STRUCT> &local_tail) 
{
	// пробегаем по всем детям этой ноды
	std::vector<std::shared_ptr<PHONEM_NODE_STRUCT>>::iterator it = childrens.begin();
	while (it != childrens.end()) {
		// если у этой ноды есть ребёнок, фонема которого совпадает с той, что нам требуется
		if (start != end && it->get()->type == PHONEM_TYPE::NODE_TYPE_PHONE && it->get()->phonem_value == *start) {
			// если для данной фонемы есть хвост в последовательности фонем поиска
			// передвигаемся к следующей фонеме в последовательности
			++start;
			local_tail = *it;
			it->get()->get_tail_for_phonems(start, end, local_tail);
			break;
		}
		++it;
	}
//	if (start == end)
//		return false;
}

void PHONEM_NODE_STRUCT::get_id_for_phonems(std::vector<std::string>::iterator &start, 
	const std::vector<std::string>::iterator &end,
	std::shared_ptr<PHONEM_NODE_STRUCT> &local_tail,
	int32_t &id_value)
{
	// пробегаем по всем детям этой ноды
	std::vector<std::shared_ptr<PHONEM_NODE_STRUCT>>::const_iterator it = childrens.begin();
	while (it != childrens.end()) {
		// если у этой ноды есть ребёнок, фонема которого совпадает с той, что нам требуется
		if (start == end)
		{
			it = childrens.begin();
			while (it != childrens.end()) {
				if (it->get()->type == PHONEM_TYPE::NODE_TYPE_WORD) {
					id_value = std::atoi(it->get()->phonem_value.c_str());
					return;
				}
				++it;
			}
			return;
		}
		else if (it->get()->type == PHONEM_TYPE::NODE_TYPE_PHONE && it->get()->phonem_value == *start) {
			++start;
			local_tail = *it;
			it->get()->get_id_for_phonems(start, end, local_tail, id_value);
			break;
		}
		++it;
	}
}

PHONEM_NODE_STRUCT::~PHONEM_NODE_STRUCT() {
	std::vector<std::shared_ptr<PHONEM_NODE_STRUCT>>::iterator it = childrens.begin();
	while (it != childrens.end()) {
		it->reset();
		++it;
	}
}