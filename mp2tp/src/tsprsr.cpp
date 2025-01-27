#include "stdafx.h"
#include <mp2tp/tsprsr.h>
#include <mp2tp/tspckt.h>

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
        TransportPacket _tspckt;
        UINT16	_cursor{};
        UINT64	_count{};
        UINT32	_packetSize{ lcss::TransportPacket::TS_SIZE };

        std::vector<uint8_t> _buffer{};
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

bool lcss::TSParser::parse(const BYTE* stream, UINT32 len)
{
    UINT32 i = 0;
    bool result = true;
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
            else if (_pimpl->_buffer.size() > 0 && _pimpl->_buffer.size() != lcss::TransportPacket::TS_SIZE)
            {
                while (_pimpl->_buffer.size() < lcss::TransportPacket::TS_SIZE)
                {
                    _pimpl->_buffer.push_back(stream[i++]);
                }
            }
            else
            {
                i++;
            }
        }
        // The TS packet is contain in the stream buffer
        else if (i + lcss::TransportPacket::TS_SIZE <= len)
        {
            // Fail to find a sync byte after reading in a byte length of at least
            // two TS packets (2 * 188 bytes), return false.
            if (_pimpl->_count == 0 && 376 < i)
            {
                return false;
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
        // The TS packet spans across two stream buffers
        else if (i + lcss::TransportPacket::TS_SIZE > len)
        {
            while (i < len)
                _pimpl->_buffer.push_back(stream[i++]);
        }
    }
    return result;
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