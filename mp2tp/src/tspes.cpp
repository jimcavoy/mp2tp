#include "tspes.h"

#ifdef WIN32
#include <WinSock2.h>
#else
#include <memory.h>
#include <arpa/inet.h>
#endif

#include <bitset>

/////////////////////////////////////////////////////////////////////////////
namespace
{
    const uint8_t PTS_DTS_MASK = 0xC0;

    void reverse(uint8_t arr[], int n)
    {
        for (int low = 0, high = n - 1; low < high; low++, high--) {
            std::swap(arr[low], arr[high]);
        }
    }

    void encodeTimestamp(uint8_t* outTs, uint64_t ts)
    {
        uint8_t buf[8]{};
        memcpy(buf, &ts, 8);

        uint16_t PTS3{ 0 };
        memcpy(&PTS3, buf, 2);
        std::bitset<16> bsPTS3(PTS3);

        uint16_t PTS2{ 0 };
        memcpy(&PTS2, buf + 2, 2);
        std::bitset<16> bsPTS2(PTS2);

        uint8_t PTS1 = buf[4];
        std::bitset<8> bsPTS1(PTS1);

        std::bitset<64> bsPTS;
        bsPTS[0] = 1;
        for (int i = 0; i < 16; i++)
        {
            if (i < 15) {
                bsPTS[i + 1] = bsPTS3[i];
            }
            else {
                bsPTS[i + 1] = 1;
                bsPTS[i + 2] = bsPTS3[i];
            }
        }

        for (int i = 0; i < 16; i++)
        {
            if (i < 15) {
                bsPTS[i + 18] = bsPTS2[i];
            }
            else {
                bsPTS[i + 17] = 1;
                bsPTS[i + 19] = bsPTS2[i];
            }
        }

        for (int i = 0; i < 3; i++)
        {
            bsPTS[i + 35] = bsPTS1[i];
        }
        bsPTS[37] = 1;

        uint64_t nPts = bsPTS.to_ullong();

        memcpy(outTs, &nPts, 5);
        reverse(outTs, 5);
    }
};

namespace lcss
{
    /// @brief PESPacket private implementation class
    class PESPacket::Impl
    {
    public:
        Impl() {};

        uint16_t parsePTS(const uint8_t* stream)
        {
            uint16_t cur = 0;
            uint16_t value = (flags2_ & PTS_DTS_MASK);
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
        uint8_t packet_start_code_prefix_[3]{};
        uint8_t stream_id_{};
        uint16_t PES_packet_length_{};
        uint8_t flags1_{};
        uint8_t flags2_{};
        uint8_t PES_header_data_length_{};
        uint8_t PTS_[5]{};
        uint8_t DTS_[5]{};
    };
}

/////////////////////////////////////////////////////////////////////////////

lcss::PESPacket::PESPacket()
    : _pimpl(std::make_unique<lcss::PESPacket::Impl>())
{

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

/// @brief Parse the @p stream so you can access PESPacket instance's field.
/// @param stream [in] The sequence of bytes containing PES packet header.  
/// The stream is retrieved from a TransportPacket payload data where the 
/// TransportPacket::payloadUnitStart is true.
/// @return The number of bytes read when parsing the PES packet.
uint16_t lcss::PESPacket::parse(const uint8_t* stream)
{
    uint16_t cur = 0;
    if (_pimpl->stream_id_ == 0)
    {
        memcpy(_pimpl->packet_start_code_prefix_, stream, 3);
        cur += 3; // skip the PES start markers which is 3 bytes;

        if (!hasPacketStartCodePrefix())
            return 0;

        memcpy(&(_pimpl->stream_id_), stream + cur, 1);
        cur++;

        uint16_t iValue{};
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

            uint16_t ptsflag = (_pimpl->flags2_ & PTS_DTS_MASK);
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

/// @brief Query if the PESPacket has a start code prefix.
/// @return Returns true if the PESPacket has a start code prefix, otherwise false.
bool lcss::PESPacket::hasPacketStartCodePrefix() const
{
    if (_pimpl->packet_start_code_prefix_[0] == 0
        && _pimpl->packet_start_code_prefix_[1] == 0
        && _pimpl->packet_start_code_prefix_[2] == 1)
        return true;
    return false;
}

/// @brief Reset the PESPacket stream id.
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
    double t{ 0.0 };

    uint16_t value = (_pimpl->flags2_ & PTS_DTS_MASK);
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
    double t{ 0.0 };

    uint16_t value = (_pimpl->flags2_ & PTS_DTS_MASK);
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
uint64_t lcss::PESPacket::pts() const
{
    uint64_t PTS = 0;

    uint16_t value = (_pimpl->flags2_ & PTS_DTS_MASK);
    if (value == 0xC0 || value == 0x80)
    {
        uint8_t PTS1 = (_pimpl->PTS_[0] & 0x0e);
        uint16_t PTS2{};
        memcpy(&PTS2, _pimpl->PTS_ + 1, 2);
        PTS2 = ntohs(PTS2);
        uint16_t PTS3{};
        memcpy(&PTS3, _pimpl->PTS_ + 3, 2);
        PTS3 = ntohs(PTS3);

        PTS = PTS1 << 29 |
            ((PTS2) >> 1) << 15 |
            (PTS3) >> 1;

        PTS = PTS & 0x1FFFFFFFF;
    }
    return PTS;
}

const uint8_t* lcss::PESPacket::PTS() const
{
    return _pimpl->PTS_;
}

/// <summary>
/// Decoding Time Stamp (DTS) in units of 90 kHz.
/// </summary>
/// <returns></returns>
uint64_t lcss::PESPacket::dts() const
{
    uint64_t DTS = 0;

    uint16_t value = (_pimpl->flags2_ & PTS_DTS_MASK);
    if (value == 0xC0)
    {
        uint8_t DTS1 = (_pimpl->DTS_[0] & 0x0e);
        uint16_t DTS2{};
        memcpy(&DTS2, _pimpl->DTS_ + 1, 2);
        DTS2 = ntohs(DTS2);
        uint16_t DTS3{};
        memcpy(&DTS3, _pimpl->DTS_ + 3, 2);
        DTS3 = ntohs(DTS3);

        DTS = DTS1 << 29 |
            ((DTS2) >> 1) << 15 |
            (DTS3) >> 1;

        DTS = DTS & 0x1FFFFFFFF;
    }
    return DTS;
}

void lcss::PESPacket::setPTS(uint8_t* pts)
{
    _pimpl->PES_header_data_length_ = 0x05;
    _pimpl->flags2_ = 0x80;
    memcpy(_pimpl->PTS_, pts, 5);
}

void lcss::PESPacket::setDTS(uint8_t * dts)
{
    _pimpl->PES_header_data_length_ = 0x0A;
    _pimpl->flags2_ = 0xC0;
    memcpy(_pimpl->DTS_, dts, 5);
}

void lcss::PESPacket::setPTS(uint64_t pts)
{
    _pimpl->PES_header_data_length_ = 0x05;
    _pimpl->flags2_ = 0x80;
    encodeTimestamp(_pimpl->PTS_, pts);
}

void lcss::PESPacket::setDTS(uint64_t dts)
{
    _pimpl->PES_header_data_length_ = 0x0A;
    _pimpl->flags2_ = 0xC0;
    encodeTimestamp(_pimpl->DTS_, dts);
}

void lcss::PESPacket::serialize(uint8_t* stream)
{
    stream[0] = _pimpl->packet_start_code_prefix_[0];
    stream[1] = _pimpl->packet_start_code_prefix_[1];
    stream[2] = _pimpl->packet_start_code_prefix_[2];
    stream[3] = _pimpl->stream_id_;
    uint8_t buf[2];
    uint16_t len = htons(_pimpl->PES_packet_length_);
    memcpy(buf, &len, 2);
    stream[4] = buf[0];
    stream[5] = buf[1];
    stream[6] = _pimpl->flags1_;
    stream[7] = _pimpl->flags2_;
    stream[8] = _pimpl->PES_header_data_length_;
    uint16_t value = _pimpl->flags2_ & PTS_DTS_MASK;
    if (value > 0)
    {
        if (value == 0xC0)
        {
            stream[9] = _pimpl->PTS_[0];
            stream[10] = _pimpl->PTS_[1];
            stream[11] = _pimpl->PTS_[2];
            stream[12] = _pimpl->PTS_[3];
            stream[13] = _pimpl->PTS_[4];
            stream[14] = _pimpl->DTS_[0];
            stream[15] = _pimpl->DTS_[1];
            stream[16] = _pimpl->DTS_[2];
            stream[17] = _pimpl->DTS_[3];
            stream[18] = _pimpl->DTS_[4];
        }
        else if (value == 0x80)
        {
            stream[9] = _pimpl->PTS_[0];
            stream[10] = _pimpl->PTS_[1];
            stream[11] = _pimpl->PTS_[2];
            stream[12] = _pimpl->PTS_[3];
            stream[13] = _pimpl->PTS_[4];
        }
    }
}

const uint8_t* lcss::PESPacket::DTS() const
{
    return _pimpl->DTS_;
}

/// @brief Get the elementary stream type.
/// @return Returns the elementary stream type.
uint8_t lcss::PESPacket::stream_id() const
{
    return _pimpl->stream_id_;
}

/// @brief Get the number of bytes in the PESPacket following the last byte of the field.
/// @return The PESPacket size in bytes.  Zero is allowed for unbounded sequence but only for 
/// video elementary streams.
uint16_t lcss::PESPacket::packet_length() const
{
    return _pimpl->PES_packet_length_;
}

uint8_t lcss::PESPacket::flags1() const
{
    return _pimpl->flags1_;
}

uint8_t lcss::PESPacket::flags2() const
{
    return _pimpl->flags2_;
}

uint8_t lcss::PESPacket::header_data_length() const
{
    return _pimpl->PES_header_data_length_;
}