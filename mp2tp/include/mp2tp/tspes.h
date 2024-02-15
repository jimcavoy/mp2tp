#pragma once

#include "tstype.h"
#include <memory>

namespace lcss
{

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


