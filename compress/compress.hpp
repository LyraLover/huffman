#pragma once

#include <cstdint>
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <fstream>
#include <queue>

#include "../bitstream/bitstream.hpp"


namespace compress {

class count_table_t {
private:
	std::map <std::string, std::size_t> map;

public:
	void count(const std::string &key) { ++map[key]; }
	[[nodiscard]] std::size_t get(const std::string &key) const { return map.at(key); }
	[[nodiscard]] decltype(map)::const_iterator begin() const { return map.cbegin(); }
	[[nodiscard]] decltype(map)::const_iterator end() const { return map.cend(); }
};

class compress_table_t {
private:
	std::map <std::string, std::vector <bool>> map;

public:
	void set(const std::string &key, std::vector<bool> code) { map[key] = std::move(code); }
	[[nodiscard]] const std::vector<bool> &get(const std::string &key) const { return map.at(key); }
};

struct node_t;
using tree_p = std::unique_ptr <node_t>;
struct node_t {
	std::string symbol;
	std::size_t amount;
	tree_p left, right;
	node_t(std::string symbol, std::size_t amount): symbol(std::move(symbol)), amount(amount) {}
};

void compress(std::ifstream &ifs, std::ofstream &ofs, std::size_t word_size);
void count_stream(std::ifstream &ifs, std::size_t word_size, count_table_t &table);
tree_p create_tree(const count_table_t &table);
void create_compress_table(const tree_p &tree, std::vector <bool> &code, compress_table_t &table);
uint8_t write_tree(bitstream::ofs &out, const tree_p &tree);
uint8_t write_data(std::ifstream &ifs, bitstream::ofs &out, const compress_table_t &compress, std::size_t word_size);
void write_ignored(bitstream::ofs &out, std::uint8_t written);

}