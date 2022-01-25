#pragma once

#include "tstype.h"
#include <memory>

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

	BYTE streamId() const;
	UINT16 packetLength() const;
	BYTE flags1() const;
	BYTE flags2() const;
	BYTE headerDataLength() const;

private:
	class Impl;
	std::unique_ptr<Impl> _pimpl;
};

}


