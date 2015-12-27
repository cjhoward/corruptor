# Corruptor

This is a small utility program which corrupts a specific range of data in a file. It was created to assist in reverse engineering ROMs. There are currently four modes of corruption: random, fill, add, and bitshift.

## Building

	g++ -std=c++11 corruptor.cpp -o corruptor

## Usage

	corruptor [options] <input> <output> <start> <end>
	  -r          fill with random values
	  -f <value>  fill with a specific value
	  -a <value>  add a value to each byte
	  -s <value>  bitshift each byte

The following example fills a block of data from 0x00000 to 0x7FFFF with the value 0x10:

	corruptor -f 0x10 input.bin output.bin 0x00000 0x7FFFF

## License

This program is licensed under the GNU General Public License, version 3. See [COPYING](COPYING) for details.
