#include "stdafx.h"
#include "tsnit.h"

#include <algorithm>
#include <iterator>

#ifndef WIN32
#include <memory.h>
#include <arpa/inet.h>
#endif

using namespace std;

const UINT16 NIT_SECTION_LEN = 0x03FF;
const BYTE NIT_VERSION_NO = 0x3E;

namespace lcss
{

NetworkInformationTable::NetworkInformationTable()
{

}

NetworkInformationTable::~NetworkInformationTable()
{

}

bool lcss::NetworkInformationTable::parse(const BYTE* table)
{
	if (table == nullptr)
		return false;

	nit_.push_back(table[0]);
	UINT16 offset = nit_[0] == 0? 1 : nit_[0]; // pointer_field
	UINT16 cur = offset;
	UINT16 value = 0;

	// stuffing bytes
	for( auto i = 1; i < nit_[0]; i++)
		nit_.push_back(table[i]);

	if(table[cur] != 0x40) // table_id must be 0x40
		return false;

	nit_.push_back(table[cur++]); // table_id

	memcpy(&value,table+cur,2);
	UINT16 section_length = ntohs(value) & NIT_SECTION_LEN;
	nit_.push_back(table[cur++]); //section_syntax_indicator, reserved_future_use, reserved, section_length
	nit_.push_back(table[cur++]);
	UINT16 crcPos = cur + section_length - 4;
	UINT32 crc;
	memcpy(&crc,table+crcPos,4);
	CRC_32_ = ntohl(crc);

	nit_.push_back(table[cur++]); //network_id
	nit_.push_back(table[cur++]);

	nit_.push_back(table[cur++]); //reserved, version_number, current_next_indicator

	nit_.push_back(table[cur++]); //section_number

	nit_.push_back(table[cur++]); //last_section_number

	memcpy(&value,table+cur,2);
	UINT16 network_descriptors_length = ntohs(value) & NIT_SECTION_LEN;
	nit_.push_back(table[cur++]); // reserved, network_descriptors_length
	nit_.push_back(table[cur++]);

	UINT16 end = network_descriptors_length + cur;

	while(cur < end)
	{
		BYTE bValue;
		memcpy(&bValue,table+cur,1);
		Descriptor desc(bValue);
		cur++;

		memcpy(&bValue,table+cur,1);
		UINT16 len = bValue;
		cur++;

		if(len > 0)
		{
			desc.setValue(table+cur,len);
			cur += len;
		}

		lcss::eqDescriptor pred(desc.tag());
		lcss::NetworkInformationTable::DescriptorArray::iterator result = find_if(network_descriptors_.begin(), network_descriptors_.end(),pred);
		if(result == network_descriptors_.end()) // prevent adding duplicates
			network_descriptors_.push_back(desc);
	}

	memcpy(&value,table+cur,2);
	UINT16 transport_stream_loop_length = ntohs(value) & NIT_SECTION_LEN;
	nit_.push_back(table[cur++]); // reserved_future_use, transport_stream_loop_length
	nit_.push_back(table[cur++]);

	end = transport_stream_loop_length + cur;

	while(cur < end)
	{
		NetworkInformationTable::Stream stream;
		memcpy(&value,table+cur,2);
		stream.transport_stream_id_ = ntohs(value);
		cur += 2;
		
		memcpy(&value,table+cur,2);
		stream.original_network_id_ = ntohs(value);
		cur += 2;

		memcpy(&value,table+cur,2);
		UINT16 transport_descriptors_length = ntohs(value) & NIT_SECTION_LEN;
		cur += 2;

		UINT16 tsEnd = transport_descriptors_length + cur;

		while(cur < tsEnd)
		{
			BYTE bValue;
			memcpy(&bValue,table+cur,1);
			Descriptor desc(bValue);
			cur++;

			memcpy(&bValue,table+cur,1);
			UINT16 len = bValue;
			cur++;

			if(len > 0)
			{
				desc.setValue(table+cur,len);
				cur += len;
			}

			stream.descriptors_.push_back(desc);
		}
		streams_.push_back(stream);
	}

	return true;
}

BYTE lcss::NetworkInformationTable::pointer_field() const
{
	return nit_[0];
}

BYTE lcss::NetworkInformationTable::table_id() const
{
	UINT16 cur = nit_[0] == 0 ? 1 : nit_[0];
	if(cur > nit_.size()-1)
		return 0xFF;
	return nit_[cur];
}

UINT16 lcss::NetworkInformationTable::network_id() const
{
	UINT16 cur = nit_[0] == 0 ? 0 : nit_[0];
	cur += 4;
	BYTE chVal[2];
	UINT16 value;
	chVal[0] = nit_[cur++];
	chVal[1] = nit_[cur];
	memcpy(&value,chVal,2);;
	return ntohs(value);
}

BYTE lcss::NetworkInformationTable::version_number() const
{
	UINT16 cur = nit_[0] == 0 ? 0 : nit_[0];
	cur += 6;
	return (nit_[cur]&NIT_VERSION_NO)>>1;
}

bool lcss::NetworkInformationTable::current_next_indicator() const
{
	UINT16 cur = nit_[0] == 0 ? 0 : nit_[0];
	cur += 6;
	return (nit_[cur]&0x01) == 0x01 ? true : false;
}

BYTE lcss::NetworkInformationTable::section_number() const
{
	UINT16 cur = nit_[0] == 0 ? 0 : nit_[0];
	cur += 7;
	return nit_[cur];
}

BYTE lcss::NetworkInformationTable::last_section_number() const
{
	UINT16 cur = nit_[0] == 0 ? 0 : nit_[0];
	cur += 8;
	return nit_[cur];
}

UINT32 lcss::NetworkInformationTable::CRC_32() const
{
	return CRC_32_;
}

}