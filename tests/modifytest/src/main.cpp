#include <mp2tp/tsprsr.h>
#include <mp2tp/tspckt.h>
#include <mp2tp/tspes.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class TSParserImpl :
    public lcss::TSParser
{
public:
    TSParserImpl() {}
    virtual ~TSParserImpl() {}

    void onPacket(lcss::TransportPacket& pckt)
    {
        if (pckt.payloadUnitStart())
        {
            _oneKlvSet.push_back(pckt);
        }
        else
        {
            _oneKlvSet.push_back(pckt);
        }
    }

public:
    std::vector<lcss::TransportPacket> _oneKlvSet;
};

int main(int argc, char* argv[])
{
    using namespace std;

    int ret = 0;
    ifstream ifile;

    do
    {
        ifile.open(argv[1], std::ios::in | std::ios::binary);
        if (!ifile)
        {
            cerr << "Fail to open file " << argv[1];
            ret = 2;
            break;
        }

        uint8_t memblock[1316]{};
        TSParserImpl parser;
        try
        {
            while (ifile.good())
            {
                ifile.read((char*)memblock, 1316);

                if (!parser.parse(memblock, (unsigned)ifile.gcount()))
                {
                    cerr << "Error: Not a valid MPEG-2 TS file." << endl;
                    ret = -1;
                    break;
                }
            }
        }
        catch (...)
        {
            cerr << "Unknown exception thrown\n";
            ret = -1;
            break;
        }

        if (parser._oneKlvSet.size() == 0)
        {
            cerr << "Failed to find TS Packets\n";
            ret = -1;
            break;
        }

        lcss::TransportPacket& pckt = *parser._oneKlvSet.begin();
        lcss::PESPacket pes;
        const UINT16 bytesParsed = pes.parse(pckt.getData());
        if (bytesParsed > 0)
        {
            cout << "PTS " << pes.pts() << " => ";
            uint64_t newPts = pes.pts() + 90000; // add one second
            pes.setPTS(newPts);
            uint8_t payload[188]{};
            int bytesRead = pckt.getPayload(payload, 188);
            pes.serialize(payload);
            pckt.setPayload(payload, bytesRead);
            cout << pes.pts() << endl;
        }

        // reset PID
        for (auto& ts : parser._oneKlvSet)
        {
            cout << "PID " << ts.PID() << " => ";
            ts.setPID(256);
            cout << ts.PID() << endl;
        }

    } while (false);

    return ret;
}

