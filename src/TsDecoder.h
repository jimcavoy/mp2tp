#pragma once

#include <mp2tp/libmp2tp.h>"
#include <iostream>
#include <sstream>

class TsDecoder :
	public lcss::TSParser
{
public:
	TsDecoder(std::ostream& ostrm);
	virtual ~TsDecoder();

	virtual void onPacket(lcss::TransportPacket& pckt) override;

private:
	lcss::ProgramAssociationTable	_pat;
	lcss::ProgramMapTable			_pmt;
	std::ostream&					_ostrm;
};

