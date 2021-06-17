#include "stdafx.h"
#include "tspes.h"

#ifndef WIN32
#include <memory.h>
#include <arpa/inet.h>
#endif

#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif

/////////////////////////////////////////////////////////////////////////////
namespace
{
	const BYTE PTS_DTS_MASK = 0xC0;
};

/////////////////////////////////////////////////////////////////////////////

lcss::PESPacket::PESPacket()
:stream_id_(0)
{
#ifdef WIN32
	SecureZeroMemory(packet_start_code_prefix_,3);
	SecureZeroMemory(PTS_,5);
	SecureZeroMemory(DTS_,5);
#else
	memset(packet_start_code_prefix_, 0, 3);
	memset(PTS_, 0, 5);
	memset(DTS_, 0, 5);
#endif
}

lcss::PESPacket::~PESPacket()
{
	
}

UINT16 lcss::PESPacket::parse(const BYTE* stream)
{
	UINT16 cur = 0;
	if(stream_id_ == 0)
	{
		BYTE value;
		memcpy(packet_start_code_prefix_,stream,3);
		cur += 3; // skip the PES start markers which is 3 bytes;

		if(!hasPacketStartCodePrefix())
			return 0;

		memcpy(&value,stream+cur,1);
		stream_id_ = value;
		cur++;

		UINT16 iValue;
		memcpy(&iValue,stream+cur,2);
		PES_packet_length_ = ntohs(iValue);
		cur += 2;

		if(stream_id_ != 188		// program stream map
			&& stream_id_ != 190	// padding stream
			&& stream_id_ != 191	// private stream 2
			&& stream_id_ != 240	// ECM
			&& stream_id_ != 241	// EMM
			&& stream_id_ != 255	// program stream directory
			&& stream_id_ != 242	// DSMCC stream
			&& stream_id_ != 248	// ITU-T Rec. H.222.1 type E stream
			)
		{
			memcpy(&value,stream+cur,1);
			flags1_ = value;
			cur++;

			memcpy(&value,stream+cur,1);
			flags2_ = value;
			cur++;

			memcpy(&value,stream+cur,1);
			PES_header_data_length_ = value;
			cur++;
			int curPlusHeaderLen = cur+PES_header_data_length_;

			UINT16 ptsflag = (flags2_ & PTS_DTS_MASK);
			if(ptsflag > 0)
			{
				cur += parsePTS(stream+cur);
			}

			// Check for stuffing bytes and skip over them
			cur += (curPlusHeaderLen-cur);
		}
	}
	return cur;
}

bool lcss::PESPacket::hasPacketStartCodePrefix() const
{
	if(packet_start_code_prefix_[0] == 0 
		&& packet_start_code_prefix_[1] == 0 
		&& packet_start_code_prefix_[2] == 1)
		return true;
	return false;
}

void lcss::PESPacket::reset()
{
	stream_id_ = 0;
}

UINT16 lcss::PESPacket::parsePTS( const BYTE* stream)
{
	UINT16 cur = 0;
	UINT16 value = (flags2_ & PTS_DTS_MASK);
	if(value > 0)
	{
		if(value == 0xC0)
		{
			memcpy(PTS_,stream,5);
			cur += 5;

			memcpy(DTS_,stream+cur,5);
			cur += 5;
		}
		else if(value == 0x80)
		{
			memcpy(PTS_,stream,5);
			cur += 5;
		}
	}
	return cur;
}

/// <summary>
/// Presentation Time Stamp (PTS) in seconds.
/// </summary>
/// <returns></returns>
double lcss::PESPacket::ptsInSeconds() const
{
	double t = 0.0;

	UINT16 value = (flags2_ & PTS_DTS_MASK);
	if(value == 0xC0 || value == 0x80)
	{
		t = pts() / 90000.0;
	}
	return t;
}

/// <summary>
/// Decoding Time Stamp (DTS) in seconds.
/// </summary>
/// <returns></returns>
double lcss::PESPacket::dtsInSeconds() const
{
	double t = 0.0;

	UINT16 value = (flags2_ & PTS_DTS_MASK);
	if(value == 0xC0)
	{
		t = dts() / 90000.0;
	}
	return t;
}

/// <summary>
/// Presentation Time Stamp (PTS) in units of 90 kHz.
/// </summary>
/// <returns></returns>
UINT64 lcss::PESPacket::pts() const
{
	UINT64 PTS = 0;

	UINT16 value = (flags2_ & PTS_DTS_MASK);
	if (value == 0xC0 || value == 0x80)
	{
		BYTE PTS1 = (PTS_[0] & 0x0e);
		UINT16 PTS2;
		memcpy(&PTS2, PTS_ + 1, 2);
		PTS2 = ntohs(PTS2);
		UINT16 PTS3;
		memcpy(&PTS3, PTS_ + 3, 2);
		PTS3 = ntohs(PTS3);

		PTS = PTS1 << 29 |
			((PTS2) >> 1) << 15 |
			(PTS3) >> 1;

		PTS = PTS & 0x1FFFFFFFF;
	}
	return PTS;
}

/// <summary>
/// Decoding Time Stamp (DTS) in units of 90 kHz.
/// </summary>
/// <returns></returns>
UINT64 lcss::PESPacket::dts() const
{
	UINT64 DTS = 0;

	UINT16 value = (flags2_ & PTS_DTS_MASK);
	if (value == 0xC0)
	{
		BYTE DTS1 = (DTS_[0] & 0x0e);
		UINT16 DTS2;
		memcpy(&DTS2, DTS_ + 1, 2);
		DTS2 = ntohs(DTS2);
		UINT16 DTS3;
		memcpy(&DTS3, DTS_ + 3, 2);
		DTS3 = ntohs(DTS3);

		DTS = DTS1 << 29 |
			((DTS2) >> 1) << 15 |
			(DTS3) >> 1;

		DTS = DTS & 0x1FFFFFFFF;
	}
	return DTS;
}
