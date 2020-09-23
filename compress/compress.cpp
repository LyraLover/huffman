#include "compress.hpp"

#include <iostream>
#include <climits>


namespace compress {

void compress(std::ifstream &ifs, std::ofstream &ofs, std::size_t word_size) {
	count_table_t count;
	count_stream(ifs, word_size, count);

	tree_p tree = create_tree(count);

	compress_table_t compress;
	std::vector <bool> code;
	create_compress_table(tree, code, compress);

	bitstream::ofs out(ofs);
	std::uint8_t written = 0; // choice of type: only 8 - written % 8 is needed
	written += write_tree(out, tree);
	written += write_data(ifs, out, compress, word_size);
	write_ignored(out, written);
}

void count_stream(std::ifstream &ifs, std::size_t word_size, count_table_t &table) {
	const std::size_t ifs_pos = ifs.tellg();

	char ch;
	std::string buffer;
	buffer.reserve(word_size);

	while (ifs.get(ch)) {
		buffer += ch;
		if (buffer.length() == word_size) {
			table.count(buffer);
			buffer.clear();
		}
	}
	if (!buffer.empty()) {
		while (buffer.length() != word_size) {
			buffer += '\0';
		}
		table.count(buffer);
	}

	ifs.clear();
	ifs.seekg(ifs_pos);
}

tree_p create_tree(const count_table_t &table) {
	auto comp = [](node_t *lhs, node_t *rhs) { return lhs->amount > rhs->amount; };
	std::priority_queue <node_t *, std::vector <node_t *>, decltype(comp)> queue;
	for (const auto &pair : table) {
		auto node = new node_t(pair.first, pair.second);
		queue.push(node);
	}

	while (queue.size() != 1) {
		auto left = queue.top(); queue.pop();
		auto right = queue.top(); queue.pop();

		auto node = new node_t("", left->amount + right->amount);
		node->left = tree_p(left);
		node->right = tree_p(right);
		queue.push(node);
	}

	return tree_p(queue.top());
}

void create_compress_table(const tree_p &tree, std::vector <bool> &code, compress_table_t &table) {
	if (tree->left) {
		code.push_back(false);
		create_compress_table(tree->left, code, table);
		code.back() = true;
		create_compress_table(tree->right, code, table);
		code.pop_back();
	}
	else {
		table.set(tree->symbol, code);
	}
}

uint8_t write_tree(bitstream::ofs &out, const tree_p &tree) {
	uint8_t written = 0;

	if (tree->left) {
		written += out.put(false);
		written += write_tree(out, tree->left);
		written += write_tree(out, tree->right);
	}
	else {
		written += out.put(true);
		written += out.put(tree->symbol);
	}

	return written;
}

uint8_t write_data(std::ifstream &ifs, bitstream::ofs &out, const compress_table_t &compress, std::size_t word_size) {
	uint8_t written = 0;
	const std::size_t ifs_pos = ifs.tellg();

	char ch;
	std::string buffer;
	buffer.reserve(word_size);
	while (ifs.get(ch)) {
		buffer += ch;
		if (buffer.length() == word_size) {
			const std::vector <bool> &code = compress.get(buffer);
			written += out.put(code);
			buffer.clear();
		}
	}
	if (!buffer.empty()) {
		while (buffer.length() != word_size) {
			buffer += '\0';
		}
		const std::vector <bool> &code = compress.get(buffer);
		written += out.put(code);
	}

	ifs.clear();
	ifs.seekg(ifs_pos);

	return written;
}

void write_ignored(bitstream::ofs &out, std::uint8_t written) {
	if (written % 8) {
		out.flush();
	}
	std::uint8_t ignored = 8 - written % 8;
	for (std::uint8_t mask = 0b100; mask; mask >>= 1u) {
		out.put(static_cast <bool>(ignored & mask));
	}
}

}