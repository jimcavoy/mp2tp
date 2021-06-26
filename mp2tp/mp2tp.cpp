// mp2tp.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <fstream>
#include <string>
#include <memory>

#include "TsDecoder.h"

const int N = 188 * 49;

std::unique_ptr<TsDecoder> createDecoder(std::string outputFilename, std::ofstream& oStream);

int main(int argc, char* argv[])
{
	using namespace std;
	std::string ifile;
	std::string ofile;
	int limit = 10000;
	char c;

	while (--argc > 0 && (*++argv)[0] == '-')
	{
		c = *++argv[0];
		switch (c)
		{
		case 'i':
			ifile = *argv + 1;
			break;
		case 'o':
			ofile = *argv + 1;
			break;
		case 'n':
			limit = std::stoi(*argv + 1);
			break;
		default:
			printf("mp2tp: illegal option %c\n", c);
			argc = 0;
			return -1;
		}
	}
	if (argc != 0)
	{
		printf("Usage: mp2tp -i<MPEG_transport_stream_file> -n<Count> -o<Output_file>\n");
		return -1;
	}

	ifstream tsfile;
	tsfile.open(ifile, std::ios::binary);
	if (!tsfile.is_open())
	{
		cerr << "Error: Fail to open file " << ifile;
		return -1;
	}

	ofstream oStream;
	std::unique_ptr<TsDecoder> decoder = createDecoder(ofile, oStream);
	BYTE memblock[N];
	int num_of_packets = 0;

	while (tsfile.good())
	{
		if (num_of_packets < limit)
		{
			tsfile.read((char*)memblock, N);
			decoder->parse(memblock, (unsigned)tsfile.gcount());
			num_of_packets += (int)tsfile.gcount() / 188;
		}
		else
		{
			break;
		}
	}

	cerr << "TS Packets Read: " << num_of_packets << endl;
	return 0;
}


std::unique_ptr<TsDecoder> createDecoder(std::string ofile, std::ofstream& oStream)
{
	std::unique_ptr<TsDecoder> decoder;
	if (ofile.empty())
	{
		decoder = std::make_unique<TsDecoder>(std::cout);
	}
	else
	{
		oStream.open(ofile);
		if (oStream.is_open())
		{
			decoder = std::make_unique<TsDecoder>(oStream);
		}
		else
		{
			decoder = std::make_unique<TsDecoder>(std::cout);
		}
	}
	return decoder;
}