#include "stdafx.h"
#include "tsprsr.h"

#include "tspckt.h"

#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif

const UINT32 MPEG2_TS_PACKET_SIZE = 188;

/////////////////////////////////////////////////////////////////////////////
// TSParser

lcss::TSParser::TSParser()
	:cursor_(0)
	, count_(0)
	, packetSize_(MPEG2_TS_PACKET_SIZE)
{

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
			if (packetSize_ == MPEG2_TS_PACKET_SIZE)
			{
				if (tspckt_.get() != nullptr)
				{
					tspckt_->push_back(*(stream + i));
					if (tspckt_->length() == MPEG2_TS_PACKET_SIZE)
					{
						onPacket(*tspckt_);
						tspckt_.reset();
						count_++;
					}
				}
				i++;
			}
			else
			{
				i += (packetSize_ - MPEG2_TS_PACKET_SIZE);
			}
		}
		else if (i + MPEG2_TS_PACKET_SIZE <= len)
		{
			if (tspckt_.get() == nullptr)
			{
				tspckt_ = std::make_unique<TransportPacket>(stream + i, MPEG2_TS_PACKET_SIZE);
				i += MPEG2_TS_PACKET_SIZE;
				onPacket(*tspckt_);
				tspckt_.reset();
				count_++;
			}
			else // False sync byte read.  Add byte to the packet.
			{
				tspckt_->push_back(*(stream + i));
				if (tspckt_->length() == MPEG2_TS_PACKET_SIZE)
				{
					onPacket(*tspckt_);
					tspckt_.reset();
					count_++;
				}
				i++;
			}
		}
		else if (i + MPEG2_TS_PACKET_SIZE > len)
		{
			tspckt_ = std::make_unique<TransportPacket>(stream + i, len - i);
			i = len;
		}
	}
}

void lcss::TSParser::onPacket(lcss::TransportPacket& pckt)
{

}

UINT64 lcss::TSParser::packetCount() const
{
	return count_;
}

void lcss::TSParser::setPacketSize(UINT32 sz)
{
	packetSize_ = sz;
}