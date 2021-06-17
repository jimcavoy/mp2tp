#pragma once

#include "libmp2t.h"

class TsDecoder :
	public lcss::TSParser
{
public:
	TsDecoder();
	virtual ~TsDecoder();

	virtual void onPacket(lcss::TransportPacket& pckt);

private:
	lcss::ProgramAssociationTable	_pat;
	lcss::ProgramMapTable			_pmt;
};
