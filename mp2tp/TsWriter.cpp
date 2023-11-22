#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "TsWriter.h"
#include "libmp2t.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#ifdef WIN32
#include <WinSock2.h>
#else
#include <memory.h>
#include <arpa/inet.h>
#endif

using namespace std;

namespace
{
	const BYTE PTS_DTS_MASK = 0xC0;

	// Function name   : getDescriptorText
	// Description     : Returns a string associated with a tag number.
	// See ISO/IEC 13818-1 : 2013 (E), Table 2-45 - Program and program element descriptors, page 69
	// Return type     : void 
	// Argument        : unsigned int tag
	std::string getDescriptorText(unsigned int tag)
	{
		std::string strTag;
		if (tag == 2)
			strTag = "video_stream_descriptor";
		else if (tag == 3)
			strTag = "audio_stream_descriptor";
		else if (tag == 4)
			strTag = "hierarchy_descriptor";
		else if (tag == 5)
			strTag = "registration_descriptor";
		else if (tag == 6)
			strTag = "data_stream_alignment_descriptor";
		else if (tag == 7)
			strTag = "target_background_grid_descriptor";
		else if (tag == 8)
			strTag = "Video_window_descriptor";
		else if (tag == 9)
			strTag = "CA_descriptor";
		else if (tag == 10)
			strTag = "ISO_639_language_descriptor";
		else if (tag == 11)
			strTag = "System_clock_descriptor";
		else if (tag == 12)
			strTag = "Multiplex_buffer_utilization_descriptor";
		else if (tag == 13)
			strTag = "Copyright_descriptor";
		else if (tag == 14)
			strTag = "Maximum_bitrate_descriptor";
		else if (tag == 15)
			strTag = "Private_data_indicator_descriptor";
		else if (tag == 16)
			strTag = "Smoothing_buffer_descriptor";
		else if (tag == 17)
			strTag = "STD_descriptor";
		else if (tag == 18)
			strTag = "IBP_descriptor";
		else if (tag >= 19 && tag <= 26)
			strTag = "Defined in ISO/IEC 13818-6";
		else if (tag == 27)
			strTag = "MPEG-4_video_descriptor";
		else if (tag == 28)
			strTag = "MPEG-4_audio_descriptor";
		else if (tag == 29)
			strTag = "IOD_descriptor";
		else if (tag == 30)
			strTag = "SL_descriptor";
		else if (tag == 31)
			strTag = "FMC_descriptor";
		else if (tag == 32)
			strTag = "external_ES_ID_descriptor";
		else if (tag == 33)
			strTag = "MuxCode_descriptor";
		else if (tag == 34)
			strTag = "FmxBufferSize_descriptor";
		else if (tag == 35)
			strTag = "multiplexBuffer_descriptor";
		else if (tag == 36)
			strTag = "content_labeling_descriptor";
		else if (tag == 37)
			strTag = "metadata_pointer_descriptor";
		else if (tag == 38)
			strTag = "metadata_descriptor";
		else if (tag == 39)
			strTag = "metadata_STD_descriptor";
		else if (tag == 40)
			strTag = "AVC video descriptor";
		else if (tag == 41)
			strTag = "IPMP_descriptor";
		else if (tag == 42)
			strTag = "AVC timing and HRD descriptor";
		else if (tag == 43)
			strTag = "MPEG-2_AAC_audio_descriptor";
		else if (tag == 44)
			strTag = "FlexMuxTiming_descriptor";
		else if (tag == 45)
			strTag = "MPEG-4_text_descriptor";
		else if (tag == 46)
			strTag = "MPEG-4_audio_extension_descriptor";
		else if (tag == 47)
			strTag = "auxiliary_video_stream_descriptor";
		else if (tag == 48)
			strTag = "SVC extension descriptor";
		else if (tag == 49)
			strTag = "MVC extension descriptor";
		else if (tag == 50)
			strTag = "J2K video descriptor";
		else if (tag == 51)
			strTag = "MVC operation point descriptor";
		else if (tag == 52)
			strTag = "MPEG2_stereoscopic_video_format_descriptor";
		else if (tag == 53)
			strTag = "Stereoscopic_program_info_descriptor";
		else if (tag == 54)
			strTag = "Stereoscopic_video_info_descriptor";
		else if (tag >= 55 && tag <= 63)
			strTag = "Rec. ITU-T H.222.0 | ISO/IEC 13818-1 Reserved";
		else if (tag == 64)
			strTag = "network_name_descriptor";
		else if (tag > 64 && tag <= 255)
			strTag = "User Private";

		return strTag;
	}

	// Function name   : getStreamTypeDescription
	// Description     : Returns a description of the stream type. 
	// Ref: ISO/IEC 13818-1 : 2013 (E) Table 2-34 - Stream type assignment, pg 51 
	// Return type     : void 
	// Argument        : unsigned int type
	std::string getStreamTypeDescription(unsigned int type)
	{
		string strType;
		if (type == 0x00)
			strType = "ITU-T | ISO/IEC Reserved";
		else if (type == 0x01)
			strType = "ISO/IEC 11172 Video";
		else if (type == 0x02)
			strType = "ITU-T Rec. H.262 | ISO/IEC 13818-2 Video or ISO/IEC 11172-2 constrained parameter video stream";
		else if (type == 0x03)
			strType = "ISO/IEC 11172 Audio";
		else if (type == 0x04)
			strType = "ISO/IEC 13818-3 Audio";
		else if (type == 0x05)
			strType = "Rec. ITU-T H.222.0 | ISO/IEC 13818-1 private_sections";
		else if (type == 0x06)
			strType = "Rec. ITU-T H.222.0 | ISO/IEC 13818-1 PES packets containing private data";
		else if (type == 0x07)
			strType = "ISO/IEC 13522 MHEG";
		else if (type == 0x08)
			strType = "Rec. ITU-T H.222.0 | ISO/IEC 13818-1 Annex A DSM-CC";
		else if (type == 0x09)
			strType = "Rec. ITU-T H.222.1";
		else if (type == 0x0A)
			strType = "ISO/IEC 13818-6 type A";
		else if (type == 0x0B)
			strType = "ISO/IEC 13818-6 type B";
		else if (type == 0x0C)
			strType = "ISO/IEC 13818-6 type C";
		else if (type == 0x0D)
			strType = "ISO/IEC 13818-6 type D";
		else if (type == 0x0E)
			strType = "ITU-T Rec. H.222.0 | ISO/IEC 13818-1 auxiliary";
		else if (type == 0x0F)
			strType = "ISO/IEC 13818-7 Audio with ADTS transport syntax";
		else if (type == 0x10)
			strType = "ISO/IEC 14496-2 Visual";
		else if (type == 0x11)
			strType = "ISO/IEC 14496-3 Audio with the LATM transport syntax as defined in ISO/IEC 14496-3 / AMD 1";
		else if (type == 0x12)
			strType = "ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in PES packets";
		else if (type == 0x13)
			strType = "ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in ISO/IEC14496_sections";
		else if (type == 0x14)
			strType = "ISO/IEC 13818-6 Synchronized Download Protocol";
		else if (type == 0x15)
			strType = "Metadata carried in PES packets";
		else if (type == 0x16)
			strType = "Metadata carried in metadata sections";
		else if (type == 0x17)
			strType = "Metadata carried in ISO/IEC 13818-6 Data Carousel";
		else if (type == 0x18)
			strType = "Metadata carried in ISO/IEC 13818-6 Object Carousel";
		else if (type == 0x19)
			strType = "Metadata carried in ISO/IEC 13818-6 Synchronized Download Protocol";
		else if (type == 0x1A)
			strType = "IPMP stream (defined in ISO/IEC 13818-11, MPEG-2 IPMP";
		else if (type == 0x1B)
			strType = "AVC video stream conforming to one or more profiles defined in Annex A of Rec. ITU-T H.264|ISO/IEC 14496-10 or AVC video sub-bitstream of SVC as defined in 2.1.78 or MVC base view sub-bitstream, as defined in 2.1.85, or AVC video sub-bitstream of MVC, as defined in 2.1.88";
		else if (type == 0x1C)
			strType = "ISO/IEC 14496-3 Audio, without using any additional transport syntax, such as DST, ALS and SLS";
		else if (type == 0x1D)
			strType = "ISO/IEC 14496-17 Text";
		else if (type == 0x1E)
			strType = "Auxiliary video stream as defined in ISO/IEC 23002-3";
		else if (type == 0x1F)
			strType = "SVC video sub-bitstream of an AVC video stream conforming to one or more profiles defined in Annex G of Rec. ITU-T H.264|ISO/IEC 14496-10";
		else if (type == 0x20)
			strType = "MVC video sub-bitstream of an AVC video stream conforming to one or more profiles defined in Annex H of Rec. ITU-T H.264|ISO/IEC 14496-10";
		else if (type == 0x21)
			strType = "Video stream conforming to one or more profiles as defined in Rec. ITU-T T.800|ISO/IEC 15444-1";
		else if (type == 0x22)
			strType = "Additional view Rec. ITU-T H.262|ISO/IEC 13818-2 video stream for service-compatible stereoscopic 3D services";
		else if (type == 0x23)
			strType = "Additional view Rec. ITU-T H.264|ISO/IEC 14496-10 video stream conforming to one or more profiles defined in Annex A for service-compatible stereoscopic 3D services";
		else if (type >= 0x24 || type <= 0x7e)
			strType = "Rec. ITU-T H.222.0 | ISO/IEC 13818-1 Reserved";
		else if (type == 0x7f)
			strType = "IPMP";
		else if (type >= 0x80 || type <= 0xFF)
			strType = "User Private";

		return strType;
	}

	// Function name   : getStreamIdDescription
	// Description     : ISO/IEC 13818-1 : 2013 (E) Table 2-22 Stream_id assignments, pg 36
	// Return type     : void 
	// Argument        : BYTE streamId
	std::string getStreamIdDescription(BYTE streamId)
	{
		string strStreamId("ERROR UNKNOWN STREAM ID");

		if (streamId == 188)
			strStreamId = "program_stream_map";
		else if (streamId == 189)
			strStreamId = "private_stream_1";
		else if (streamId == 190)
			strStreamId = "padding_stream";
		else if (streamId == 191)
			strStreamId = "private_stream_2";
		else if (streamId >= 192 && streamId <= 223)
			strStreamId = "ISO/IEC 13818-3 or ISO/IEC 11172-3 or ISO/IEC 13818-7 or ISO/IEC 14496-3 audio stream number x xxxx";
		else if (streamId >= 224 && streamId <= 239)
			strStreamId = "ITU-T Rec. H.262 | ISO/IEC 13818-2, ISO/IEC 11172-2, ISO/IEC 14496-2 or Rec. ITU-T H.264 | ISO/IEC 14496-10 video stream number xxxx";
		else if (streamId == 240)
			strStreamId = "ECM_stream";
		else if (streamId == 241)
			strStreamId = "EMM_stream";
		else if (streamId == 242)
			strStreamId = "Rec. ITU-T H.222.0 | ISO/IEC 13818-1 Annex A or ISO/IEC 13818-6_DSMCC_stream";
		else if (streamId == 243)
			strStreamId = "ISO/IEC_13522_stream";
		else if (streamId == 244)
			strStreamId = "Rec. ITU-T H.222.1 type A";
		else if (streamId == 245)
			strStreamId = "Rec. ITU-T H.222.1 type B";
		else if (streamId == 246)
			strStreamId = "Rec. ITU-T H.222.1 type C";
		else if (streamId == 247)
			strStreamId = "Rec. ITU-T H.222.1 type D";
		else if (streamId == 248)
			strStreamId = "Rec. ITU-T H.222.1 type E";
		else if (streamId == 249)
			strStreamId = "ancillary_stream";
		else if (streamId == 250)
			strStreamId = "ISO/IEC14496-1_SL-packetized_stream";
		else if (streamId == 251)
			strStreamId = "ISO/IEC14496-1_FlexMux_stream";
		else if (streamId == 252)
			strStreamId = "(0xFC) metadata stream";
		else if (streamId == 253)
			strStreamId = "extended_stream_id";
		else if (streamId == 254)
			strStreamId = "reserved data stream";
		else if (streamId == 255)
			strStreamId = "program_stream_directory";

		return strStreamId;
	}


	void printRegistrationDescriptor(std::ostream& ostrm, const lcss::Descriptor& desc, int depth)
	{
		string text;
		std::stringstream tag;
		std::stringstream info;
		size_t cur = 0;

		char buf[BUFSIZ]{};
		BYTE val[BUFSIZ]{};
		desc.value(val);

		string indent;
		for (int i = 0; i < depth; i++)
		{
			indent += "\t";
		}

		text = getDescriptorText(desc.tag());

		tag << "0x" << hex << setfill('0') << setw(2) << (int)desc.tag();
		ostrm << indent << "tag: " << tag.str() << " (" << text << ")" << endl;
		ostrm << indent << "length: " << (unsigned int)desc.length() << endl;

		while (cur < desc.length())
		{
			if (cur == 0) // format_identifier
			{
				sprintf(buf, "%c%c%c%c (%#4.2x %#4.2x %#4.2x %#4.2x)", val[cur], val[cur + 1], val[cur + 2], val[cur + 3], val[cur], val[cur + 1], val[cur + 2], val[cur + 3]);
				cur += 4;
				ostrm << indent << "format_identifier: " << buf << endl;
			}
			else
			{
				BYTE c = val[cur++];
				(isprint(c) != 0) ? info << (char)c : info << "(0x" << hex << setfill('0') << setw(2) << (int)c << ")";
			}
		}

		if (info.str().length() > 0)
		{
			ostrm << indent << "additional_identification_info: " << info.str() << endl;
		}
	}

	// ISO/IEC 13818-1 : 2013 (E), Table 2-83 - Metadata pointer descriptor, page 91
	void printMetadataPointerDescriptor(std::ostream& ostrm, const lcss::Descriptor& desc)
	{
		char text[BUFSIZ]{};
		size_t cur = 0;
		BYTE metadata_format = 0;
		BYTE MPEG_carriage_flag = 0;
		string indent("\t\t\t\t");
		std::stringstream tag;

		BYTE val[BUFSIZ]{};
		desc.value(val);

		ostrm << "\t\t\t" << getDescriptorText(desc.tag()) << "() {" << endl;

		tag << "0x" << hex << setfill('0') << setw(2) << (int)desc.tag();
		ostrm << indent << "tag: " << tag.str() << endl;
		ostrm << indent << "length: " << (unsigned int)desc.length() << endl;

		while (cur < desc.length())
		{
			if (cur == 0)
			{
				unsigned short metadata_application_format; // Table 2-81, page 89
				memcpy(&metadata_application_format, val, 2);
				metadata_application_format = ntohs(metadata_application_format);

				if (metadata_application_format >= 0 && metadata_application_format <= 0x000F)
					sprintf(text, "Reserved 0x%2.2x 0x%2.2x", val[0], val[1]);
				else if (metadata_application_format == 0x0010)
					strcpy(text, "ISO 15706 (ISAN) encoded in its binary form 0x00 0x10");
				else if (metadata_application_format == 0x0011)
					strcpy(text, "ISO 15706-2 (V-ISAN) encoded in its binary form 0x00 0x11");
				else if (metadata_application_format >= 0x0012 && metadata_application_format <= 0x00FF)
					sprintf(text, "Reserved 0x%2.2x 0x%2.2x", val[0], val[1]);
				else if (metadata_application_format >= 0x0100 && metadata_application_format <= 0xFFFE)
					sprintf(text, "User defined 0x%2.2x 0x%2.2x", val[0], val[1]);
				else if (metadata_application_format == 0xFFFF)
					sprintf(text, "Defined by the metadata_application_format_identifier field %#4.2x %#4.2x", val[0], val[1]);
				cur += 2;
				ostrm << indent << "metadata_application_format: " << text << endl;
			}
			else if (cur == 2)
			{
				metadata_format = val[2];

				if (metadata_format >= 0 && metadata_format <= 0x0F)
					sprintf(text, "Reserved %#4.2x", metadata_format);
				else if (metadata_format == 0x10)
					sprintf(text, "ISO/IEC 15938-1 TeM %#4.2x", metadata_format);
				else if (metadata_format == 0x11)
					sprintf(text, "ISO/IEC 15938-1 BiM %#4.2x", metadata_format);
				else if (metadata_format >= 0x12 && metadata_format <= 0x3E)
					sprintf(text, "Reserved %#4.2x", metadata_format);
				else if (metadata_format == 0x3F)
					strcpy(text, "Defined by metadata application format 0x3F");
				else if (metadata_format >= 0x40 && metadata_format <= 0xFE)
					sprintf(text, "Private use %#4.2x", metadata_format);
				else if (metadata_format == 0xFF)
					strcpy(text, "Defined by metadata_format_identifer field 0xFF");

				cur++;
				ostrm << indent << "metadata_format: " << text << endl;
			}
			else if (cur == 3)
			{
				if (metadata_format == 0xFF)
				{
					sprintf(text, "%c%c%c%c (%#4.2x %#4.2x %#4.2x %#4.2x)", val[cur], val[cur + 1], val[cur + 2], val[cur + 3], val[cur], val[cur + 1], val[cur + 2], val[cur + 3]);
					cur += 4;
					ostrm << indent << "metadata_format_identifier: " << text << endl;
				}
				else
					cur++;
			}
			else if ((cur == 4 && metadata_format != 0xFF) || cur == 7)
			{
				ostrm << indent << "metadata_service_id: " << (unsigned int)val[cur++] << endl;
			}
			else if ((cur == 5 && metadata_format != 0xFF) || cur == 8)
			{
				BYTE flag = val[cur++];
				char name[BUFSIZ]{};
				sprintf(name, "%#4.2x", flag);
				ostrm << indent << "metadata_locator_record_flag: " << (flag & 0x80 ? "true" : "false") << endl;

				BYTE mask = 0x60;
				MPEG_carriage_flag = flag & mask;
				std::stringstream tag;
				// Table 2-85 - MPEG_carrier_flags
				if (MPEG_carriage_flag == 0x00)
					tag << "Carriage in the same transport stream where this metadata pointer descriptor is carried. 0";
				else if (MPEG_carriage_flag == 0x20)
					tag << "\"Carriage in a different transport stream from where this metadata pointer descriptor is carried. 1";
				else if (MPEG_carriage_flag == 0x40)
					tag << "\"Carriage in a program stream.  This may or may not be the same program stream in which this metadata pointer descriptor is carried. 2";

				ostrm << indent << "MPEG_carriage_flags: " << tag.str() << endl;
				ostrm << indent << "reserved: " << "11111" << endl;
			}
			else if ((cur == 6 && metadata_format != 0xFF) || cur == 9)
			{
				if (MPEG_carriage_flag == 0x00 || MPEG_carriage_flag == 0x20 || MPEG_carriage_flag == 0x40)
				{
					unsigned short program_number;
					memcpy(&program_number, val + cur, 2);
					program_number = ntohs(program_number);
					sprintf(text, "%d (0x%2.2x 0x%2.2x)", program_number, val[cur], val[cur + 1]);
					ostrm << indent << "program_number: " << text << endl;
				}
				cur += 2;
			}
			else if ((cur == 7 && metadata_format != 0xFF) || cur == 11)
			{
				if (MPEG_carriage_flag == 0x20)
				{
					unsigned short transport_stream_location;
					unsigned short transport_stream_id;
					memcpy(&transport_stream_location, val + cur, 2);
					memcpy(&transport_stream_id, val + (cur + 2), 2);

					sprintf(text, "\"%d %#4.2x %#4.2x\",", transport_stream_location, val[cur], val[cur + 1]);
					ostrm << indent << "transport_stream_location: " << text << endl;

					sprintf(text, "\"%d %#4.2x %#4.2x\"", transport_stream_id, val[cur + 2], val[cur + 3]);
					ostrm << indent << "transport_stream_id: " << text << endl;
					cur += 4;
				}
			}
			else if ((cur > 7 && metadata_format != 0xFF) || cur > 11)// private data
			{
				tag = std::stringstream();
				while (cur < desc.length())
				{
					int c = (int)val[cur++];
					char hexc[32];
					if (c != 0) {
						if (isalnum(c))
							sprintf(hexc, "%#4.2x (%c) ", c, c);
						else
							sprintf(hexc, "%#4.2x ", c);
					}
					else
						sprintf(hexc, "0x00 ");
					tag << hexc;
				}
				ostrm << indent << "private_data: " << tag.str() << endl;
			}
		}
		ostrm << "\t\t\t}" << endl;
	}

	// ISO/IEC 13818-1 : 2013 (E), Table 2-86 - Metadata descriptor, page 94
	void printMetadataDescriptor(std::ostream& ostrm, const lcss::Descriptor& desc, int depth)
	{
		size_t cur = 0;
		BYTE metadata_format = 0;
		BYTE MPEG_carriage_flag = 0;
		stringstream tag;
		string indent;
		for (int i = 0; i < depth; i++)
		{
			indent += "\t";
		}
		BYTE val[BUFSIZ]{};
		desc.value(val);
		char text[BUFSIZ]{};

		tag << "0x" << hex << setfill('0') << setw(2) << (int)desc.tag() << " (" << getDescriptorText(desc.tag()) << ")";
		ostrm << indent << "tag: " << tag.str() << endl;
		ostrm << indent << "length: " << (unsigned int)desc.length() << endl;

		while (cur < desc.length())
		{
			if (cur == 0)
			{
				unsigned short metadata_application_format; // Table 2-81, page 89
				memcpy(&metadata_application_format, val, 2);
				metadata_application_format = ntohs(metadata_application_format);

				if (metadata_application_format >= 0 && metadata_application_format <= 0x000F)
					sprintf(text, "Reserved %#4.2x %#4.2x", val[0], val[1]);
				else if (metadata_application_format == 0x0010)
					strcpy(text, "ISO 15706 (ISAN) encoded in its binary form 0x00 0x10");
				else if (metadata_application_format == 0x0011)
					strcpy(text, "ISO 15706-2 (V-ISAN) encoded in its binary form 0x00 0x11");
				else if (metadata_application_format >= 0x0012 && metadata_application_format <= 0x00FF)
					sprintf(text, "Reserved %#4.2x %#4.2x", val[0], val[1]);
				else if (metadata_application_format == 0x0100)
					sprintf(text, "MISB ST 1402 Table 4, General, 0x0100 (default)");
				else if (metadata_application_format == 0x0101)
					sprintf(text, "MISB ST 1402 Table 4, Geographic Metadata, 0x0101");
				else if (metadata_application_format == 0x0102)
					sprintf(text, "MISB ST 1402 Table 4, Annotation Metadata, 0x0102");
				else if (metadata_application_format == 0x0103)
					sprintf(text, "MISB ST 1402 Table 4, Still Image on Demand, 0x0103");
				else if (metadata_application_format > 0x0103 && metadata_application_format <= 0xFFFE)
					sprintf(text, "User defined %#4.2x %#4.2x", val[0], val[1]);
				else if (metadata_application_format == 0xFFFF)
					sprintf(text, "Defined by the metadata_application_format_identifier field %#4.2x %#4.2x", val[0], val[1]);

				cur += 2;

				ostrm << indent << "metadata_application_format: " << text << endl;
			}
			else if (cur == 2)
			{
				metadata_format = val[2];

				if (metadata_format >= 0 && metadata_format <= 0x0F)
					sprintf(text, "\"Reserved %#4.2x\"", metadata_format);
				else if (metadata_format == 0x10)
					sprintf(text, "\"ISO/IEC 15938-1 TeM %#4.2x\"", metadata_format);
				else if (metadata_format == 0x11)
					sprintf(text, "\"ISO/IEC 15938-1 BiM %#4.2x\"", metadata_format);
				else if (metadata_format >= 0x12 && metadata_format <= 0x3E)
					sprintf(text, "\"Reserved %#4.2x\"", metadata_format);
				else if (metadata_format == 0x3F)
					strcpy(text, "Defined by metadata application format 0x3F");
				else if (metadata_format >= 0x40 && metadata_format <= 0xFE)
					sprintf(text, "\"Private use %#4.2x\"", metadata_format);
				else if (metadata_format == 0xFF) {
					strcpy(text, "Defined by metadata_format_identifer field 0xFF");
				}
				cur++;

				ostrm << indent << "metadata_format: " << text << endl;
			}
			else if (cur == 3)
			{
				if (metadata_format == 0xFF)
				{
					sprintf(text, "%c%c%c%c (%#4.2x %#4.2x %#4.2x %#4.2x)", val[cur], val[cur + 1], val[cur + 2], val[cur + 3], val[cur], val[cur + 1], val[cur + 2], val[cur + 3]);
					cur += 4;
					ostrm << indent << "metadata_format_identifier: " << text << endl;
				}
				else
					cur++;
			}
			else if ((cur == 4 && metadata_format != 0xFF) || cur == 7)
			{
				ostrm << indent << "metadata_service_id: " << (unsigned int)val[cur++] << endl;
			}
			else if ((cur == 5 && metadata_format != 0xFF) || cur == 8)
			{
				tag = std::stringstream();
				char name[BUFSIZ]{};
				BYTE flag = val[cur++];
				sprintf(name, "%#4.2x", flag);
				BYTE mask = 0xE0;
				BYTE decoder_config_flags = flag & mask;

				if (decoder_config_flags == 0x00)
				{
					tag << "No decoder configuration is needed. 000" << endl;
					ostrm << indent << "decoder_config_flags: " << tag.str();
					BYTE DSM_CC_flag = flag & 0x10;
					ostrm << indent << "DSM-CC_flag: " << (DSM_CC_flag ? "true" : "false") << endl;
					ostrm << indent << "reserved: " << "1111" << endl;
				}
				else if (decoder_config_flags == 0x20)
				{
					tag << "The decoder configuration is carried in this descriptor in the decoder_config_byte field. 001";
					ostrm << indent << "decoder_config_flags" << tag.str() << endl;
					BYTE DSM_CC_flag = flag & 0x10;
					ostrm << indent << "DSM-CC_flag: " << (DSM_CC_flag ? "true" : "false") << endl;
					ostrm << indent << "reserved: " << "1111" << endl;

					BYTE decoder_config_length = val[cur++];
					ostrm << indent << "decoder_config_length: " << (unsigned int)decoder_config_length << endl;
					std::stringstream str;
					for (int i = 0; i < decoder_config_length; i++)
					{
						int c = (int)val[cur++];
						char hexc[32];
						if (c != 0) {
							if (isprint(c))
								sprintf(hexc, "%c", c);
							else
								sprintf(hexc, "(%#4.2x)", c);
						}
						else
							sprintf(hexc, "(0x00)");
						str << hexc;
					}
					ostrm << indent << "decoder_config_byte: " << str.str() << endl;
				}
				else if (decoder_config_flags == 0x40)
					tag << "The decoder configuration is carried in the same metadata service as to which this metadata descriptor applies. 010";
				else if (decoder_config_flags == 0x60)
					tag << "The decoder configuration is carried in a DSM-CC carousel. This value shall only be used if the metadata service to which this descriptor applies is using the same type of DSM-CC carousel. 011";
				else if (decoder_config_flags == 0x80)
					tag << "The decoder configuration is carried in another metadata service within the same program, as identified by the decoder_config_metadata_service_id field in this metadata descriptor. 100";
				else if (decoder_config_flags == 0xA0)
					tag << "Reserved. 101";
				else if (decoder_config_flags == 0xB0)
					tag << "Reserved. 110";
				else if (decoder_config_flags == 0xE0)
					tag << "Privately defined. 111";
			}
		} // while
	}

	// ISO/IEC 13818-1 : 2013 (E), Table 2-88 - Metadata STD descriptor, page 96
	void printMetadataSTDDescriptor(std::ostream& ostrm, const lcss::Descriptor& desc, int depth)
	{
		std::stringstream tag;
		BYTE val[BUFSIZ]{};
		desc.value(val);
		string text;
		string indent;
		for (int i = 0; i < depth; i++)
		{
			indent += "\t";
		}

		text = getDescriptorText(desc.tag());
		tag << "0x" << hex << setfill('0') << setw(2) << (int)desc.tag() << " (" << text << ")";
		ostrm << indent << "tag: " << tag.str() << endl;
		ostrm << indent << "length: " << (unsigned int)desc.length() << endl;

		size_t cur = 0;
		BYTE mask = 0xC0;

		while (cur < desc.length())
		{
			if (cur == 0) // metadata_input_leak_rate
			{
				BYTE a = val[cur++] ^ mask;
				signed int n;
				BYTE v[4]{};
				v[0] = 0; v[1] = a; v[2] = val[cur++]; v[3] = val[cur++];
				memcpy(&n, v, 4);
				n = ntohl(n);
				ostrm << indent << "metadata_input_leak_rate: " << n << endl;
			}
			else if (cur == 3) // metadata_buffer_size
			{
				BYTE a = val[cur++] ^ mask;
				signed int n;
				BYTE v[4]{};
				v[0] = 0; v[1] = a; v[2] = val[cur++]; v[3] = val[cur++];
				memcpy(&n, v, 4);
				n = ntohl(n);
				ostrm << indent << "metadata_buffer_size: " << n << endl;
			}
			else if (cur == 6) // metadata_output_leak_rate
			{
				BYTE a = val[cur++] ^ mask;
				signed int n;
				BYTE v[4]{};
				v[0] = 0; v[1] = a; v[2] = val[cur++]; v[3] = val[cur++];
				memcpy(&n, v, 4);
				n = ntohl(n);
				ostrm << indent << "metadata_output_leak_rate: " << n << endl;
			}
		}
	}

	void printDescriptorValue(ostream& ostrm, const lcss::Descriptor& desc, int depth)
	{
		BYTE val[BUFSIZ]{};
		desc.value(val);
		std::stringstream str;
		std::stringstream tag;
		string indent;
		for (int i = 0; i < depth; i++)
		{
			indent += "\t";
		}

		switch (desc.tag())
		{
		case 0x05: // registration_descriptor
			printRegistrationDescriptor(ostrm, desc, depth);
			break;
		case 0x25: // metadata_pointer_descriptor
			printMetadataPointerDescriptor(ostrm, desc);
			break;
		case 0x26: // metadata_descriptor
			printMetadataDescriptor(ostrm, desc, depth);
			break;
		case 0x27: // metadata STD descriptor
			printMetadataSTDDescriptor(ostrm, desc, depth);
			break;
		default:
		{
			string text = getDescriptorText(desc.tag());
			tag << "0x" << hex << setfill('0') << setw(2) << (int)desc.tag() << " (" << text << ")";
			ostrm << indent << "tag: " << tag.str() << endl;
			ostrm << indent << "length: " << (unsigned int)desc.length() << endl;
			for (int n = 0; n < desc.length(); n++)
			{
				int c = (int)val[n];
				char hexc[32];
				if (c != 0) {
					if (isprint(c))
						sprintf(hexc, "%c", c);
					else
						sprintf(hexc, "(%#4.2x)", c);
				}
				else
					sprintf(hexc, "(0x00)");
				str << hexc;
			}
			ostrm << indent << "value: " << str.str() << endl;
		}
		}
	}

}

void TsWriter::printHeader(std::ostream& ostrm, const lcss::TransportPacket& pckt)
{
	bool isAdaptationField = false;
	ostrm << "\ttransport_packet() {" << endl;
	ostrm << "\t\tsync_byte: 0x47" << endl;
	ostrm << "\t\ttransport_error_indicator: " << pckt.TEI() << endl;
	ostrm << "\t\tpayload_unit_start_indicator: " << pckt.payloadUnitStart() << endl;
	ostrm << "\t\ttransport_priority: " << pckt.transportPriority() << endl;
	ostrm << "\t\tPID: " << pckt.PID() << endl;
	ostrm << "\t\ttransport_scrambling_control: " << (int)pckt.scramblingControl() << endl;

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
	ostrm << "\t\tadaptation_field_control: " << strAfe << endl;
	ostrm << "\t\tcontinuity_counter: " << (int)pckt.cc() << endl;

	if (isAdaptationField)
	{
		const lcss::AdaptationField* adf = pckt.getAdaptationField();
		TsWriter::printAdaptationField(ostrm, *adf);
	}

	unsigned char data_byte = pckt.data_byte();
	if (data_byte != 0)
	{
		ostrm << "\t\tdata_byte: " << (int)data_byte << endl;
	}
}

void TsWriter::printAdaptationField(std::ostream& ostrm, const lcss::AdaptationField& adf)
{
	ostrm << "\t\tadaptation_field() {" << endl;
	ostrm << "\t\t\tadaptation_field_length: " << adf.length() << endl;
	ostrm << "\t\t\tdiscontinuity_indicator: " << adf.discontinuity_indicator() << endl;
	ostrm << "\t\t\trandom_access_indicator: " << adf.random_access_indicator() << endl;
	ostrm << "\t\t\telementary_stream_priority_indicator: " << adf.elementary_stream_priority_indicator() << endl;
	ostrm << "\t\t\tPCR_flag: " << adf.PCR_flag() << endl;
	ostrm << "\t\t\tOPCR_flag: " << adf.OPCR_flag() << endl;
	ostrm << "\t\t\tsplicing_point_flag: " << adf.splicing_point_flag() << endl;
	ostrm << "\t\t\ttransport_private_data_flag: " << adf.transport_private_data_flag() << endl;
	ostrm << "\t\t\tadaptation_field_extension_flag: " << adf.adaptation_field_extension_flag() << endl;

	if (adf.PCR_flag() && adf.length() > 0)
	{
		ostrm << "\t\t\tPCR: " << TsWriter::printPCR(ostrm, adf) << endl;
	}
	ostrm << "\t\t}" << endl;
}

std::string TsWriter::printPCR(std::ostream& ostrm, const lcss::AdaptationField& adf)
{
	string prc;
	if (adf.PCR_flag() && adf.length() > 0)
	{
		unsigned char pcr[6]{};
		if (adf.getPCR(pcr))
		{
			UINT64 pcr_base = ((UINT64)pcr[0] << (33 - 8)) |
				((UINT64)pcr[1] << (33 - 16)) |
				((UINT64)pcr[2] << (33 - 24)) |
				((UINT64)pcr[3] << (33 - 32));

			unsigned short pcr_ext = pcr[4] & 0x01 << 9;
			pcr_ext = pcr_ext | pcr[5];
			double dpcr = (double)pcr_base * 300 + pcr_ext;
			ostrm.precision(12);
			std::stringstream pcr_stream;
			pcr_stream.precision(12);
			pcr_stream << "base(" << pcr_base << ") * 300 + ext(" << pcr_ext << ") = " << dpcr << ", pcr/27-MHz = " << (dpcr / 27000000) << " seconds";
			prc = pcr_stream.str();
		}
	}
	return prc;
}

void TsWriter::printPAT(std::ostream& ostrm, const lcss::ProgramAssociationTable& pat)
{
	std::stringstream val;

	ostrm << "\t\tprogram_association_section() {" << endl;
	ostrm << "\t\t\tpointer_field: " << (int)pat.pointer_field() << endl;
	ostrm << "\t\t\ttable_id: 0 (Program Association Section)" << endl;
	ostrm << "\t\t\tsection_length: " << pat.section_length() << endl;
	ostrm << "\t\t\ttransport_stream_id: " << pat.transport_stream_id() << endl;
	ostrm << "\t\t\tversion_number: " << (int)pat.version_number() << endl;
	ostrm << "\t\t\tcurrent_next_indicator: " << pat.current_next_indicator() << endl;
	ostrm << "\t\t\tsection_number: " << (int)pat.section_number() << endl;
	ostrm << "\t\t\tlast_section_number: " << (int)pat.last_section_number() << endl;

	// print out the sections
	lcss::ProgramAssociationTable::const_iterator it;
	size_t sz = pat.size();
	size_t i = 1;
	ostrm << "\t\t\tprograms() {" << endl;
	for (it = pat.begin(); it != pat.end(); ++it, ++i)
	{
		ostrm << "\t\t\t\tprogram_number: " << it->second << endl;
		ostrm << "\t\t\t\tPID: " << it->first << endl;
	}
	ostrm << "\t\t\t}" << endl;

	val << "0x" << hex << pat.CRC_32();
	ostrm << "\t\t\tCRC_32: " << val.str() << endl;
	ostrm << "\t\t}" << endl;
}

void TsWriter::printPMT(std::ostream& ostrm, const lcss::ProgramMapTable& pmt)
{
	ostrm << "\t\tTS_program_map_section() {" << endl;

	ostrm << "\t\t\tpointer_field: " << (unsigned int)pmt.pointer_field() << endl;
	ostrm << "\t\t\ttable_id: " << (unsigned int)pmt.table_id() << endl;
	ostrm << "\t\t\tsection_syntax_indicator: " << pmt.section_syntax_indicator() << endl;
	ostrm << "\t\t\tsection_length: " << pmt.section_length() << endl;
	ostrm << "\t\t\tprogram_number: " << pmt.program_number() << endl;
	ostrm << "\t\t\tversion_number: " << (unsigned int)pmt.version_number() << endl;
	ostrm << "\t\t\tcurrent_next_indicator: " << pmt.current_next_indicator() << endl;
	ostrm << "\t\t\tsection_number: " << (unsigned int)pmt.section_number() << endl;
	ostrm << "\t\t\tlast_section_number: " << (unsigned int)pmt.last_section_number() << endl;
	ostrm << "\t\t\tPCR_PID: " << pmt.PCR_PID() << endl;
	ostrm << "\t\t\tprogram_info_length: " << pmt.program_info_length() << endl;
	ostrm << "\t\t\tprogram_info() {" << endl;

	if (pmt.program_info_length() > 0)
	{
		std::vector<lcss::Descriptor> program_info;
		pmt.program_infos(std::back_inserter(program_info));
		for (size_t i = 0; i < program_info.size(); ++i)
		{
			ostrm << "\t\t\t\tdescriptor() {" << endl;
			printDescriptorValue(ostrm, program_info[i], 5);
			ostrm << "\t\t\t\t}" << endl;
		}
	}
	ostrm << "\t\t\t}" << endl;

	ostrm << "\t\t\tprogram_elements() {" << endl;
	// Iterate over all the Program Elements
	for (const auto& pe : pmt)
	{
		string indent("\t\t\t\t\t");
		ostrm << "\t\t\t\t{" << endl;
		string descr = getStreamTypeDescription(pe.stream_type());

		std::stringstream tag;
		tag << "0x" << setfill('0') << setw(2) << hex << (int)pe.stream_type() << " (" << descr << ")";
		ostrm << indent << "stream_type: " << tag.str() << endl;
		ostrm << indent << "telementary_PID: " << pe.pid() << endl;
		ostrm << indent << "ES_info_length: " << pe.ES_info_length() << endl;

		// Iterate over all the Descriptors in a Program Element
		for (const auto& desc : pe)
		{
			ostrm << "\t\t\t\t\tdescriptor() {" << endl;
			printDescriptorValue(ostrm, desc, 6);
			ostrm << "\t\t\t\t\t}" << endl;
		}
		ostrm << "\t\t\t\t}" << endl;
	}
	ostrm << "\t\t\t}" << endl;

	stringstream crc;
	crc << "0x" << hex << pmt.CRC_32();
	ostrm << "\t\t\tCRC_32: " << crc.str() << endl;
	ostrm << "\t\t}" << endl;
}

void TsWriter::printPES(std::ostream& ostrm, const lcss::PESPacket& pes)
{
	string text = getStreamIdDescription(pes.stream_id());
	UINT16 pts_dts_flag = (pes.flags2() & PTS_DTS_MASK);
	std::stringstream val;
	std::stringstream ptsflag;
	string indent("\t\t\t");

	ostrm << "\t\tPES_packet() {" << endl;

	ostrm << indent << "packet_start_code_prefix: " << "0x000001" << endl;
	val << "0x" << hex << uppercase << (int)pes.stream_id() << " (" << text << ")";
	ostrm << indent << "stream_id: " << val.str() << endl;
	ostrm << indent << "PES_packet_length: " << pes.packet_length() << endl;
	ostrm << indent << "PES_scrambling_control: " << (pes.flags1() & 0x30 ? "true" : "false") << endl;
	ostrm << indent << "PES_priority: " << (pes.flags1() & 0x08 ? "true" : "false") << endl;
	ostrm << indent << "data_alignment_indicator: " << (pes.flags1() & 0x04 ? "true" : "false") << endl;
	ostrm << indent << "copyright: " << (pes.flags1() & 0x02 ? "true" : "false") << endl;
	ostrm << indent << "original_or_copy: " << (pes.flags1() & 0x01 ? "true" : "false") << endl;
	ptsflag << "0x" << hex << pts_dts_flag;
	ostrm << indent << "PTS_DTS_flags: " << ptsflag.str() << endl;
	ostrm << indent << "ESCR_flag: " << (pes.flags2() & 0x20 ? "true" : "false") << endl;
	ostrm << indent << "ES_rate_flag: " << (pes.flags2() & 0x10 ? "true" : "false") << endl;
	ostrm << indent << "DSM_trick_mode_flag: " << (pes.flags2() & 0x08 ? "true" : "false") << endl;
	ostrm << indent << "additional_copy_info_flag: " << (pes.flags2() & 0x04 ? "true" : "false") << endl;
	ostrm << indent << "PES_CRC_flag: " << (pes.flags2() & 0x02 ? "true" : "false") << endl;
	ostrm << indent << "PES_extension_flag: " << (pes.flags2() & 0x01 ? "true" : "false") << endl;
	ostrm << indent << "PES_header_data_length: " << (unsigned)pes.header_data_length() << endl;

	if (pts_dts_flag > 0)
	{
		if (pts_dts_flag == 0xC0)
		{
			ostrm.precision(12);
			std::stringstream pts;
			std::stringstream dts;
			pts.precision(12);
			dts.precision(12);

			pts << pes.pts() << "/90-kHz = " << pes.ptsInSeconds() << " seconds";
			dts << pes.dts() << "/90-kHz = " << pes.dtsInSeconds() << " seconds";
			ostrm << indent << "PTS: " << pts.str() << endl;
			ostrm << indent << "DTS: " << dts.str() << endl;
		}
		else if (pts_dts_flag == 0x80)
		{
			ostrm.precision(12);
			std::stringstream pts;
			pts.precision(12);

			pts << pes.pts() << "/90-kHz = " << pes.ptsInSeconds() << " seconds";
			ostrm << indent << "PTS: " << pts.str() << endl;
		}
	}
	ostrm << "\t\t}" << endl;
}

std::ostream& operator<<(std::ostream& ostrm, const lcss::ProgramAssociationTable& pat)
{
	TsWriter::printPAT(ostrm, pat);
	return ostrm;
}

std::ostream& operator<<(std::ostream& ostrm, const lcss::ProgramMapTable& pat)
{
	TsWriter::printPMT(ostrm, pat);
	return ostrm;
}

std::ostream& operator<<(std::ostream& ostrm, const lcss::PESPacket& pctk)
{
	TsWriter::printPES(ostrm, pctk);
	return ostrm;
}
