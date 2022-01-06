#pragma once

#include "tstype.h"

namespace lcss
{

class PESPacket
{
public:
	PESPacket();
	~PESPacket();

	UINT16 parse(const BYTE* stream);
	
	bool hasPacketStartCodePrefix() const;
	void reset();

	double ptsInSeconds() const;
	double dtsInSeconds() const;

	UINT64 pts() const;
	UINT64 dts() const;

	UINT16 parsePTS(const BYTE* stream);

public:
	BYTE packet_start_code_prefix_[3]{};
	BYTE stream_id_{};
	UINT16 PES_packet_length_{};
	BYTE flags1_{};
	BYTE flags2_{};
	BYTE PES_header_data_length_{};
	BYTE PTS_[5]{};
	BYTE DTS_[5]{};
};

}


