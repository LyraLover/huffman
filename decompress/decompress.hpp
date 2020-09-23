#pragma once

#include <fstream>
#include <string>
#include <memory>

#include "../bitstream/bitstream.hpp"

namespace decompress {

struct node_t;
using tree_p = std::unique_ptr <node_t>;
struct node_t {
	std::string symbol;
	tree_p left, right;

	explicit node_t(std::string symbol): symbol(std::move(symbol)) {}
	node_t(tree_p &left, tree_p &right): left(std::move(left)), right(std::move(right)) {}
};

void decompress(std::ifstream &ifs, std::ofstream &ofs, std::size_t word_size);
std::uint8_t get_ignored(std::ifstream &ifs);
tree_p read_tree(bitstream::ifs &in, std::size_t word_size);
	void write_data(bitstream::ifs &in, std::ofstream &ofs, const tree_p &tree, std::size_t ignored);

}