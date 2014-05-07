#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string.h>
#include <stdio.h>
#include <algorithm>

const size_t LINE_BUF_LENGTH = 300;

using namespace std;

typedef unsigned int	uint;

void parse_line(const string text, uint line_buffer[]) {
    uint i = 0;
    uint s = 0;
	string::const_iterator iter = text.begin();

	if (*iter != ':') {
		cerr << "Invalid start code 0x" << hex << (*iter) << endl;
		exit(EXIT_FAILURE);
	}

    do {
        uint val = 0;
        uint cnt = 2;
		// parse one byte (two hex digits)
        do {
			++iter;
            uint ch = *iter;
            val <<= 4;
            ch -= (unsigned int) '0';

            if (ch < 10) {
                val |= ch;
                continue;
            }

            ch -= ('A' - '0');
            if (ch < 6) {
                val |= ch + 10;
                continue;
            }

            ch -= ('a' - 'A');
            if (ch < 6) {
                val |= ch + 10;
                continue;
            }

			cerr << "Invalid HEX character 0x" << hex << (*iter) << endl;
			exit(EXIT_FAILURE);
        } while (--cnt);

		if (i >= LINE_BUF_LENGTH) {
			cerr << "Line too long." << endl;
			exit(EXIT_FAILURE);
		}

        line_buffer[i++] = val;
        s += val;
    } while (i < 5 + line_buffer[0]);

	// sum of all bytes including checksum should equal zero
    if (s & 0xFF) {
		cerr << "Invalid checksum." << endl;
		exit(EXIT_FAILURE);
	}

	if (line_buffer[3] > 1) {
		cerr << "Unsupported record type 0x" << hex << line_buffer[3] << endl;
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char** argv) {

	istream* input = &cin;
	ifstream file;

	int start_of_protected_space = 0xFC00; // factory calibration data begins here

	// parse command line options
	for (int a = 1; a < argc; a++) {
		string arg = string(argv[a]);

		if (arg == "-f" || arg == "--file") {
			if (a+1 == argc) {
				cerr << "specify input file" << endl;
				return EXIT_FAILURE;
			}

			file.open(argv[++a]);
			// cout << "reading from file " << argv[a] << endl;

			if (!file) {
				cerr << "could not open file " << argv[a] << endl;
				return EXIT_FAILURE;
			}

			input = &file;
		} else if (arg == "-p" || arg == "--protect") {
			if (a+1 == argc) {
				cerr << "specify start address (as hex) of protected memory" << endl;
				return EXIT_FAILURE;
			}

			char * p;
			start_of_protected_space = strtoul(argv[++a], &p, 16);
			if (*p != 0) {
				cerr << "invalid start address specified, use hexadecimal format without leading 0x" << endl;
				return EXIT_FAILURE;
			}
		}
		else {
			cerr << "WARNING: ignoring unknown command line parameter '" << argv[a] << "'" << endl;
		}
	}

	while(1) {
		string text;
		uint line[LINE_BUF_LENGTH];
		getline(*input, text);

		// remove whitespace
		text.erase(remove_if(text.begin(), text.end(), ::isspace), text.end());

		if (!text.empty())
			parse_line(text, line);

		unsigned int len  = line[0];
		unsigned int addr = line[1] << 8 | line[2];
		unsigned int type = line[3];

		// cout << "len: " << len << " addr: 0x" << hex << addr << endl;

		// end of file record
		if (type == 1)
			break;

		if (addr+len-1 >= start_of_protected_space)  {
			cerr << "HEX image too big, would overwrite factory calibration data!" << endl;
			return EXIT_FAILURE;
		}
	}

	if (file)
		file.close();

	cout << "OK" << endl;

	return EXIT_SUCCESS;
}
