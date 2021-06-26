#pragma once

#include "libmp2t.h"
#include <iostream>
#include <sstream>

class TsDecoder :
	public lcss::TSParser
{
public:
	TsDecoder(std::ostream& ostrm);
	virtual ~TsDecoder();

	virtual void onPacket(lcss::TransportPacket& pckt);

private:
	lcss::ProgramAssociationTable	_pat;
	lcss::ProgramMapTable			_pmt;
	std::ostream&					_ostrm;
};

