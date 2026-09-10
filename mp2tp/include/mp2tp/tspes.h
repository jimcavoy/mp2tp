#pragma once

#include <cstdint>
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

		uint16_t parse(const uint8_t* stream);

		// PES packet fields
		uint8_t stream_id() const;
		uint16_t packet_length() const;
		uint8_t flags1() const;
		uint8_t flags2() const;
		uint8_t header_data_length() const;
		const uint8_t* PTS() const; 
		const uint8_t* DTS() const; 

		// Methods
		bool hasPacketStartCodePrefix() const;
		void reset();

		double ptsInSeconds() const;
		double dtsInSeconds() const;

		uint64_t pts() const;
		uint64_t dts() const;

		void setPTS(uint8_t* pts);
		void setDTS(uint8_t* dts);

		void setPTS(uint64_t pts);
		void setDTS(uint64_t dts);

		void serialize(uint8_t* stream);

	private:
		class Impl;
		std::unique_ptr<Impl> _pimpl;
	};

}


