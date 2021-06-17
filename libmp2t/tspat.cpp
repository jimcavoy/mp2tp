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


lcss::ProgramAssociationTable::ProgramAssociationTable()
	:reserved_(0xff)
{

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
	pointer_field_ = table[0];

	UINT32 offset = pointer_field_ == 0? 1 : pointer_field_;
	UINT32 cur = offset;

	table_id_ = *(table+cur);
	cur += 1;

	UINT16 value;
	memcpy(&value,table+cur,2);
	section_length_ = (ntohs(value) & PAT_SECTION_LEN);
	cur += 2;

	memcpy(&value,table+cur,2);
	transport_stream_id_ = ntohs(value);
	cur += 2;

	version_number_ = *(table+cur) & PAT_VERSION_NO;
	current_next_indicator_ = *(table+cur) & PAT_CUR_NEXT_INDICATOR;
	cur++;

	section_number_ = *(table+cur);
	cur++;

	last_section_number_ = *(table+cur);
	cur++;

	UINT32 last = cur + section_length_ - 9;
	// Iterate over all the sections
	while(cur < last)
	{
		UINT16 pid;
		UINT16 program;
		memcpy(&program,table+cur,2);
		cur += 2;
		program = ntohs(program);

		memcpy(&pid,table+cur,2);
		cur +=2;
		pid = (ntohs(pid)) & PAT_ENTRY_PID;

		ProgramAssociationTable::iterator it = pid_to_program_.find(pid);

		if(it == pid_to_program_.end())
			pid_to_program_.insert(MapType::value_type(pid,program));
	}

	// Get CRC
	UINT32 crc;
	memcpy(&crc,table+cur,4);
	CRC_32_ = ntohl(crc);

}

lcss::ProgramAssociationTable::iterator lcss::ProgramAssociationTable::begin()
{
	return pid_to_program_.begin();
}

lcss::ProgramAssociationTable::iterator lcss::ProgramAssociationTable::end()
{
	return pid_to_program_.end();
}

lcss::ProgramAssociationTable::const_iterator lcss::ProgramAssociationTable::begin() const
{
	return pid_to_program_.begin();
}

lcss::ProgramAssociationTable::const_iterator lcss::ProgramAssociationTable::end() const
{
	return pid_to_program_.end();
}


lcss::ProgramAssociationTable::iterator lcss::ProgramAssociationTable::find(UINT32 pid)
{
	return pid_to_program_.find(pid);
}

size_t lcss::ProgramAssociationTable::size() const
{
	return pid_to_program_.size();
}

BYTE lcss::ProgramAssociationTable::pointer_field() const
{
	return pointer_field_;
}

BYTE lcss::ProgramAssociationTable::table_id() const
{
	return table_id_;
}

UINT16 lcss::ProgramAssociationTable::section_length() const
{
	return section_length_;
}

UINT16 lcss::ProgramAssociationTable::transport_stream_id() const
{
	return transport_stream_id_;
}

BYTE lcss::ProgramAssociationTable::version_number() const
{
	return version_number_;
}

bool lcss::ProgramAssociationTable::current_next_indicator() const
{
	return current_next_indicator_;
}

BYTE lcss::ProgramAssociationTable::section_number() const
{
	return section_number_;
}

BYTE lcss::ProgramAssociationTable::last_section_number() const
{
	return last_section_number_;
}

UINT32 lcss::ProgramAssociationTable::CRC_32() const
{
	return  CRC_32_;
}
