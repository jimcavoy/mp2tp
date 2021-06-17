#ifndef TSPRSR_H
#define TSPRSR_H

#include "tstype.h"

#include <memory>

namespace lcss
{

class TransportPacket;

class TSParser
{
public:
	TSParser();
	virtual ~TSParser();

	virtual void parse(const BYTE* buf, UINT32 size);

	virtual void onPacket(lcss::TransportPacket& pckt);

	UINT64 packetCount() const;

	void setPacketSize(UINT32 sz);

private:
	std::unique_ptr<TransportPacket> tspckt_;
	UINT16	cursor_;
	UINT64	count_;
	UINT32	packetSize_;
};

}

#endif