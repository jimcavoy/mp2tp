#pragma once

#include <string>

namespace lcss
{
	class TransportPacket;
	class AdaptationField;
	class ProgramAssociationTable;
	class ProgramMapTable;
	class PESPacket;
	class NetworkInformationTable;
}

class TsWriter
{
private:
	TsWriter();

public:
	static void printHeader(const lcss::TransportPacket& pckt);
	static void printAdaptationField(const lcss::AdaptationField& adf);
	static std::string printPCR(const lcss::AdaptationField& adf);
};
