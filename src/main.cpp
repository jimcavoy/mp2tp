// mp2tp.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <algorithm>
#include <fcntl.h>
#ifdef WIN32
#include <io.h>
#endif

#include "TsDecoder.h"

const int N = lcss::TransportPacket::TS_SIZE * 49;
const char* usage = "Usage: mp2tp -i<MPEG_transport_stream_file> -n<Count> -o<Output_file> -s<TS_packet_size>";
const char* opts = "  -i\tInput MPEG transport stream file path or standard console in (default: console).\n \
 -n\tThe minimum number of TS packets to read from the input file before exiting.\n \
   \tSet to zero to read all. (default: 1000).\n \
 -o\tOptional output file name (default: console).\n \
 -s\tTS Packet Size (default: 188).\n \
 -?\tPrint this message.";

// Forward declarations
std::unique_ptr<mp2tpser::TsDecoder> createDecoder(std::string outputFilename, std::ofstream& oStream);
bool canStop(int num, int limit);
std::string getFilename(std::string& path);

int main(int argc, char* argv[])
{
    using namespace std;
    std::string ifile;
    std::string ofile;
    int limit = 1000;
    int packetSize = 188;
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
        case 's':
            packetSize = std::stoi(*argv + 1);
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

    shared_ptr<istream> input;
    // read file from stdin
    if (ifile.empty())
    {
#ifdef WIN32
        int result = _setmode(_fileno(stdin), _O_BINARY);
        if (result < 0)
        {
            cerr << "Error: Fail to open input stream in binary mode." << endl;
            return 1;
        }
#endif
        input.reset(&cin, [](...) {});
    }
    else // read the file
    {
        ifstream* tsfile = new ifstream(ifile, std::ios::binary);
        if (!tsfile->is_open())
        {
            cerr << "Error: Fail to open input file, " << getFilename(ifile) << "." << endl;
            return -1;
        }
        input.reset(tsfile);
    }

    ofstream oStream;
    std::unique_ptr<mp2tpser::TsDecoder> decoder = createDecoder(ofile, oStream);
    decoder->setPacketSize(packetSize);

    BYTE memblock[N];
    int num_of_packets = 0;
    std::streamsize bytesRead = 0;
    bool strict = true;

    while (input->good())
    {
        input->read((char*)memblock, N);
        bytesRead += input->gcount();
        bool result = decoder->parse(memblock, (unsigned)input->gcount(), strict);

        if (!result)
        {
            if (bytesRead == N)
            {
                cerr << "Error: " << getFilename(ifile) << " is not a valid MPEG-2 TS file." << endl;
                return -1;
            }
            else
            {
                cerr << "Error: " << "Parsing error in file, " << getFilename(ifile) << ", when " << bytesRead << " bytes were read" << endl;
            }
        }

        strict = false;
        num_of_packets += (int)input->gcount() / lcss::TransportPacket::TS_SIZE;
        if (canStop(num_of_packets, limit))
            break;
    }

    cerr << "TS Packets Read: " << num_of_packets << endl;
    return 0;
}


std::unique_ptr<mp2tpser::TsDecoder> createDecoder(std::string ofile, std::ofstream& oStream)
{
    std::unique_ptr<mp2tpser::TsDecoder> decoder;
    if (ofile.empty())
    {
        decoder = std::make_unique<mp2tpser::TsDecoder>(std::cout);
    }
    else
    {
        oStream.open(ofile);
        if (oStream.is_open())
        {
            decoder = std::make_unique<mp2tpser::TsDecoder>(oStream);
        }
        else
        {
            decoder = std::make_unique<mp2tpser::TsDecoder>(std::cout);
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