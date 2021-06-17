#include "TsWriter.h"
#include "libmp2t.h"
#include <iostream>
#include <sstream>

using namespace std;

void TsWriter::printHeader(const lcss::TransportPacket& pckt)
{
	bool isAdaptationField = false;
	cout << "\ttransport_packet() {" << endl;
	cout << "\t\tsync_byte: 0x47" << endl;
	cout << "\t\ttransport_error_indicator: " << pckt.TEI() << endl;
	cout << "\t\tpayload_unit_start_indicator: " << pckt.payloadUnitStart() << endl;
	cout << "\t\ttransport_priority: " << pckt.transportPriority() << endl;
	cout << "\t\tPID: " << pckt.PID() << endl;
	cout << "\t\ttransport_scrambling_control: " << (int)pckt.scramblingControl() << endl;

	std::string strAfe;
	char afe = pckt.adaptationFieldExist();
	switch (afe)
	{
	case 0x00: strAfe = "00 Reserved of future use by ISO/IEC"; break;
	case 0x01: strAfe = "01 No Adaptation_field, payload only"; break;
	case 0x02: strAfe = "10 Adaptation_field only, no payload";
		isAdaptationField = true;
		break;
	case 0x03: strAfe = "11 Adaptation_field followed by payload";
		isAdaptationField = true;
		break;
	default: strAfe = "Unknown value";
	}
	cout << "\t\tadaptation_field_control: " << strAfe << endl;
	cout << "\t\tcontinuity_counter: " << (int)pckt.cc() << endl;

	if (isAdaptationField)
	{
		const lcss::AdaptationField* adf = pckt.getAdaptationField();
		TsWriter::printAdaptationField(*adf);
	}

	unsigned char data_byte = pckt.data_byte();
	if (data_byte != 0)
	{
		cout << "\t\tdata_byte: " << (int)data_byte << endl;
	}
}

void TsWriter::printAdaptationField(const lcss::AdaptationField& adf)
{
	cout << "\t\tadaptation_field() {" << endl;
	cout << "\t\t\tadaptation_field_length: " << adf.length() << endl;
	cout << "\t\t\tdiscontinuity_indicator: " << adf.discontinuity_indicator() << endl;
	cout << "\t\t\trandom_access_indicator: " << adf.random_access_indicator() << endl;
	cout << "\t\t\telementary_stream_priority_indicator: " << adf.elementary_stream_priority_indicator() << endl;
	cout << "\t\t\tPCR_flag: " << adf.PCR_flag() << endl;
	cout << "\t\t\tOPCR_flag: " << adf.OPCR_flag() << endl;
	cout << "\t\t\tsplicing_point_flag: " << adf.splicing_point_flag() << endl;
	cout << "\t\t\ttransport_private_data_flag: " << adf.transport_private_data_flag() << endl;
	cout << "\t\t\tadaptation_field_extension_flag: " << adf.adaptation_field_extension_flag() << endl;

	if (adf.PCR_flag() && adf.length() > 0)
	{
		cout << "\t\t\tPCR: " << TsWriter::printPCR(adf) << endl;
	}
	cout << "\t\t}" << endl;
}

std::string TsWriter::printPCR(const lcss::AdaptationField& adf)
{
	string prc;
	if (adf.PCR_flag() && adf.length() > 0)
	{
		unsigned char pcr[6];
		memset(pcr, 0, 6);
		if (adf.getPCR(pcr))
		{
			UINT64 pcr_base = ((UINT64)pcr[0] << (33 - 8)) |
				((UINT64)pcr[1] << (33 - 16)) |
				((UINT64)pcr[2] << (33 - 24)) |
				((UINT64)pcr[3] << (33 - 32));

			unsigned short pcr_ext = pcr[4] & 0x01 << 9;
			pcr_ext = pcr_ext | pcr[5];
			double dpcr = (double)pcr_base * 300 + pcr_ext;
			cout.precision(12);
			std::stringstream pcr_stream;
			pcr_stream.precision(12);
			pcr_stream << "base(" << pcr_base << ") * 300 + ext(" << pcr_ext << ") = " << dpcr << ", pcr/27-MHz = " << (dpcr / 27000000) << " seconds";
			prc = pcr_stream.str();
		}
	}
	return prc;
}

void TsWriter::printPAT(const lcss::ProgramAssociationTable& pat)
{
	std::stringstream val;

	cout << "\t\tprogram_association_section() {" << endl;
	cout << "\t\t\tpointer_field: " << (int) pat.pointer_field() << endl;
	cout << "\t\t\ttable_id: 0 (Program Association Section)" << endl;
	cout << "\t\t\tsection_length: " <<  pat.section_length() << endl;
	cout << "\t\t\ttransport_stream_id: " << pat.transport_stream_id() << endl;
	cout << "\t\t\tversion_number: " << (int) pat.version_number() << endl;
	cout << "\t\t\tcurrent_next_indicator: " << pat.current_next_indicator() << endl;
	cout << "\t\t\tsection_number: " << (int) pat.section_number() << endl;
	cout << "\t\t\tlast_section_number: " << (int) pat.last_section_number() << endl;

	// print out the sections
	lcss::ProgramAssociationTable::const_iterator it;
	size_t sz = pat.size();
	size_t i = 1;
	cout << "\t\t\tprograms() {" << endl;
	for (it = pat.begin(); it != pat.end(); ++it, ++i)
	{
		cout << "\t\t\t\tprogram_number: " << it->second << endl;
		cout << "\t\t\t\tPID: " << it->first << endl;
	}
	cout << "\t\t}" << endl;

	val << "0x" << hex << pat.CRC_32();
	cout << "\t\tCRC_32: " << val.str() << endl;
}
