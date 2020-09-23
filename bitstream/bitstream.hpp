#pragma once

#include <cstdint>
#include <vector>
#include <fstream>

namespace bitstream {
	constexpr std::uint8_t MASK = 0b10000000;

	class ofs {
	private:
		std::ofstream &ofstream;
		std::uint8_t buffer;
		std::uint8_t mask;

	public:
		explicit ofs(std::ofstream &ofstream): ofstream(ofstream), buffer(0), mask(MASK) {}
		~ofs() { flush(); }

		std::uint8_t put(bool bit) {
			if (!mask)
				flush();
			if (bit)
				buffer |= mask;
			mask >>= 1u;
			return 1u;
		}

		void flush() {
			ofstream << buffer;
			buffer = 0;
			mask = MASK;
		}

		std::uint8_t put(char ch) {
			const auto uch = static_cast <uint8_t>(ch);
			for (std::uint8_t ch_mask = MASK; ch_mask; ch_mask >>= 1u) {
				this->put(static_cast<bool>(uch & ch_mask));
			}
			return 8;
		}

		std::uint8_t put(const std::vector <bool> &bits) {
			for (bool bit : bits) {
				this->put(bit);
			}
			return bits.size();
		}

		std::uint8_t put(const std::string &str) {
			for (char ch : str) {
				this->put(ch);
			}
			return str.length() * 8;
		}
	};

	class ifs {
	private:
		std::uint8_t buffer = 0;
		std::uint8_t mask = 0;

	public:
		std::ifstream &ifstream;
		explicit ifs(std::ifstream &ifstream): ifstream(ifstream) {}

		bool get_bool() {
			if (!mask) {
				refill();
			}
			bool bit = buffer & mask;
			mask >>= 1;
			return bit;
		}

		void refill() {
			buffer = static_cast <std::uint8_t>(ifstream.get());
			mask = MASK;
		}

		char get_char() {
			std::uint8_t uch = 0;
			for (std::uint8_t ch_mask = MASK; ch_mask; ch_mask >>= 1u) {
				if (get_bool()) {
					uch |= ch_mask;
				}
			}
			return static_cast <char>(uch);
		}

		std::uint8_t bits_read_from_refill() const {
			std::uint8_t mask = this->mask;
			std::uint8_t read = 0;
			while (mask <<= 1u)
				++read;
			return read;
		}
	};
}