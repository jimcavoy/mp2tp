#include "stdafx.h"
#include "tsprsr.h"

#include "tspckt.h"

#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif

const UINT32 MPEG2_TS_PACKET_SIZE = 188;

namespace lcss
{
	class TSParser::TSParserImpl
	{
	public:
		TSParserImpl() {};

	public:
		std::unique_ptr<TransportPacket> _tspckt;
		UINT16	_cursor{};
		UINT64	_count{};
		UINT32	_packetSize{MPEG2_TS_PACKET_SIZE};
	};
}

/////////////////////////////////////////////////////////////////////////////
// TSParser

lcss::TSParser::TSParser()
{
	_pimpl = std::make_unique<lcss::TSParser::TSParserImpl>();
}

lcss::TSParser::~TSParser()
{

}

void lcss::TSParser::parse(const BYTE* stream, UINT32 len)
{
	UINT32 i = 0;
	while (i < len)
	{
		// Malformed stream. Not on the 188 boundary. Find the sync byte.
		if (*(stream + i) != 0x47)
		{
			if (_pimpl->_packetSize == MPEG2_TS_PACKET_SIZE)
			{
				if (_pimpl->_tspckt.get() != nullptr)
				{
					_pimpl->_tspckt->push_back(*(stream + i));
					if (_pimpl->_tspckt->length() == MPEG2_TS_PACKET_SIZE)
					{
						onPacket(*_pimpl->_tspckt);
						_pimpl->_tspckt.reset();
						_pimpl->_count++;
					}
				}
				i++;
			}
			else
			{
				i += (_pimpl->_packetSize - MPEG2_TS_PACKET_SIZE);
			}
		}
		else if (i + MPEG2_TS_PACKET_SIZE <= len)
		{
			if (_pimpl->_tspckt.get() == nullptr)
			{
				_pimpl->_tspckt = std::make_unique<TransportPacket>(stream + i, MPEG2_TS_PACKET_SIZE);
				i += MPEG2_TS_PACKET_SIZE;
				onPacket(*_pimpl->_tspckt);
				_pimpl->_tspckt.reset();
				_pimpl->_count++;
			}
			else // False sync byte read.  Add byte to the packet.
			{
				_pimpl->_tspckt->push_back(*(stream + i));
				if (_pimpl->_tspckt->length() == MPEG2_TS_PACKET_SIZE)
				{
					onPacket(*_pimpl->_tspckt);
					_pimpl->_tspckt.reset();
					_pimpl->_count++;
				}
				i++;
			}
		}
		else if (i + MPEG2_TS_PACKET_SIZE > len)
		{
			_pimpl->_tspckt = std::make_unique<TransportPacket>(stream + i, len - i);
			i = len;
		}
	}
}

void lcss::TSParser::onPacket(lcss::TransportPacket& pckt)
{

}

UINT64 lcss::TSParser::packetCount() const
{
	return _pimpl->_count;
}

void lcss::TSParser::setPacketSize(UINT32 sz)
{
	_pimpl->_packetSize = sz;
}