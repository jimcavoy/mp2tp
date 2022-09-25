#include "stdafx.h"
#include "tspes.h"

#ifndef WIN32
#include <memory.h>
#include <arpa/inet.h>
#endif

#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif

/////////////////////////////////////////////////////////////////////////////
namespace
{
	const BYTE PTS_DTS_MASK = 0xC0;
};

namespace lcss
{
	class PESPacket::Impl
	{
	public:
		Impl() {};

		UINT16 parsePTS(const BYTE* stream)
		{
			UINT16 cur = 0;
			UINT16 value = (flags2_ & PTS_DTS_MASK);
			if (value > 0)
			{
				if (value == 0xC0)
				{
					memcpy(PTS_, stream, 5);
					cur += 5;

					memcpy(DTS_, stream + cur, 5);
					cur += 5;
				}
				else if (value == 0x80)
				{
					memcpy(PTS_, stream, 5);
					cur += 5;
				}
			}
			return cur;
		}

	public:
		BYTE packet_start_code_prefix_[3]{};
		BYTE stream_id_{};
		UINT16 PES_packet_length_{};
		BYTE flags1_{};
		BYTE flags2_{};
		BYTE PES_header_data_length_{};
		BYTE PTS_[5]{};
		BYTE DTS_[5]{};
	};
}

/////////////////////////////////////////////////////////////////////////////

lcss::PESPacket::PESPacket()
{
	_pimpl = std::make_unique<lcss::PESPacket::Impl>();
}

lcss::PESPacket::~PESPacket()
{

}

lcss::PESPacket::PESPacket(const lcss::PESPacket& other)
	:_pimpl(std::make_unique<lcss::PESPacket::Impl>(*other._pimpl))
{

}

lcss::PESPacket& lcss::PESPacket::operator=(const PESPacket& rhs)
{
	if (this != &rhs)
	{
		_pimpl.reset(new lcss::PESPacket::Impl(*rhs._pimpl));
	}
	return *this;
}

lcss::PESPacket::PESPacket(lcss::PESPacket&& other) noexcept
{
	*this = std::move(other);
}

lcss::PESPacket& lcss::PESPacket::operator=(PESPacket&& rhs) noexcept
{
	if (this != &rhs)
	{
		_pimpl = std::move(rhs._pimpl);
	}
	return *this;
}

UINT16 lcss::PESPacket::parse(const BYTE* stream)
{
	UINT16 cur = 0;
	if (_pimpl->stream_id_ == 0)
	{
		memcpy(_pimpl->packet_start_code_prefix_, stream, 3);
		cur += 3; // skip the PES start markers which is 3 bytes;

		if (!hasPacketStartCodePrefix())
			return 0;

		memcpy(&(_pimpl->stream_id_), stream + cur, 1);
		cur++;

		UINT16 iValue{};
		memcpy(&iValue, stream + cur, 2);
		_pimpl->PES_packet_length_ = ntohs(iValue);
		cur += 2;

		if (_pimpl->stream_id_ != 188		// program stream map
			&& _pimpl->stream_id_ != 190	// padding stream
			&& _pimpl->stream_id_ != 191	// private stream 2
			&& _pimpl->stream_id_ != 240	// ECM
			&& _pimpl->stream_id_ != 241	// EMM
			&& _pimpl->stream_id_ != 255	// program stream directory
			&& _pimpl->stream_id_ != 242	// DSMCC stream
			&& _pimpl->stream_id_ != 248	// ITU-T Rec. H.222.1 type E stream
			)
		{
			memcpy(&(_pimpl->flags1_), stream + cur, 1);
			cur++;

			memcpy(&(_pimpl->flags2_), stream + cur, 1);
			cur++;

			memcpy(&(_pimpl->PES_header_data_length_), stream + cur, 1);
			cur++;
			int curPlusHeaderLen = cur + _pimpl->PES_header_data_length_;

			UINT16 ptsflag = (_pimpl->flags2_ & PTS_DTS_MASK);
			if (ptsflag > 0)
			{
				cur += _pimpl->parsePTS(stream + cur);
			}

			// Check for stuffing bytes and skip over them
			cur += (curPlusHeaderLen - cur);
		}
	}
	return cur;
}

bool lcss::PESPacket::hasPacketStartCodePrefix() const
{
	if (_pimpl->packet_start_code_prefix_[0] == 0
		&& _pimpl->packet_start_code_prefix_[1] == 0
		&& _pimpl->packet_start_code_prefix_[2] == 1)
		return true;
	return false;
}

void lcss::PESPacket::reset()
{
	_pimpl->stream_id_ = 0;
}

/// <summary>
/// Presentation Time Stamp (PTS) in seconds.
/// </summary>
/// <returns></returns>
double lcss::PESPacket::ptsInSeconds() const
{
	double t{0.0};

	UINT16 value = (_pimpl->flags2_ & PTS_DTS_MASK);
	if (value == 0xC0 || value == 0x80)
	{
		t = pts() / 90000.0;
	}
	return t;
}

/// <summary>
/// Decoding Time Stamp (DTS) in seconds.
/// </summary>
/// <returns></returns>
double lcss::PESPacket::dtsInSeconds() const
{
	double t{0.0};

	UINT16 value = (_pimpl->flags2_ & PTS_DTS_MASK);
	if (value == 0xC0)
	{
		t = dts() / 90000.0;
	}
	return t;
}

/// <summary>
/// Presentation Time Stamp (PTS) in units of 90 kHz.
/// </summary>
/// <returns></returns>
UINT64 lcss::PESPacket::pts() const
{
	UINT64 PTS = 0;

	UINT16 value = (_pimpl->flags2_ & PTS_DTS_MASK);
	if (value == 0xC0 || value == 0x80)
	{
		BYTE PTS1 = (_pimpl->PTS_[0] & 0x0e);
		UINT16 PTS2{};
		memcpy(&PTS2, _pimpl->PTS_ + 1, 2);
		PTS2 = ntohs(PTS2);
		UINT16 PTS3{};
		memcpy(&PTS3, _pimpl->PTS_ + 3, 2);
		PTS3 = ntohs(PTS3);

		PTS = PTS1 << 29 |
			((PTS2) >> 1) << 15 |
			(PTS3) >> 1;

		PTS = PTS & 0x1FFFFFFFF;
	}
	return PTS;
}

const BYTE* lcss::PESPacket::PTS() const
{
	return _pimpl->PTS_;
}

/// <summary>
/// Decoding Time Stamp (DTS) in units of 90 kHz.
/// </summary>
/// <returns></returns>
UINT64 lcss::PESPacket::dts() const
{
	UINT64 DTS = 0;

	UINT16 value = (_pimpl->flags2_ & PTS_DTS_MASK);
	if (value == 0xC0)
	{
		BYTE DTS1 = (_pimpl->DTS_[0] & 0x0e);
		UINT16 DTS2{};
		memcpy(&DTS2, _pimpl->DTS_ + 1, 2);
		DTS2 = ntohs(DTS2);
		UINT16 DTS3{};
		memcpy(&DTS3, _pimpl->DTS_ + 3, 2);
		DTS3 = ntohs(DTS3);

		DTS = DTS1 << 29 |
			((DTS2) >> 1) << 15 |
			(DTS3) >> 1;

		DTS = DTS & 0x1FFFFFFFF;
	}
	return DTS;
}

const BYTE* lcss::PESPacket::DTS() const
{
	return _pimpl->DTS_;
}

BYTE lcss::PESPacket::stream_id() const
{
	return _pimpl->stream_id_;
}

UINT16 lcss::PESPacket::packet_length() const
{
	return _pimpl->PES_packet_length_;
}

BYTE lcss::PESPacket::flags1() const
{
	return _pimpl->flags1_;
}

BYTE lcss::PESPacket::flags2() const
{
	return _pimpl->flags2_;
}

BYTE lcss::PESPacket::header_data_length() const
{
	return _pimpl->PES_header_data_length_;
}