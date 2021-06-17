#ifndef TSADPTFD_H
#define TSADPTFD_H

#include "tstype.h"

namespace lcss
{

class AdaptationField
{
public: 
	AdaptationField(const BYTE* data);
	~AdaptationField();

	unsigned short length() const;
	bool discontinuity_indicator() const;
	bool random_access_indicator() const;
	bool elementary_stream_priority_indicator() const;
	bool PCR_flag() const;
	bool OPCR_flag() const;
	bool splicing_point_flag() const;
	bool transport_private_data_flag() const;
	bool adaptation_field_extension_flag() const;

	bool getPCR(BYTE* pcr) const;

private:
	const BYTE* _data;
};


}

#endif