#include <cstring>
#include <iostream>
#include <sstream>

#include "compress/compress.hpp"
#include "decompress/decompress.hpp"

char *program_name;
void usage() {
	std::cerr << "Usage: " << program_name << " <mode> <word_size> <input> <output>" << std::endl;
	std::cerr << "Parameters:\n"
			     "\tmode (c|d) - compress or decompress\n"
	             "\tword_size - size of the word in bytes\n"
	             "\tinput - input file\n"
	             "\toutput - output file" << std::endl;
}

int main(int argc, char **argv) {
	program_name = argv[0];

	if (argc == 1) {
		usage();
		return 0;
	}

	if (argc - 1 != 4) {
		std::cerr << "Invalid amount of arguments" << std::endl;
		usage();
		return 1;
	}

	char mode;
	if (!std::strcmp("c", argv[1]) || !strcmp("d", argv[1])) {
		mode = argv[1][0];
	}
	else {
		std::cerr << "Invalid mode" << std::endl;
		usage();
		return 2;
	}

	std::size_t word_size;
	std::stringstream size_stream;
	size_stream << argv[2];
	size_stream >> word_size;
	if (!size_stream.eof()) {
		std::cerr << "Invalid word size" << std::endl;
		usage();
		return 3;
	}

	std::ifstream ifs(argv[3], std::ios::binary);
	if (!ifs.is_open()) {
		std::cerr << "Failed to read from " << argv[3] << std::endl;
		return 4;
	}

	std::ofstream ofs(argv[4], std::ios::binary);
	if (!ofs.is_open()) {
		std::cerr << "Failed to write to " << argv[4] << std::endl;
		return 5;
	}

	switch(mode) {
		case 'c':
			compress::compress(ifs, ofs, word_size);
			break;
		case 'd':
			decompress::decompress(ifs, ofs, word_size);
			break;
		default:
			std::cerr << "Unreachable code reached" << std::endl;
			return -1;
	}

	return 0;
}
