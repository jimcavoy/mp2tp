#include "TsDecoder.h"

#include "TsWriter.h"
#include <iostream>
#include <sstream>

using namespace std;

TsDecoder::TsDecoder(std::ostream& ostrm)
	:_ostrm(ostrm)
{
	_ostrm << "MPEG_transport_stream() {" << endl;
}

TsDecoder::~TsDecoder()
{
	_ostrm << "}" << endl;
}


void TsDecoder::onPacket(lcss::TransportPacket& tsPacket)
{
	lcss::TransportPacket pckt = std::move(tsPacket);

	_ostrm << pckt;

	const BYTE* data = pckt.getData();
	if (pckt.payloadUnitStart())
	{
		if (pckt.PID() == 0)
		{
			_pat.parse(data);
			_ostrm << _pat;
		}
		else if (_pat.find(pckt.PID()) != _pat.end())
		{
			auto it = _pat.find(pckt.PID());
			if (it->second == 0)
			{
				_ostrm << "\t\tNetwork_information_table() { }" << endl;
			}
			else
			{
				_pmt.clear();
				_pmt.add(data, pckt.data_byte());
				if (_pmt.parse())
				{
					_ostrm << _pmt;
				}
			}
		}
		else
		{
			lcss::PESPacket pes;
			if (pes.parse(data) > 0)
			{
				_ostrm << pes;
			}
		}
	}
	else
	{
		// PMT spans across two or more TS packets
		auto it = _pat.find(pckt.PID());
		if (it != _pat.end() && it->second != 0)
		{
			_pmt.add(data, pckt.data_byte());
			if (_pmt.parse())
			{
				_ostrm << _pmt;
			}
		}
	}

	_ostrm << "\t}" << endl;
}
