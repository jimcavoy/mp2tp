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
	void printHeader(const lcss::TransportPacket& pckt);
	void printAdaptationField(const lcss::AdaptationField& adf);

private:
	lcss::ProgramAssociationTable	_pat;
	lcss::ProgramMapTable			_pmt;
};
