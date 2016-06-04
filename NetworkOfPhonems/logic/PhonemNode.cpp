#include "PhonemNode.h"

std::atomic<uint32_t> PHONEM_NODE_STRUCT::phonem_id(0);

PHONEM_NODE_STRUCT::PHONEM_NODE_STRUCT()
	: phonem_value(""), type(PHONEM_TYPE::NODE_TYPE_BEGIN)
{
}

PHONEM_NODE_STRUCT::PHONEM_NODE_STRUCT(const PHONEM_TYPE &_type, const std::string &_phonem_value)
	: phonem_value(_phonem_value), type(_type)
{
}

void PHONEM_NODE_STRUCT::add_phonem_sequence(std::vector<std::string>::iterator &it, const std::vector<std::string>::iterator &end) {
	std::shared_ptr<PHONEM_NODE_STRUCT> tail = nullptr;

	// добавляю фонему в список детей ноды
	if (it != end)
	{
		this->childrens.push_back(std::make_shared<PHONEM_NODE_STRUCT>(PHONEM_NODE_STRUCT(PHONEM_TYPE::NODE_TYPE_PHONE, *it)));

#ifdef VERBOSE_MODE
		std::cout << *it << " ";
#endif

		// получаю хвост списка детей ноды
		tail = this->childrens.back();
		++it;
	}

	// в цикле дозаписываю оставшиеся фонемы в хвост
	while (it != end)
	{
		tail->childrens.push_back(std::make_shared<PHONEM_NODE_STRUCT>(PHONEM_NODE_STRUCT(PHONEM_TYPE::NODE_TYPE_PHONE, *it)));

#ifdef VERBOSE_MODE
		std::cout << *it << " ";
#endif

		tail = tail->childrens.back();
		++it;
	}

	if (tail != nullptr)
		tail->childrens.push_back(std::make_shared<PHONEM_NODE_STRUCT>(PHONEM_NODE_STRUCT(PHONEM_TYPE::NODE_TYPE_WORD, std::to_string(phonem_id++))));
	else 
		this->childrens.push_back(std::make_shared<PHONEM_NODE_STRUCT>(PHONEM_NODE_STRUCT(PHONEM_TYPE::NODE_TYPE_WORD, std::to_string(phonem_id++))));

#ifdef VERBOSE_MODE
	std::cout << phonem_id - 1 << std::endl;
#endif
}

void PHONEM_NODE_STRUCT::add_phonem_sequence(const std::vector<std::string> &sequence, PHONEM_NODE_STRUCT &head_node) {
	std::vector<std::string>::const_iterator it = sequence.begin();
	std::shared_ptr<PHONEM_NODE_STRUCT> tail;
	if (it != sequence.end())
	{
		head_node.childrens.push_back(std::make_shared<PHONEM_NODE_STRUCT>(PHONEM_NODE_STRUCT(PHONEM_TYPE::NODE_TYPE_PHONE, *it)));

#ifdef VERBOSE_MODE
		std::cout << *it << " ";
#endif

		tail = head_node.childrens.back();
		++it;
	}

	while (it != sequence.end())
	{
		tail->childrens.push_back(std::make_shared<PHONEM_NODE_STRUCT>(PHONEM_NODE_STRUCT(PHONEM_TYPE::NODE_TYPE_PHONE, *it)));

#ifdef VERBOSE_MODE
		std::cout << *it << " ";
#endif

		tail = tail->childrens.back();
		++it;
	}
	tail->childrens.push_back(std::make_shared<PHONEM_NODE_STRUCT>(PHONEM_NODE_STRUCT( PHONEM_TYPE::NODE_TYPE_WORD, std::to_string(phonem_id.fetch_add(1)) )));
	++phonem_id;

#ifdef VERBOSE_MODE
	std::cout << phonem_id - 1 << std::endl;
#endif
}

bool PHONEM_NODE_STRUCT::dictionary_to_file(std::ofstream &file_out)
{
	std::vector<std::string> phonems_line;
	return tree_traverssal(file_out, phonems_line);
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

uint32_t PHONEM_NODE_STRUCT::get_last_id() const {
	return phonem_id;
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

bool PHONEM_NODE_STRUCT::tree_traverssal(std::ofstream &file_out, std::vector<std::string> &phonems_line)
{
	std::vector<std::shared_ptr<PHONEM_NODE_STRUCT>>::iterator start_it = childrens.begin();
	while (start_it != childrens.end()) {
		if (start_it->get()->type != PHONEM_TYPE::NODE_TYPE_WORD) {
			phonems_line.push_back(start_it->get()->phonem_value);
			start_it->get()->tree_traverssal(file_out, phonems_line);
		}
		else {
			phonems_line.push_back(start_it->get()->phonem_value);
			write_line_to_dictionary(file_out, phonems_line);
			phonems_line.pop_back();
		}
		++start_it;
	}
	if (!phonems_line.empty())
		phonems_line.pop_back();

	return true;
}

void PHONEM_NODE_STRUCT::write_line_to_dictionary(std::ofstream &file_out, std::vector<std::string> &phonems_line) const {
	std::vector<std::string>::const_iterator itr = phonems_line.begin();
	std::string line = "", temp = "";
	while (itr != phonems_line.end()) {
		temp = *itr;
		++itr;
		if (itr == phonems_line.end()) {
			line.append("#" + temp + "\n");
			if (std::atoi(temp.c_str()) == 17250)
				int x = 6;
		} else
			line.append(temp + " ");
	}
	file_out << line;

#ifdef VERBOSE_MODE
	std::cout << line << std::endl;
#endif
}

PHONEM_NODE_STRUCT::~PHONEM_NODE_STRUCT() {
	std::vector<std::shared_ptr<PHONEM_NODE_STRUCT>>::iterator it = childrens.begin();
	while (it != childrens.end()) {
		it->reset();
		++it;
	}
}