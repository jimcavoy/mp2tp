// mp2tp.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <fstream>

#include "TsDecoder.h"

const int N = 188 * 49;

int main(int argc, char* argv[])
{
	using namespace std;

	if (argc != 2)
	{
		cout << "usage: mp2tp <MPEG_transport_stream_file>" << endl;
		return -1;
	}

	ifstream tsfile;
	tsfile.open(argv[1], std::ios::binary);
	if (!tsfile.is_open())
	{
		cerr << "Error: Fail to open file " << argv[1];
		return -1;
	}

	TsDecoder decoder;
	BYTE memblock[N];
	int num_of_packets = 0;

	while (tsfile.good())
	{
		tsfile.read((char*)memblock, N);
		decoder.parse(memblock, (unsigned)tsfile.gcount());
		num_of_packets += (int) tsfile.gcount() / 188;
		if (num_of_packets > 10000)
			break;
	}

	cerr << "TS Packets Read: " << num_of_packets << endl;
	return 0;
}
