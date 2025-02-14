#include "tsprsr.h"
#include "tspckt.h"

#include <vector>

namespace lcss
{
    class TSParser::TSParserImpl
    {
    public:
        TSParserImpl()
        {
            _buffer.reserve(lcss::TransportPacket::TS_SIZE);
        };
        ~TSParserImpl() = default;

    public:
        TransportPacket         _tspckt;
        uint64_t                _count{};
        uint32_t                _packetSize{ lcss::TransportPacket::TS_SIZE };
        std::vector<uint8_t>    _buffer{};
    };
}

/////////////////////////////////////////////////////////////////////////////
// TSParser

lcss::TSParser::TSParser()
    :_pimpl(std::make_unique<lcss::TSParser::TSParserImpl>())
{

}

lcss::TSParser::~TSParser()
{

}

bool lcss::TSParser::parse(const BYTE* stream, UINT32 len, bool strict)
{
    uint32_t h = 0;
    uint32_t i = 0;

    // The TS packet spans across two stream buffers
    if (_pimpl->_buffer.size() > 0 && _pimpl->_buffer.size() != lcss::TransportPacket::TS_SIZE)
    {
        while (_pimpl->_buffer.size() < lcss::TransportPacket::TS_SIZE)
        {
            _pimpl->_buffer.push_back(stream[i++]);
        }
    }

    while (i < len)
    {
        // Malformed stream. Not on the 188 boundary. Find the sync byte.
        if (*(stream + i) != 0x47)
        {
            // Converting AVCHD packets to TS packets
            if (_pimpl->_packetSize != lcss::TransportPacket::TS_SIZE)
            {
                i += _pimpl->_packetSize - lcss::TransportPacket::TS_SIZE;
            }
            else
            {
                i++;
            }
        }
        // The TS packet is contain in the stream buffer
        else if (i + lcss::TransportPacket::TS_SIZE <= len)
        {
            // Check to see if the sync byte falls on TS Packet size boundary
            if (strict)
            {
                if (h != 0 && 
                    h != i &&
                    i - h != _pimpl->_packetSize
                )
                {
                    return false;
                }
                h = i;
            }

            if (_pimpl->_buffer.size() == 0)
            {
                _pimpl->_buffer.insert(_pimpl->_buffer.end(), &stream[i], &stream[i + lcss::TransportPacket::TS_SIZE]);
                i += lcss::TransportPacket::TS_SIZE;
            }

            _pimpl->_tspckt.parse(_pimpl->_buffer.data());
            onPacket(_pimpl->_tspckt);
            _pimpl->_tspckt = lcss::TransportPacket();
            _pimpl->_count++;
            _pimpl->_buffer.clear();
        }
        // The TS packet spans across two buffer reads
        else if (i + lcss::TransportPacket::TS_SIZE > len)
        {
            while (i < len)
                _pimpl->_buffer.push_back(stream[i++]);
        }
    }

    if (strict)
    {
        if (_pimpl->_buffer.empty() && // TS Packet spans over two buffer reads so don't check
            i - h != _pimpl->_packetSize)
        {
            return false;
        }
    }
    return true;
}

void lcss::TSParser::onPacket(lcss::TransportPacket& pckt)
{

}

UINT64 lcss::TSParser::packetCount() const
{
    return _pimpl->_count;
}

void lcss::TSParser::setPacketSize(UINT32 sz)
{
    _pimpl->_packetSize = sz;
}