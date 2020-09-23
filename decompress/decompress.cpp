#include "decompress.hpp"

namespace decompress {

	void decompress(std::ifstream &ifs, std::ofstream &ofs, std::size_t word_size) {
		std::uint8_t ignored = get_ignored(ifs);
		bitstream::ifs in(ifs);
		tree_p tree = read_tree(in, word_size);
		write_data(in, ofs, tree, ignored);
	}

	std::uint8_t get_ignored(std::ifstream &ifs) {
		const std::size_t ifs_pos = ifs.tellg();
		ifs.seekg(-1, std::ios::end);
		auto ignored = static_cast<std::uint8_t>(ifs.peek());
		ifs.seekg(ifs_pos);
		return ignored >> 5u;
	}

	tree_p read_tree(bitstream::ifs &in, std::size_t word_size) {
		if (in.get_bool()) {
			std::string symbol;
			symbol.reserve(word_size);
			while (symbol.length() != word_size) {
				symbol += in.get_char();
			}
			return std::make_unique <node_t>(symbol);
		}
		else {
			tree_p left = read_tree(in, word_size),
			       right = read_tree(in, word_size);
			return std::make_unique <node_t>(left, right);
		}
	}

	void write_data(bitstream::ifs &in, std::ofstream &ofs, const tree_p &tree, std::size_t ignored) {
		auto node = tree.get();
		auto beg = in.ifstream.tellg();
		in.ifstream.seekg(-1, std::ios::end);
		auto end = in.ifstream.tellg();
		in.ifstream.seekg(beg);

		while (true) {
			bool bit = in.get_bool();
			if (bit) // right
				node = node->right.get();
			else     // left
				node = node->left.get();

			if (!node->symbol.empty()) {
				for (char ch : node->symbol) {
					ofs.put(ch);
				}
				node = tree.get();
				auto p = in.ifstream.tellg();
				if (in.ifstream.tellg() == end && in.bits_read_from_refill() + ignored == 8u)
					break;
			}
		}
	}

}