#pragma once

#include "tstype.h"
#include <memory>

namespace lcss
{
	/// @brief TransportPacket implements the PES_packet() as defined in
	/// Ref: ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Reserved, Table 2-21 page 33.
	/// How to use: If the TransportPacket::payloadUnitStart is true, 
	/// get the payload data from a TransportPacket::getData. 
	/// Pass the payload data into PESPacket::parse to read PES fields.
	class PESPacket
	{
	public:
		PESPacket();
		~PESPacket();

		PESPacket(const PESPacket& other);
		PESPacket& operator=(const PESPacket& rhs);

		PESPacket(PESPacket&&) noexcept;
		PESPacket& operator=(PESPacket&&) noexcept;

		UINT16 parse(const BYTE* stream);

		// PES packet fields
		BYTE stream_id() const;
		UINT16 packet_length() const;
		BYTE flags1() const;
		BYTE flags2() const;
		BYTE header_data_length() const;
		const BYTE* PTS() const; 
		const BYTE* DTS() const; 

		// Methods
		bool hasPacketStartCodePrefix() const;
		void reset();

		double ptsInSeconds() const;
		double dtsInSeconds() const;

		UINT64 pts() const;
		UINT64 dts() const;

	private:
		class Impl;
		std::unique_ptr<Impl> _pimpl;
	};

}


