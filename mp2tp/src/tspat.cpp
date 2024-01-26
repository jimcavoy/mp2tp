#include "stdafx.h"
#include <mp2tp/tspat.h>

#include <iterator>
#include <vector>

#ifndef WIN32
#include <memory.h>
#include <arpa/inet.h>
#endif

using namespace std;

const UINT16 PAT_SECTION_LEN = 0x0FFF;
const BYTE PAT_VERSION_NO = 0x3E;
const BYTE PAT_CUR_NEXT_INDICATOR = 0x01;
const UINT16 PAT_ENTRY_PID = 0x1FFF;

extern uint32_t dvbpsi_crc32_table[256];

namespace lcss
{
	class ProgramAssociationTable::Impl
	{
	public:
		Impl() 
			: reserved_(3)
			, version_number_(0)
			, current_next_indicator_(0)
		{}
		Impl(const Impl& other)
			:reserved_(3)
			, pointer_field_(other.pointer_field_)
			, table_id_(other.table_id_)
			, section_length_(other.section_length_)
			, transport_stream_id_(other.transport_stream_id_)
			, version_number_(other.version_number_)
			, current_next_indicator_(other.current_next_indicator_)
			, section_number_(other.section_number_)
			, last_section_number_(other.last_section_number_)
			, CRC_32_(other.CRC_32_)
		{
			pid_to_program_.insert(other.pid_to_program_.begin(), other.pid_to_program_.end());
		}

	public:
		void calcCRC(const lcss::ProgramAssociationTable& pat);

	public:
		BYTE pointer_field_{0};
		BYTE table_id_{0};
		UINT16 section_length_{13};
		UINT16 transport_stream_id_{1};
		BYTE reserved_ : 2;
		BYTE version_number_ : 5;
		BYTE current_next_indicator_ : 1;
		BYTE section_number_{};
		BYTE last_section_number_{};
		UINT32 CRC_32_{};
		ProgramAssociationTable::MapType pid_to_program_;
	};
}

void lcss::ProgramAssociationTable::Impl::calcCRC(const lcss::ProgramAssociationTable& pat)
{
	std::vector<BYTE> rawByteSeq;
	pat.serialize(std::back_inserter(rawByteSeq));
	CRC_32_ = 0xffffffff;

	for (size_t i = 1; i < rawByteSeq.size() - 4; i++)
	{
		CRC_32_ = (CRC_32_ << 8) ^ dvbpsi_crc32_table[(CRC_32_ >> 24) ^ (rawByteSeq[i])];
	}
}


lcss::ProgramAssociationTable::ProgramAssociationTable()
{
	_pimpl = std::make_unique<lcss::ProgramAssociationTable::Impl>();
}

lcss::ProgramAssociationTable::~ProgramAssociationTable()
{

}

lcss::ProgramAssociationTable::ProgramAssociationTable(const ProgramAssociationTable& other)
	:_pimpl(std::make_unique<lcss::ProgramAssociationTable::Impl>(*other._pimpl))
{

}

lcss::ProgramAssociationTable& lcss::ProgramAssociationTable::operator=(const ProgramAssociationTable& rhs)
{
	if (this != &rhs)
	{
		_pimpl.reset(new lcss::ProgramAssociationTable::Impl(*rhs._pimpl));
	}
	return *this;
}

lcss::ProgramAssociationTable::ProgramAssociationTable(ProgramAssociationTable&& other) noexcept
{
	*this = std::move(other);
}

lcss::ProgramAssociationTable& lcss::ProgramAssociationTable::operator=(ProgramAssociationTable&& rhs) noexcept
{
	if (this != &rhs)
	{
		_pimpl = std::move(rhs._pimpl);
	}
	return *this;
}


void lcss::ProgramAssociationTable::parse(const BYTE* table)
{
	_pimpl->pointer_field_ = table[0];

	UINT32 offset = _pimpl->pointer_field_ == 0? 1 : _pimpl->pointer_field_;
	UINT32 cur = offset;

	_pimpl->table_id_ = *(table+cur);
	cur += 1;

	UINT16 value{};
	memcpy(&value,table+cur,2);
	_pimpl->section_length_ = (ntohs(value) & PAT_SECTION_LEN);
	cur += 2;

	memcpy(&value,table+cur,2);
	_pimpl->transport_stream_id_ = ntohs(value);
	cur += 2;

	_pimpl->version_number_ = *(table+cur) & PAT_VERSION_NO;
	_pimpl->current_next_indicator_ = *(table+cur) & PAT_CUR_NEXT_INDICATOR;
	cur++;

	_pimpl->section_number_ = *(table+cur);
	cur++;

	_pimpl->last_section_number_ = *(table+cur);
	cur++;

	UINT32 last = cur + _pimpl->section_length_ - 9;
	// Iterate over all the sections
	while(cur < last)
	{
		UINT16 pid{};
		UINT16 program{};
		memcpy(&program,table+cur,2);
		cur += 2;
		program = ntohs(program);

		memcpy(&pid,table+cur,2);
		cur +=2;
		pid = (ntohs(pid)) & PAT_ENTRY_PID;

		ProgramAssociationTable::iterator it = _pimpl->pid_to_program_.find(pid);

		if(it == _pimpl->pid_to_program_.end())
			_pimpl->pid_to_program_.insert(MapType::value_type(pid,program));
	}

	// Get CRC
	UINT32 crc{};
	memcpy(&crc,table+cur,4);
	_pimpl->CRC_32_ = ntohl(crc);
}

lcss::ProgramAssociationTable::MapType::iterator lcss::ProgramAssociationTable::addProgram(uint16_t program, uint16_t pid)
{
	MapType::iterator it = end();

	std::pair<MapType::iterator, bool> ret = _pimpl->pid_to_program_.insert(MapType::value_type(pid, program));

	if (ret.second)
	{
		it = ret.first;
		_pimpl->calcCRC(*this);
	}
	return it;
}

lcss::ProgramAssociationTable::iterator lcss::ProgramAssociationTable::begin()
{
	return _pimpl->pid_to_program_.begin();
}

lcss::ProgramAssociationTable::iterator lcss::ProgramAssociationTable::end()
{
	return _pimpl->pid_to_program_.end();
}

lcss::ProgramAssociationTable::const_iterator lcss::ProgramAssociationTable::begin() const
{
	return _pimpl->pid_to_program_.begin();
}

lcss::ProgramAssociationTable::const_iterator lcss::ProgramAssociationTable::end() const
{
	return _pimpl->pid_to_program_.end();
}


lcss::ProgramAssociationTable::iterator lcss::ProgramAssociationTable::find(uint16_t pid)
{
	return _pimpl->pid_to_program_.find(pid);
}

size_t lcss::ProgramAssociationTable::size() const
{
	return _pimpl->pid_to_program_.size();
}

BYTE lcss::ProgramAssociationTable::pointer_field() const
{
	return _pimpl->pointer_field_;
}

BYTE lcss::ProgramAssociationTable::table_id() const
{
	return _pimpl->table_id_;
}

UINT16 lcss::ProgramAssociationTable::section_length() const
{
	return _pimpl->section_length_;
}

UINT16 lcss::ProgramAssociationTable::transport_stream_id() const
{
	return _pimpl->transport_stream_id_;
}

BYTE lcss::ProgramAssociationTable::version_number() const
{
	return _pimpl->version_number_;
}

bool lcss::ProgramAssociationTable::current_next_indicator() const
{
	return _pimpl->current_next_indicator_;
}

BYTE lcss::ProgramAssociationTable::section_number() const
{
	return _pimpl->section_number_;
}

BYTE lcss::ProgramAssociationTable::last_section_number() const
{
	return _pimpl->last_section_number_;
}

UINT32 lcss::ProgramAssociationTable::CRC_32() const
{
	return  _pimpl->CRC_32_;
}

template<typename BackInsertIter>
void lcss::ProgramAssociationTable::serialize(BackInsertIter backit) const
{
	uint8_t buf[2]{};
	*backit++ = pointer_field();
	*backit++ = table_id();

	uint16_t sz  = (uint16_t)size();

	uint16_t section_len = 9 + (sz * 4);
	section_len = htons(section_len);
	memcpy(buf, &section_len, 2);
	buf[0] |= 0xB0;

	*backit++ = buf[0];
	*backit++ = buf[1];

	memcpy(buf, &_pimpl->transport_stream_id_, 2);

	*backit++ = buf[1];
	*backit++ = buf[0];
	*backit++ = 0xC1;
	*backit++ = section_number();
	*backit++ = last_section_number();

	for (auto p : _pimpl->pid_to_program_)
	{
		memcpy(buf, &p.second, 2);
		*backit++ = buf[1];
		*backit++ = buf[0];
		memcpy(buf, &p.first, 2);
		*backit++ = buf[1] | 0xE0;
		*backit++ = buf[0];
	}

	*backit++ = (_pimpl->CRC_32_ >> 24) & 0xff;
	*backit++ = (_pimpl->CRC_32_ >> 16) & 0xff;
	*backit++ = (_pimpl->CRC_32_ >> 8) & 0xff;
	*backit++ = (_pimpl->CRC_32_) & 0xff;
}

template void lcss::ProgramAssociationTable::serialize<std::back_insert_iterator<std::vector<BYTE>>>(std::back_insert_iterator<std::vector<BYTE>>) const;