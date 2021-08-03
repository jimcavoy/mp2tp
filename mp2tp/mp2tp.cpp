// mp2tp.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <algorithm>

#include "TsDecoder.h"

const int N = lcss::TransportPacket::TS_SIZE * 49;
const char* usage = "Usage: mp2tp -i<MPEG_transport_stream_file> -n<Count> -o<Output_file>";
const char* opts = "  -i\tInput MPEG transport stream file path.\n  -n\tThe minimum number of TS packets to read from the input file before exiting.\n    \tSet to zero to read all. (default: 1000).\n  -o\tOptional output file name (default: console).\n  -?\tPrint this message.";

// Forward declarations
std::unique_ptr<TsDecoder> createDecoder(std::string outputFilename, std::ofstream& oStream);
bool canStop(int num, int limit);
std::string getFilename(std::string& path);

int main(int argc, char* argv[])
{
	using namespace std;
	std::string ifile;
	std::string ofile;
	int limit = 1000;
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
		case '?':
			cout << usage << endl;
			cout << endl << "Options: " << endl;
			cout << opts << endl;
			return 0;
			break;
		default:
			cout << "mp2tp: illegal option " << c << endl;
			return -1;
		}
	}

	if (argc == 0 && ifile.empty())
	{
		cout << usage << endl;
		return -1;
	}

	ifstream tsfile;
	tsfile.open(ifile, std::ios::binary);
	if (!tsfile.is_open())
	{
		cout << "Error: Fail to open input file, " << getFilename(ifile) << endl;
		return -1;
	}

	ofstream oStream;
	std::unique_ptr<TsDecoder> decoder = createDecoder(ofile, oStream);

	BYTE memblock[N];
	int num_of_packets = 0;
	while (tsfile.good())
	{
		tsfile.read((char*)memblock, N);
		decoder->parse(memblock, (unsigned)tsfile.gcount());
		num_of_packets += (int)tsfile.gcount() / lcss::TransportPacket::TS_SIZE;
		if (canStop(num_of_packets, limit))
			break;
	}

	cout << "TS Packets Read: " << num_of_packets << endl;
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

bool canStop(int num, int limit)
{
	if (limit == 0)
		return false;
	return num > limit ? true : false;
}

std::string getFilename(std::string& path)
{
	std::string fname;
	std::string::const_reverse_iterator it;
	for (it = path.rbegin(); it != path.rend(); ++it)
	{
		if (*it == '\\' || *it == '/')
			break;
		fname.push_back(*it);
	}
	std::reverse(fname.begin(), fname.end());
	return fname;
}