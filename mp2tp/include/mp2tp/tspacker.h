#ifndef TSPACKER_H
#define TSPACKER_H

#include "tstype.h"
#include "tspckt.h"
#include "AccessUnit.h"

#include <memory>
#include <vector>

namespace lcss
{
	/// @brief TSPacker provides member functions to packetize coded media representation for an elementary stream.
	/// Use this class to create a MPEG-2 TS multiplexor.
	class TSPacker
	{
	public:
		TSPacker();
		~TSPacker();

	public:
		/// @brief Creates one TransportPacket for an elementary stream where the coded media representation size is 184 bytes or less.
		/// @param buf [in] Buffer containing the coded media representation for an elementary stream.
		/// @param bufsiz [in] The size of the buffer of @p buf.  Must be no greater than 184 bytes.
		/// @param pid [in] The PID associated with the elementary stream.
		/// @param cc [in] The continuity counter to assign to the TransportPacket instance.
		/// @return A TransportPacket for an elementary stream.
		lcss::TransportPacket packetize(uint8_t* buf, size_t bufsiz, uint16_t pid, uint8_t cc);

		/// @brief Creates a collection of TransportPacket instances for an elementary stream where the coded media representation size is greater than 184 bytes.
		/// @param buf [in] Buffer containing the coded media representation for an elementary stream.
		/// @param bufsiz [in] The size of the buffer of @p buf.
		/// @param pid [in] The PID associated with the elementary stream.
		/// @param cc [in,out] The continuity counter to assign to the TransportPacket instances.  The function will increment the @p cc for each new 
		/// TransportPacket instance created.
		/// @return A collection of TransportPacket instances for an elementary stream.
		std::vector<lcss::TransportPacket> packetize(uint8_t* buf, size_t bufsiz, uint16_t pid, size_t* cc);

		/// @brief Creates a collection of TransportPacket instances for an elementary stream where the coded media representation is based on an
		/// an AccessUnit instance.
		/// @param au [in] AccessUnit instance representing a coded media representation of a presentation time.  (i.e. video frame).
		/// @param pid [in] The PID associated with the elementary stream.
		/// @param cc [in, out] The continuity counter to assign to the TransportPacket instances.  The function will increment the @p cc for each new 
		/// TransportPacket instance created.
		/// @param pcr [in] Program Clock Reference (pcr) that will be added to the PES packet.
		/// @return A collection of TransportPacket instances for an AccessUnit belonging to an elementary stream.
		std::vector<lcss::TransportPacket> packetizePES(const lcss::AccessUnit& au, uint16_t pid, size_t* cc, uint64_t pcr);

	private:
		class Impl;
		std::unique_ptr<Impl> _pimpl;
	};
}


#endif
