#ifndef TSPACKER_H
#define TSPACKER_H

#include "tstype.h"
#include "tspckt.h"

#include <memory>

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
		lcss::TransportPacket packetize(uint8_t* buf, uint32_t bufsiz, uint16_t pid, uint8_t cc);

	private:
		class Impl;
		std::unique_ptr<Impl> _pimpl;
	};
}


#endif
