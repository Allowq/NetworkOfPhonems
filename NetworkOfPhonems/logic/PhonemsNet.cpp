#include "PhonemsNet.h"

uint32_t PhonemsNet::word_count = 0;

PhonemsNet::PhonemsNet()
	: path_to_dict(""), is_interrupt(false)
{
	console = spdlog::get("console");
#ifdef LOCALE_DICTIONARY
	path_to_dict = "dictionary.txt";
#endif
}

void PhonemsNet::create_network() {
	if (!dict_is_set()) {
		console->info("\nFail! Path to dictionary not seted");
		return;
	}

	if (phonem_table.empty())
		if (!init_phonem_table())
			return;

	std::string line, full_word, phonems_line;
	std::stringstream liness;

	while (getline(dict_file, line)) {
		if (!line.empty() && line != "\r")
		{
			liness << line;

			getline(liness, full_word, '=');
			getline(liness, phonems_line, '#');

			parse_and_add(phonems_line);
		}
	}

	console->info("\nNumber of words = ") << head_of_net.get_last_id();
}

bool PhonemsNet::dict_is_set() {
	if (dict_file.is_open())
		return true;
	else if (!path_to_dict.empty())
	{
		dict_file.open(path_to_dict, std::ifstream::binary);
		if (dict_file && dict_file.is_open() && dict_file.good())
			return true;
		else 
			path_to_dict = "";
	}

	return false;
}

void PhonemsNet::generate_dictionary(const std::string &file_name) const {
	std::string out_file_name = file_name;
	
	if (out_file_name.empty())
		out_file_name = "empty.txt";
	std::ofstream file_out(out_file_name, std::ofstream::binary / std::ofstream::trunc);

	if (!file_out || !file_out.is_open() || !file_out.good()) {
		console->info("\nFail! File to dictionary not available");
		return;
	}

	PHONEM_NODE_STRUCT *head = const_cast<PHONEM_NODE_STRUCT *>(&head_of_net);
	if (head->dictionary_to_file(file_out)) 
		console->info("\nDictionary success upload");
}

int32_t PhonemsNet::get_id_by_set(const std::string &phonems) {
	std::stringstream words(phonems);
	std::string phonem;

	std::vector<std::string> phonem_vector;
	while (words >> phonem)
		phonem_vector.push_back(phonem);

	std::shared_ptr<PHONEM_NODE_STRUCT> local_tail = nullptr;
	std::vector<std::string>::iterator shift = phonem_vector.begin();

	int32_t id_value = -1;
	head_of_net.get_id_for_phonems(shift, phonem_vector.end(), local_tail, id_value);

	return id_value;
}

bool PhonemsNet::init_phonem_table() {
	std::ifstream file("phone.txt", std::ifstream::binary);
	if (!file || !file.is_open() || !file.good()) {
		console->info("\nFail! Phonem table not found");
		return false;
	}

	std::string line, phonem;

	while (getline(file, line)) {
		std::stringstream liness(line);
		getline(liness, phonem, '/');
		phonem.erase(std::remove(phonem.begin(), phonem.end(), '"'), phonem.end());
		phonem.erase(std::remove(phonem.begin(), phonem.end(), '\t'), phonem.end());
		phonem.erase(std::remove(phonem.begin(), phonem.end(), ','), phonem.end());
		phonem_table.insert(phonem);
	}

	return true;
}

void PhonemsNet::interrupt() {
	is_interrupt = true;
}

bool PhonemsNet::is_interrupted() const {
	return is_interrupt;
}

void PhonemsNet::parse_and_add(const std::string &phonems_line) {
	std::stringstream words(phonems_line);
	std::string phonem;

	std::shared_ptr<PHONEM_NODE_STRUCT> local_tail = nullptr;
	std::vector<std::string> phonem_vector;
	std::vector<std::string>::iterator shift;

	while (words >> phonem)
		phonem_vector.push_back(phonem);
	shift = phonem_vector.begin();

	head_of_net.get_tail_for_phonems(shift, phonem_vector.end(), local_tail);
	if (local_tail == nullptr)
		head_of_net.add_phonem_sequence(phonem_vector, head_of_net);
	else
		local_tail.get()->add_phonem_sequence(shift, phonem_vector.end());
}

void PhonemsNet::set_dictionary_path(const std::string &path) {
	if (!path.empty()) {
		if (!path_to_dict.empty()) {
			dict_file.close();
			phonem_table.clear();
			word_count = 0;
		}
		path_to_dict = path;
	}
}

PhonemsNet::~PhonemsNet() {
	if (dict_file.is_open())
		dict_file.close();
}
