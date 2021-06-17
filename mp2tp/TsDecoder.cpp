#include "TsDecoder.h"

#ifdef WIN32
#include <gsl/gsl>
#endif
#include "TsWriter.h"
#include <iostream>
#include <sstream>

using namespace std;

TsDecoder::TsDecoder()
{
	cout << "MPEG_transport_stream() {" << endl;
}

TsDecoder::~TsDecoder()
{
	cout << "}" << endl;
}

void TsDecoder::onPacket(lcss::TransportPacket& pckt)
{
	TsWriter::printHeader(pckt);

	const BYTE* data = pckt.getData();
	if (pckt.payloadUnitStart())
	{
		if (pckt.PID() == 0)
		{
			_pat.parse(data);
			cout << "\t\tprogram_association_section() { }" << endl;
		}
		else if (_pat.find(pckt.PID()) != _pat.end())
		{
			auto it = _pat.find(pckt.PID());
			if (it->second == 0)
			{
				cout << "\t\tNetwork_information_table() { }" << endl;
			}
			else
			{
				_pmt.clear();
#ifdef WIN32
				_pmt.add({ data, pckt.data_byte() });
#else
				_pmt.add(data, pckt.data_byte());
#endif
				if (_pmt.parse())
				{
					cout << "\t\tTS_program_map_section() { }" << endl;
				}
			}
		}
		else
		{
			lcss::PESPacket pes;
			int bytesParsed = pes.parse(data);
			if (bytesParsed > 0)
			{
				cout << "\t\tPES_packet() { }" << endl;
			}
		}
	}
	else
	{
		// PMT spans across two or more TS packets
		auto it = _pat.find(pckt.PID());
		if (it != _pat.end() && it->second != 0)
		{
#ifdef WIN32
			_pmt.add({ data, pckt.data_byte() });
#else
			_pmt.add(data, pckt.data_byte());
#endif
			if (_pmt.parse())
			{
				cout << "\t\tTS_program_map_section() { }" << endl;
			}
		}
	}

	cout << "\t}" << endl;
}
