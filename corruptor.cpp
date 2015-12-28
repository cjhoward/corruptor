/*
 * Copyright (C) 2015  Christopher J. Howard
 *
 * This file is part of Corruptor.
 *
 * Corruptor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Corruptor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Corruptor.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

#define MODE_RANDOM 0
#define MODE_FILL 1
#define MODE_ADD 2
#define MODE_SHIFT 3

void usage()
{
	std::cerr << "Usage: corruptor [options] <input> <output> <start> <end>\n"\
"  -r          fill with random values\n"\
"  -f <value>  fill with a specific value\n"\
"  -a <value>  add a value to each byte\n"\
"  -s <value>  bitshift each byte\n"\
"  -n <value>  corrupt every nth byte\n";
}

int main(int argc, const char* argv[])
{
	std::srand(std::time(nullptr));

	const char* input_arg = nullptr;
	const char* output_arg = nullptr;
	const char* start_arg = nullptr;
	const char* end_arg = nullptr;
	const char** arg_table[] =
	{
		&input_arg,
		&output_arg,
		&start_arg,
		&end_arg
	};

	int mode = MODE_RANDOM;
	int fill_value;
	int add_value;
	int shift_value;
	int skip_value = 1;
	int* option_arg_table[] =
	{
		nullptr,
		&fill_value,
		&add_value,
		&shift_value,
		&skip_value
	};

	int current_arg = 0;
	for (int i = 1; i < argc; ++i)
	{
		if (argv[i][0] == '-' && std::strlen(argv[i]) == 2)
		{
			int option = 0;
			switch (argv[i][1])
			{
				case 'r':
					mode = MODE_RANDOM;
					option = 0;
					break;
				case 'f':
					mode = MODE_FILL;
					option = 1;
					break;
				case 'a':
					mode = MODE_ADD;
					option = 2;
					break;
				case 's':
					mode = MODE_SHIFT;
					option = 3;
					break;
				case 'n':
					option = 4;
					break;
				default:
					break;
			}

			if (option_arg_table[option] != nullptr)
			{
				if (i + 1 >= argc)
				{
					usage();
					return EXIT_FAILURE;
				}

				*(option_arg_table[option]) = (int)std::stol(argv[++i], nullptr, 0);
			}
		}
		else
		{
			*(arg_table[current_arg++]) = argv[i];
			if (current_arg > 4)
			{
				usage();
				return EXIT_FAILURE;
			}
		}
	}

	if (current_arg != 4)
	{
		usage();
		return EXIT_FAILURE;
	}

	std::ifstream input(input_arg, std::ios::binary);
	if (!input.is_open())
	{
		std::cerr << "Failed to open input file \"" << input_arg << "\"" << std::endl;
		return EXIT_FAILURE;
	}

	std::ofstream output(output_arg, std::ios::binary);
	if (!output.is_open())
	{
		std::cerr << "Failed to open output file \"" << output_arg << "\"" << std::endl;
		return EXIT_FAILURE;
	}

	unsigned long start = std::stoul(start_arg, nullptr, 0);
	unsigned long end = std::stoul(end_arg, nullptr, 0);

	if (end < start)
	{
		std::cerr << "Invalid end address" << std::endl;
		return EXIT_FAILURE;
	}

	// Copy input to output
	output << input.rdbuf();

	// Jump to start offset
	input.clear();
	input.seekg(start, input.beg);
	output.clear();
	output.seekp(start, output.beg);

	unsigned long position = start;
	unsigned long nth = skip_value;
	char byte;

	// Corrupt data
	while (position <= end && input.get(byte))
	{
		// Every nth byte
		if ((position - start) % nth == 0)
		{
			switch (mode)
			{
				case MODE_RANDOM:
					byte = (char)(rand() % 0xFF);
					break;

				case MODE_FILL:
					byte = fill_value;
					break;

				case MODE_ADD:
					byte = (char)((((int)byte) + add_value) % 0xFF);
					break;

				case MODE_SHIFT:
				{
					if (shift_value < 0)
						byte <<= std::abs(shift_value);
					else
						byte >>= std::abs(shift_value);
					break;
				}

				default:
					break;
			}
		}

		output.put(byte);
		++position;
	}

	input.close();
	output.close();

	return EXIT_SUCCESS;
}
