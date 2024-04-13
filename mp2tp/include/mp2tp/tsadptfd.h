#ifndef TSADPTFD_H
#define TSADPTFD_H

#include "tstype.h"

namespace lcss
{

/// @brief AdaptationField represents an instance of a adaptation_field as defined in
/// Ref: ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Reserved, Table 2-6 page 23
class AdaptationField
{
public:
	AdaptationField();
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
	void parse(const BYTE* data);

private:
	const BYTE* _data;
};


}

#endif