#include "stdafx.h"
#include "tspckt.h"

#include "tsadptfd.h"

#include <algorithm>
#include <iterator>
#include <array>
#include <cassert>

#ifdef _DEBUG_NEW
#define new DEBUG_CLIENTBLOCK
#endif

#ifndef WIN32
#include <memory.h>
#endif

// Transport Headers
const BYTE TRANSPORT_ERROR_MASK = 0x80;
const BYTE PAYLOAD_UNIT_START_MASK = 0x40;
const BYTE TRANSPORT_PRI_MASK = 0x20;

const UINT16 TP_PID = 0x1FFF;
const BYTE TP_SCRAMBLLING_CTRL = 0xC0;
const BYTE TP_ADAPTATION_FD_CTRL = 0x30;
const BYTE TP_CONTINUITY_COUNTER = 0x0F;

namespace 
{
	BYTE continuity_value[] = {
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
		0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
	};

	BYTE continuity_value_adaptation[] = {
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
		0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F
	};
}

namespace lcss
{
	class TransportPacket::Impl
	{
	public:
		Impl() 
		{
			_data.fill(0xFF);
		}
		Impl(const Impl& other)
			:_pos(other._pos)
		{
			std::copy(other._data.begin(), other._data.end(), _data.begin());
		}
		~Impl() {}

		void insert(const BYTE* data, size_t len)
		{
			assert(len == TransportPacket::TS_SIZE);
			clear();
			std::copy(data, data+len, _data.begin());
			_pos = (int)len;
		}

		void clear()
		{
			_pos = 0;
		}

		void push_back(BYTE d)
		{
			if (_pos < TransportPacket::TS_SIZE)
			{
				_data[_pos++] = d;
			}
		}

	public:
		int _pos{ 0 };
		std::array<BYTE, TransportPacket::TS_SIZE> _data{ };
		lcss::AdaptationField _adptFd;
	};

	std::ostream& operator<<(std::ostream& os, const lcss::TransportPacket& packet)
	{
		for (auto c : packet._pimpl->_data)
		{
			os << c;
		}
		return os;
	}
}

/////////////////////////////////////////////////////////////////////////////
// TransportPacket

lcss::TransportPacket::TransportPacket()
{
	_pimpl = std::make_unique<lcss::TransportPacket::Impl>();
}

lcss::TransportPacket::TransportPacket(const BYTE* data)
{
	_pimpl = std::make_unique<lcss::TransportPacket::Impl>();
	_pimpl->insert(data, TransportPacket::TS_SIZE);
}

lcss::TransportPacket::TransportPacket(const BYTE* data, size_t len)
{
	_pimpl = std::make_unique<lcss::TransportPacket::Impl>();
	_pimpl->insert(data, TransportPacket::TS_SIZE);
}

lcss::TransportPacket::~TransportPacket()
{

}

lcss::TransportPacket::TransportPacket(const TransportPacket& src)
	:_pimpl(std::make_unique<lcss::TransportPacket::Impl>(*src._pimpl))
{

}

lcss::TransportPacket& lcss::TransportPacket::operator=(const TransportPacket& rhs)
{
	if (this != &rhs)
	{
		_pimpl.reset(new lcss::TransportPacket::Impl(*rhs._pimpl));
	}
	return *this;
}

lcss::TransportPacket::TransportPacket(TransportPacket&& src) noexcept
{
	*this = std::move(src);
}

lcss::TransportPacket& lcss::TransportPacket::operator=(TransportPacket&& rhs) noexcept
{
	if (this != &rhs)
	{
		_pimpl = std::move(rhs._pimpl);
	}
	return *this;
}

bool lcss::TransportPacket::TEI() const
{
	return _pimpl->_data[1] & TRANSPORT_ERROR_MASK ? true : false;
}

bool lcss::TransportPacket::payloadUnitStart() const
{
	return _pimpl->_data[1] & PAYLOAD_UNIT_START_MASK ? true : false;
}

bool lcss::TransportPacket::transportPriority() const
{
	return _pimpl->_data[1] & TRANSPORT_PRI_MASK ? true : false;
}

uint16_t lcss::TransportPacket::PID() const
{
	char pid[2]{};
	pid[0] = _pimpl->_data[2];
	pid[1] = _pimpl->_data[1];
	UINT16 npid;
	memcpy(&npid, pid, 2);

	return npid & TP_PID;
}

uint8_t lcss::TransportPacket::scramblingControl() const
{
	return (_pimpl->_data[3] & TP_SCRAMBLLING_CTRL) >> 6;
}

uint8_t lcss::TransportPacket::adaptationFieldExist() const
{
	return (_pimpl->_data[3] & TP_ADAPTATION_FD_CTRL) >> 4;
}

uint8_t lcss::TransportPacket::cc() const
{
	return _pimpl->_data[3] & TP_CONTINUITY_COUNTER;
}

uint8_t lcss::TransportPacket::incrementCC()
{
	uint8_t afc = adaptationFieldExist();

	switch (afc)
	{
	case 0x01: // 01 No adaptation field, payload only
		_pimpl->_data[3] = continuity_value[(cc() + 1) % 16];
		break;
	default:
		_pimpl->_data[3] = continuity_value_adaptation[(cc() + 1) % 16];
	}

	return cc();
}

unsigned char lcss::TransportPacket::data_byte() const
{
	uint8_t db = 0;
	uint8_t afc = adaptationFieldExist();

	switch (afc)
	{
	case 0x01: //01
		db = 184;
		break;
	case 0x02: //10
		db = 183;
		break;
	case 0x03: //11
	{
		short len = getAdaptationField()->length();
		if (len >= 0 && len <= 182)
			db = 184 - len - 1;
		else
			db = 182;
	}
	break;
	}
	return db;
}


const lcss::AdaptationField* lcss::TransportPacket::getAdaptationField() const
{
	char flag = adaptationFieldExist();
	bool hasAdaptationField = flag == 0x02 || flag == 0x03 ? true : false;
	if (hasAdaptationField)
	{
		_pimpl->_adptFd.parse(_pimpl->_data.data());
		return &_pimpl->_adptFd;
	}
	return nullptr;
}

void lcss::TransportPacket::getData(BYTE* buffer, int len) const
{
	BYTE dataByte = data_byte();
	int start = TS_SIZE - dataByte;
	if (start > 0)
	{
		std::array<BYTE, TransportPacket::TS_SIZE>::iterator first = _pimpl->_data.begin();
		std::advance(first, start);
#ifdef WIN32
		std::copy(first, _pimpl->_data.end(),
			stdext::checked_array_iterator<BYTE*>(buffer, len));
#else
		int i = 0;
		for (auto it = first; it != _pimpl->_data.end(); ++it, i++)
		{
			if (i < len)
			{
				buffer[i] = *it;
			}
		}
#endif
	}
}

const BYTE* lcss::TransportPacket::getData() const
{
	BYTE dataByte = data_byte();
	int start = TS_SIZE - dataByte;

	if (start > 0)
	{
		return _pimpl->_data.data() + start;
	}

	return nullptr;
}

size_t lcss::TransportPacket::length() const
{
	return _pimpl->_pos;
}

void lcss::TransportPacket::serialize(BYTE* data, int len) const
{
#ifdef WIN32
	std::copy(_pimpl->_data.begin(), _pimpl->_data.end(),
		stdext::checked_array_iterator<BYTE*>(data, len));
#else
	int i = 0;
	for (auto it = _pimpl->_data.begin(); it != _pimpl->_data.end(); ++it, i++)
	{
		if (i < len)
		{
			data[i] = *it;
		}
	}
#endif
}

void lcss::TransportPacket::parse(const BYTE* buf)
{
	_pimpl->insert(buf, TransportPacket::TS_SIZE);
}

void lcss::TransportPacket::push_back(BYTE b)
{
	_pimpl->push_back(b);
}

const BYTE* lcss::TransportPacket::data() const
{
	return _pimpl->_data.data();
}
