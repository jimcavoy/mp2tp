#include "stdafx.h"
#include "tspckt.h"

#include "tsadptfd.h"

#include <iterator>

#ifdef _DEBUG
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

/////////////////////////////////////////////////////////////////////////////
// TransportPacket

lcss::TransportPacket::TransportPacket()
{
	_data.resize(TS_SIZE, '\0');
}

lcss::TransportPacket::TransportPacket(const BYTE* data)
{
	_data.insert(_data.end(), &data[0], &data[TS_SIZE]);
}

lcss::TransportPacket::TransportPacket(const BYTE* data, size_t len)
{
	_data.insert(_data.end(), &data[0], &data[len]);
}

lcss::TransportPacket::~TransportPacket()
{

}

lcss::TransportPacket::TransportPacket(const TransportPacket & src)
{
	std::copy(src._data.begin(), src._data.end(), std::back_inserter(_data));
}

lcss::TransportPacket & lcss::TransportPacket::operator=(const TransportPacket & rhs)
{
	TransportPacket temp(rhs);
	swap(temp);

	return *this;
}

lcss::TransportPacket::TransportPacket(TransportPacket && src) noexcept
{
	// Eliminate redundant code by writing the move constructor to call the
	// move assignment operator.
	*this = std::move(src);
}

lcss::TransportPacket & lcss::TransportPacket::operator=(TransportPacket && rhs) noexcept
{
	if (this != &rhs)
	{
		_data = std::move(rhs._data);
		_adptFd.swap(rhs._adptFd);
	}
	return *this;
}

void lcss::TransportPacket::swap(TransportPacket & src)
{
	_adptFd.swap(src._adptFd);
	_data.swap(src._data);
}

bool lcss::TransportPacket::TEI() const
{
	return _data[1] & TRANSPORT_ERROR_MASK ? true : false;
}

bool lcss::TransportPacket::payloadUnitStart() const
{
	return _data[1] & PAYLOAD_UNIT_START_MASK ? true : false;
}

bool lcss::TransportPacket::transportPriority() const
{
	return _data[1] & TRANSPORT_PRI_MASK ? true : false;
}

UINT16 lcss::TransportPacket::PID() const
{
	char pid[2];
	pid[0] = _data[2];
	pid[1] = _data[1];
	UINT16 npid;
	memcpy(&npid, pid, 2);

	return npid & TP_PID;
}

char lcss::TransportPacket::scramblingControl() const
{
	return (_data[3] & TP_SCRAMBLLING_CTRL) >> 6;
}

char lcss::TransportPacket::adaptationFieldExist() const
{
	return (_data[3] & TP_ADAPTATION_FD_CTRL) >> 4;
}

char lcss::TransportPacket::cc() const
{
	return _data[3] & TP_CONTINUITY_COUNTER;
}

unsigned char lcss::TransportPacket::data_byte() const
{
	unsigned char db = 0;
	char afc = adaptationFieldExist();

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
	if (_adptFd.get() == nullptr)
		_adptFd = std::make_unique<lcss::AdaptationField>(_data.data());
	return _adptFd.get();
}

void lcss::TransportPacket::getData(BYTE* buffer, int len) const
{
	BYTE dataByte = data_byte();
	int start = TS_SIZE - dataByte;
	if (start > 0)
	{
		std::vector<BYTE>::const_iterator first = _data.begin();
		std::advance(first, start);
#ifdef WIN32
		std::copy(first, _data.end(),
			stdext::checked_array_iterator<BYTE*>(buffer, len));
#else
		int i = 0;
		for (auto it = first; it != _data.end(); ++it, i++)
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
		return _data.data() + start;
	}

	return nullptr;
}

size_t lcss::TransportPacket::length() const
{
	return _data.size();
}

void lcss::TransportPacket::serialize(BYTE* data, int len) const
{
#ifdef WIN32
	std::copy(_data.begin(), _data.end(),
		stdext::checked_array_iterator<BYTE*>(data, len));
#else
	int i = 0;
	for (auto it = _data.begin(); it != _data.end(); ++it, i++)
	{
		if (i < len)
		{
			data[i] = *it;
		}
	}
#endif
}

void lcss::TransportPacket::parse(BYTE* buf)
{
	_data.clear();
	_data.insert(_data.end(), &buf[0], &buf[TS_SIZE]);
}

void lcss::TransportPacket::push_back(BYTE b)
{
	_data.push_back(b);
}

const BYTE * lcss::TransportPacket::data() const
{
	return _data.data();
}

void lcss::TransportPacket::move(std::vector<BYTE>& seq)
{
	seq = std::move(_data);
}
