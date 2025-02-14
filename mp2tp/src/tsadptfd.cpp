#include "tsadptfd.h"

#include <memory.h>
#include <string.h>

// Adaptation Headers
const BYTE AF_DISCONTINUITY_MASK	= 0x80;
const BYTE AF_RANDOM_ACCESS_MASK	= 0x40;
const BYTE AF_ELEM_STREAM_PRI_MASK	= 0x20;
const BYTE AF_PCR_MASK				= 0x10;
const BYTE AF_OPCR_MASK				= 0x08;
const BYTE AF_SPLICING_PT_MASK		= 0x04;
const BYTE AF_TRANSPORT_PRIV_MASK	= 0x02;
const BYTE AF_AF_EXT_MASK			= 0x01;

lcss::AdaptationField::AdaptationField()
	:_data(nullptr)
{

}


lcss::AdaptationField::AdaptationField(const BYTE* data)
	:_data(data+4) // start after the TS header
{

}

lcss::AdaptationField::~AdaptationField()
{

}

unsigned short lcss::AdaptationField::length() const
{
	short len = 0;
	memcpy(&len,_data,1);
	return len;
}

bool lcss::AdaptationField::discontinuity_indicator() const
{
	return _data[1] & AF_DISCONTINUITY_MASK ? true : false;
}

bool lcss::AdaptationField::random_access_indicator() const
{
	return _data[1] & AF_RANDOM_ACCESS_MASK ? true : false;
}

bool lcss::AdaptationField::elementary_stream_priority_indicator() const
{
	return _data[1] & AF_ELEM_STREAM_PRI_MASK ? true : false;
}

bool lcss::AdaptationField::PCR_flag() const
{
	return _data[1] & AF_PCR_MASK ? true : false;
}

bool lcss::AdaptationField::OPCR_flag() const
{
	return _data[1] & AF_OPCR_MASK ? true : false;
}

bool lcss::AdaptationField::splicing_point_flag() const
{
	return _data[1] & AF_SPLICING_PT_MASK ? true : false;
}

bool lcss::AdaptationField::transport_private_data_flag() const
{
	return _data[1] & AF_TRANSPORT_PRIV_MASK ? true : false;
}

bool lcss::AdaptationField::adaptation_field_extension_flag() const
{
	return _data[1] & AF_AF_EXT_MASK ? true : false;
}

bool lcss::AdaptationField::getPCR(BYTE* pcr) const
{
	if(PCR_flag())
	{
		memset(pcr,0,6);
		memcpy(pcr,_data+2,6);
		return true;
	}
	return false;
}

void lcss::AdaptationField::parse(const BYTE* data)
{
	_data = data + 4;
}