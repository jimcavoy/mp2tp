#ifndef TSPACKER_H
#define TSPACKER_H

#include "tstype.h"
#include "tspckt.h"
#include "AccessUnit.h"

#include <memory>
#include <vector>

namespace lcss
{
	/// <summary>
	/// 
	/// </summary>
	class TSPacker
	{
	public:
		TSPacker();
		~TSPacker();

	public:
		lcss::TransportPacket packetize(uint8_t* buf, size_t bufsiz, uint16_t pid, uint8_t cc);

		std::vector<lcss::TransportPacket> packetize(uint8_t* buf, size_t bufsiz, uint16_t pid, size_t* cc);

		std::vector<lcss::TransportPacket> packetizePES(const lcss::AccessUnit& au, uint16_t pid, size_t* cc, uint64_t pcr);

	private:
		class Impl;
		std::unique_ptr<Impl> _pimpl;
	};
}


#endif
