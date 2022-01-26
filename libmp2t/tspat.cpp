#include "stdafx.h"
#include "tspat.h"

#ifndef WIN32
#include <memory.h>
#include <arpa/inet.h>
#endif

using namespace std;

const UINT16 PAT_SECTION_LEN = 0x0FFF;
const BYTE PAT_VERSION_NO = 0x3E;
const BYTE PAT_CUR_NEXT_INDICATOR = 0x01;
const UINT16 PAT_ENTRY_PID = 0x1FFF;

namespace lcss
{
	class ProgramAssociationTable::Impl
	{
	public:
		Impl() :reserved_(3) {}

	public:
		BYTE pointer_field_{};
		BYTE table_id_{};
		UINT16 section_length_{};
		UINT16 transport_stream_id_{};
		BYTE reserved_ : 2;
		BYTE version_number_ : 5;
		BYTE current_next_indicator_ : 1;
		BYTE section_number_{};
		BYTE last_section_number_{};
		UINT32 CRC_32_{};
		ProgramAssociationTable::MapType pid_to_program_;
	};
}


lcss::ProgramAssociationTable::ProgramAssociationTable()
{
	_pimpl = std::make_unique<lcss::ProgramAssociationTable::Impl>();
}

lcss::ProgramAssociationTable::~ProgramAssociationTable()
{

}



// Function name   : ProgramAssociationTable::parse
// Description     : This function implements definition program_association_section
//  in ISO/IEC 13818-1 : 2013(E) Table 2-30 page 47.
// Return type     : void 
// Argument        : BYTE* table
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


lcss::ProgramAssociationTable::iterator lcss::ProgramAssociationTable::find(UINT32 pid)
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
