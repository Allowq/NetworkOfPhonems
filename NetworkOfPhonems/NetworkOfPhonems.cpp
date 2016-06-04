// NetworkOfPhonems.cpp : Defines the entry point for the console application.
//

#include "defines.h"

PhonemsNet *phone_net;

boost::mutex io_mutex;
auto console = spdlog::stdout_logger_mt("console", true);

void enter_dict_path() {
	std::string path_to_dict = "";
	{
		boost::mutex::scoped_lock lock(io_mutex);
		std::cout << "Enter full path to dict: ";
		std::cin >> path_to_dict;
	}
	phone_net->set_dictionary_path(path_to_dict);
}

unsigned int get_option(uint32_t from, uint32_t to)
{
	std::cout << "Yours choice: ";
	unsigned int option;
	do option = (unsigned int)_getch() - 48;
	while (option < from || option > to);
	std::cout << option << std::endl << std::endl;
	return option;
}

std::pair<LOAD_OPTIONS_ENUM, std::string> parsing_parameters(int argc, char* argv[])
{
	namespace po = boost::program_options;
	std::string appName = boost::filesystem::basename(argv[0]);
	po::options_description desc("Options");
	desc.add_options()
		("help,h", "Print help message")
		("create,c", po::value<std::string>(), "Create dictionary from file");

	po::variables_map vm;

	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);

		if (vm.count("help")) {
			std::cout << "This is test work for Speech Technology Center" << std::endl << std::endl;
			std::cout << "USAGE: " << appName << " [-h] [-c PATH]" << std::endl;
			std::cout << "-- Option Description --" << std::endl;
			std::cout << "\t-h[--help]" << "\t" << "Print help message" << std::endl;
			std::cout << "\t-c[--path]" << "\t" << "Create dictionary from file" << std::endl;
			exit(0);
		}

		po::notify(vm);
	}
	catch (boost::program_options::required_option &e) {
		std::cout << "Error: " << e.what() << std::endl << std::endl;
		std::cout << "For print help message use -h[--help] key" << std::endl;
		exit(0);
	}
	catch (boost::program_options::error &e) {
		std::cout << "Error: " << e.what() << std::endl << std::endl;
		exit(0);
	}

	if (vm.count("create") && !vm["create"].as<std::string>().empty())
		return std::pair<LOAD_OPTIONS_ENUM, std::string>(LOAD_OPTIONS_ENUM::dict_from_file, vm["create"].as<std::string>());

	return std::pair<LOAD_OPTIONS_ENUM, std::string>(LOAD_OPTIONS_ENUM::none, "");
}

void signal_handler(int32_t signal)
{
	console->info("\nSignal {} was send", signal);

	if (phone_net) {
		phone_net->interrupt();
		delete phone_net;
		phone_net = NULL;
	}

	exit(signal);
}

int main(int argc, char* argv[])
{
	std::pair<LOAD_OPTIONS_ENUM, std::string> pair = parsing_parameters(argc, argv);
	uint32_t option = 0;
	console->set_pattern("%v");

	phone_net = new PhonemsNet();
	if (pair.first == LOAD_OPTIONS_ENUM::dict_from_file)
		phone_net->set_dictionary_path(pair.second);

	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	while (phone_net && !phone_net->is_interrupted())
	{
		{
			boost::mutex::scoped_lock lock(io_mutex);
			std::cout << "\n\n- - - - -" << std::endl;
			std::cout << "Choose option: " << std::endl
				<< "[1] Create network" << std::endl
				<< "[2] Get id" << std::endl
				<< "[3] Generate dictionary" << std::endl
				<< "[4] Set path to dictionary" << std::endl
				<< "[0] Exit" << std::endl
				<< "- - - - -" << std::endl;
		}

		option = get_option(0, 4);
		switch (option)
		{
		case 0:
			phone_net->interrupt();
			delete phone_net;
			phone_net = NULL;
			break;

		case 1:
			phone_net->create_network();
			break;

		case 2:
			if (phone_net->dict_is_set())
			{
				std::string phonems = "";
				{
					boost::mutex::scoped_lock lock(io_mutex);

					std::cout << "Please type sequence of phonems: ";
					std::getline(std::cin, phonems);
				}
				std::cout << "ID: " << std::to_string(phone_net->get_id_by_set(phonems)) << std::endl;
			}
			else
				console->info("\nFail! Path to dictionary not seted");
			break;

		case 3:
			if (phone_net->dict_is_set())
			{
				std::string file_name = "";
				{
					boost::mutex::scoped_lock lock(io_mutex);
					std::cout << "Please type file name: ";
					std::cin >> file_name;
				}
				phone_net->generate_dictionary(file_name);
			}
			else
				console->info("\nFail! Path to dictionary not seted");
			break;

		case 4:
			enter_dict_path();
			break;

		default:
			continue;
		}
	}

    return 0;
}

