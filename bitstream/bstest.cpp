#include "bitstream.hpp"

int main() {
	std::ofstream ofs("out", std::ios::binary);
	bitstream::ofs out(ofs);
	out.put((char) 0b01010101);
	out.put(true);
}
