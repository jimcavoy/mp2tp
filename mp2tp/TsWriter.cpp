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
	// Argument        : char* text
	// Argument        : unsigned int len
	void getDescriptorText(unsigned int tag, char* text, unsigned int len)
	{
		if (tag == 2)
			strcpy_s(text, len, "video_stream_descriptor");
		else if (tag == 3)
			strcpy_s(text, len, "audio_stream_descriptor");
		else if (tag == 4)
			strcpy_s(text, len, "hierarchy_descriptor");
		else if (tag == 5)
			strcpy_s(text, len, "registration_descriptor");
		else if (tag == 6)
			strcpy_s(text, len, "data_stream_alignment_descriptor");
		else if (tag == 7)
			strcpy_s(text, len, "target_background_grid_descriptor");
		else if (tag == 8)
			strcpy_s(text, len, "Video_window_descriptor");
		else if (tag == 9)
			strcpy_s(text, len, "CA_descriptor");
		else if (tag == 10)
			strcpy_s(text, len, "ISO_639_language_descriptor");
		else if (tag == 11)
			strcpy_s(text, len, "System_clock_descriptor");
		else if (tag == 12)
			strcpy_s(text, len, "Multiplex_buffer_utilization_descriptor");
		else if (tag == 13)
			strcpy_s(text, len, "Copyright_descriptor");
		else if (tag == 14)
			strcpy_s(text, len, "Maximum_bitrate_descriptor");
		else if (tag == 15)
			strcpy_s(text, len, "Private_data_indicator_descriptor");
		else if (tag == 16)
			strcpy_s(text, len, "Smoothing_buffer_descriptor");
		else if (tag == 17)
			strcpy_s(text, len, "STD_descriptor");
		else if (tag == 18)
			strcpy_s(text, len, "IBP_descriptor");
		else if (tag >= 19 && tag <= 26)
			strcpy_s(text, len, "Defined in ISO/IEC 13818-6");
		else if (tag == 27)
			strcpy_s(text, len, "MPEG-4_video_descriptor");
		else if (tag == 28)
			strcpy_s(text, len, "MPEG-4_audio_descriptor");
		else if (tag == 29)
			strcpy_s(text, len, "IOD_descriptor");
		else if (tag == 30)
			strcpy_s(text, len, "SL_descriptor");
		else if (tag == 31)
			strcpy_s(text, len, "FMC_descriptor");
		else if (tag == 32)
			strcpy_s(text, len, "external_ES_ID_descriptor");
		else if (tag == 33)
			strcpy_s(text, len, "MuxCode_descriptor");
		else if (tag == 34)
			strcpy_s(text, len, "FmxBufferSize_descriptor");
		else if (tag == 35)
			strcpy_s(text, len, "multiplexBuffer_descriptor");
		else if (tag == 36)
			strcpy_s(text, len, "content_labeling_descriptor");
		else if (tag == 37)
			strcpy_s(text, len, "metadata_pointer_descriptor");
		else if (tag == 38)
			strcpy_s(text, len, "metadata_descriptor");
		else if (tag == 39)
			strcpy_s(text, len, "metadata_STD_descriptor");
		else if (tag == 40)
			strcpy_s(text, len, "AVC video descriptor");
		else if (tag == 41)
			strcpy_s(text, len, "IPMP_descriptor");
		else if (tag == 42)
			strcpy_s(text, len, "AVC timing and HRD descriptor");
		else if (tag == 43)
			strcpy_s(text, len, "MPEG-2_AAC_audio_descriptor");
		else if (tag == 44)
			strcpy_s(text, len, "FlexMuxTiming_descriptor");
		else if (tag == 45)
			strcpy_s(text, len, "MPEG-4_text_descriptor");
		else if (tag == 46)
			strcpy_s(text, len, "MPEG-4_audio_extension_descriptor");
		else if (tag == 47)
			strcpy_s(text, len, "auxiliary_video_stream_descriptor");
		else if (tag == 48)
			strcpy_s(text, len, "SVC extension descriptor");
		else if (tag == 49)
			strcpy_s(text, len, "MVC extension descriptor");
		else if (tag == 50)
			strcpy_s(text, len, "J2K video descriptor");
		else if (tag == 51)
			strcpy_s(text, len, "MVC operation point descriptor");
		else if (tag == 52)
			strcpy_s(text, len, "MPEG2_stereoscopic_video_format_descriptor");
		else if (tag == 53)
			strcpy_s(text, len, "Stereoscopic_program_info_descriptor");
		else if (tag == 54)
			strcpy_s(text, len, "Stereoscopic_video_info_descriptor");
		else if (tag >= 55 && tag <= 63)
			strcpy_s(text, len, "Rec. ITU-T H.222.0 | ISO/IEC 13818-1 Reserved");
		else if (tag == 64)
			strcpy_s(text, len, "network_name_descriptor");
		else if (tag > 64 && tag <= 255)
			strcpy_s(text, len, "User Private");
	}

	// Function name   : getStreamTypeDescription
	// Description     : Returns a description of the stream type. 
	// Ref: ISO/IEC 13818-1 : 2013 (E) Table 2-34 - Stream type assignment, pg 51 
	// Return type     : void 
	// Argument        : unsigned int type
	// Argument        : char* descr
	// Argument        : unsigned int descrLen
	void getStreamTypeDescription(unsigned int type, char* descr, unsigned int descrLen)
	{
		if (type == 0x00)
			strcpy_s(descr, descrLen, "ITU-T | ISO/IEC Reserved");
		else if (type == 0x01)
			strcpy_s(descr, descrLen, "ISO/IEC 11172 Video");
		else if (type == 0x02)
			strcpy_s(descr, descrLen, "ITU-T Rec. H.262 | ISO/IEC 13818-2 Video or ISO/IEC 11172-2 constrained parameter video stream");
		else if (type == 0x03)
			strcpy_s(descr, descrLen, "ISO/IEC 11172 Audio");
		else if (type == 0x04)
			strcpy_s(descr, descrLen, "ISO/IEC 13818-3 Audio");
		else if (type == 0x05)
			strcpy_s(descr, descrLen, "Rec. ITU-T H.222.0 | ISO/IEC 13818-1 private_sections");
		else if (type == 0x06)
			strcpy_s(descr, descrLen, "Rec. ITU-T H.222.0 | ISO/IEC 13818-1 PES packets containing private data");
		else if (type == 0x07)
			strcpy_s(descr, descrLen, "ISO/IEC 13522 MHEG");
		else if (type == 0x08)
			strcpy_s(descr, descrLen, "Rec. ITU-T H.222.0 | ISO/IEC 13818-1 Annex A DSM-CC");
		else if (type == 0x09)
			strcpy_s(descr, descrLen, "Rec. ITU-T H.222.1");
		else if (type == 0x0A)
			strcpy_s(descr, descrLen, "ISO/IEC 13818-6 type A");
		else if (type == 0x0B)
			strcpy_s(descr, descrLen, "ISO/IEC 13818-6 type B");
		else if (type == 0x0C)
			strcpy_s(descr, descrLen, "ISO/IEC 13818-6 type C");
		else if (type == 0x0D)
			strcpy_s(descr, descrLen, "ISO/IEC 13818-6 type D");
		else if (type == 0x0E)
			strcpy_s(descr, descrLen, "ITU-T Rec. H.222.0 | ISO/IEC 13818-1 auxiliary");
		else if (type == 0x0F)
			strcpy_s(descr, descrLen, "ISO/IEC 13818-7 Audio with ADTS transport syntax");
		else if (type == 0x10)
			strcpy_s(descr, descrLen, "ISO/IEC 14496-2 Visual");
		else if (type == 0x11)
			strcpy_s(descr, descrLen, "ISO/IEC 14496-3 Audio with the LATM transport syntax as defined in ISO/IEC 14496-3 / AMD 1");
		else if (type == 0x12)
			strcpy_s(descr, descrLen, "ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in PES packets");
		else if (type == 0x13)
			strcpy_s(descr, descrLen, "ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in ISO/IEC14496_sections");
		else if (type == 0x14)
			strcpy_s(descr, descrLen, "ISO/IEC 13818-6 Synchronized Download Protocol");
		else if (type == 0x15)
			strcpy_s(descr, descrLen, "Metadata carried in PES packets");
		else if (type == 0x16)
			strcpy_s(descr, descrLen, "Metadata carried in metadata sections");
		else if (type == 0x17)
			strcpy_s(descr, descrLen, "Metadata carried in ISO/IEC 13818-6 Data Carousel");
		else if (type == 0x18)
			strcpy_s(descr, descrLen, "Metadata carried in ISO/IEC 13818-6 Object Carousel");
		else if (type == 0x19)
			strcpy_s(descr, descrLen, "Metadata carried in ISO/IEC 13818-6 Synchronized Download Protocol");
		else if (type == 0x1A)
			strcpy_s(descr, descrLen, "IPMP stream (defined in ISO/IEC 13818-11, MPEG-2 IPMP");
		else if (type == 0x1B)
			strcpy_s(descr, descrLen, "AVC video stream conforming to one or more profiles defined in Annex A of Rec. ITU-T H.264|ISO/IEC 14496-10 or AVC video sub-bitstream of SVC as defined in 2.1.78 or MVC base view sub-bitstream, as defined in 2.1.85, or AVC video sub-bitstream of MVC, as defined in 2.1.88");
		else if (type == 0x1C)
			strcpy_s(descr, descrLen, "ISO/IEC 14496-3 Audio, without using any additional transport syntax, such as DST, ALS and SLS");
		else if (type == 0x1D)
			strcpy_s(descr, descrLen, "ISO/IEC 14496-17 Text");
		else if (type == 0x1E)
			strcpy_s(descr, descrLen, "Auxiliary video stream as defined in ISO/IEC 23002-3");
		else if (type == 0x1F)
			strcpy_s(descr, descrLen, "SVC video sub-bitstream of an AVC video stream conforming to one or more profiles defined in Annex G of Rec. ITU-T H.264|ISO/IEC 14496-10");
		else if (type == 0x20)
			strcpy_s(descr, descrLen, "MVC video sub-bitstream of an AVC video stream conforming to one or more profiles defined in Annex H of Rec. ITU-T H.264|ISO/IEC 14496-10");
		else if (type == 0x21)
			strcpy_s(descr, descrLen, "Video stream conforming to one or more profiles as defined in Rec. ITU-T T.800|ISO/IEC 15444-1");
		else if (type == 0x22)
			strcpy_s(descr, descrLen, "Additional view Rec. ITU-T H.262|ISO/IEC 13818-2 video stream for service-compatible stereoscopic 3D services");
		else if (type == 0x23)
			strcpy_s(descr, descrLen, "Additional view Rec. ITU-T H.264|ISO/IEC 14496-10 video stream conforming to one or more profiles defined in Annex A for service-compatible stereoscopic 3D services");
		else if (type >= 0x24 || type <= 0x7e)
			strcpy_s(descr, descrLen, "Rec. ITU-T H.222.0 | ISO/IEC 13818-1 Reserved");
		else if (type == 0x7f)
			strcpy_s(descr, descrLen, "IPMP");
		else if (type >= 0x80 || type <= 0xFF)
			strcpy_s(descr, descrLen, "User Private");
	}

	// Function name   : getStreamIdDescription
	// Description     : ISO/IEC 13818-1 : 2013 (E) Table 2-22 Stream_id assignments, pg 36
	// Return type     : void 
	// Argument        : BYTE streamId
	// Argument        : char* type
	// Argument        : UINT32 sz
	void getStreamIdDescription(BYTE streamId, char* type, UINT32 sz)
	{
		if (streamId == 188)
			strcpy_s(type, sz, "program_stream_map");
		else if (streamId == 189)
			strcpy_s(type, sz, "private_stream_1");
		else if (streamId == 190)
			strcpy_s(type, sz, "padding_stream");
		else if (streamId == 191)
			strcpy_s(type, sz, "private_stream_2");
		else if (streamId >= 192 && streamId <= 223)
			strcpy_s(type, sz, "ISO/IEC 13818-3 or ISO/IEC 11172-3 or ISO/IEC 13818-7 or ISO/IEC 14496-3 audio stream number x xxxx");
		else if (streamId >= 224 && streamId <= 239)
			strcpy_s(type, sz, "ITU-T Rec. H.262 | ISO/IEC 13818-2, ISO/IEC 11172-2, ISO/IEC 14496-2 or Rec. ITU-T H.264 | ISO/IEC 14496-10 video stream number xxxx");
		else if (streamId == 240)
			strcpy_s(type, sz, "ECM_stream");
		else if (streamId == 241)
			strcpy_s(type, sz, "EMM_stream");
		else if (streamId == 242)
			strcpy_s(type, sz, "Rec. ITU-T H.222.0 | ISO/IEC 13818-1 Annex A or ISO/IEC 13818-6_DSMCC_stream");
		else if (streamId == 243)
			strcpy_s(type, sz, "ISO/IEC_13522_stream");
		else if (streamId == 244)
			strcpy_s(type, sz, "Rec. ITU-T H.222.1 type A");
		else if (streamId == 245)
			strcpy_s(type, sz, "Rec. ITU-T H.222.1 type B");
		else if (streamId == 246)
			strcpy_s(type, sz, "Rec. ITU-T H.222.1 type C");
		else if (streamId == 247)
			strcpy_s(type, sz, "Rec. ITU-T H.222.1 type D");
		else if (streamId == 248)
			strcpy_s(type, sz, "Rec. ITU-T H.222.1 type E");
		else if (streamId == 249)
			strcpy_s(type, sz, "ancillary_stream");
		else if (streamId == 250)
			strcpy_s(type, sz, "ISO/IEC14496-1_SL-packetized_stream");
		else if (streamId == 251)
			strcpy_s(type, sz, "ISO/IEC14496-1_FlexMux_stream");
		else if (streamId == 252)
			strcpy_s(type, sz, "(0xFC) metadata stream");
		else if (streamId == 253)
			strcpy_s(type, sz, "extended_stream_id");
		else if (streamId == 254)
			strcpy_s(type, sz, "reserved data stream");
		else if (streamId == 255)
			strcpy_s(type, sz, "program_stream_directory");
		else
			strcpy_s(type, sz, "ERROR UNKNOWN STREAM ID");
	}


	void printRegistrationDescriptor(const lcss::Descriptor& desc, int depth)
	{
		char text[BUFSIZ];
		std::stringstream tag;
		std::stringstream info;
		size_t cur = 0;

		BYTE val[BUFSIZ];
		desc.value(val);

		string indent;
		for (int i = 0; i < depth; i++)
		{
			indent += "\t";
		}

		memset(text, 0, BUFSIZ);
		getDescriptorText(desc.tag(), text, BUFSIZ);

		tag << "0x" << hex << setfill('0') << setw(2) << (int)desc.tag();
		cout << indent << "tag: " << tag.str() << " (" << text << ")" << endl;
		cout << indent << "length: " << (unsigned int)desc.length() << endl;

		while (cur < desc.length())
		{
			if (cur == 0) // format_identifier
			{
				sprintf_s(text, "%c%c%c%c (%#4.2x %#4.2x %#4.2x %#4.2x)", val[cur], val[cur + 1], val[cur + 2], val[cur + 3], val[cur], val[cur + 1], val[cur + 2], val[cur + 3]);
				cur += 4;
				cout << indent << "format_identifier: " << text << endl;
			}
			else
			{
				BYTE c = val[cur++];
				(isprint(c) != 0) ? info << (char)c : info << "(0x" << hex << setfill('0') << setw(2) << (int)c << ")";
			}
		}

		if (info.str().length() > 0)
		{
			cout << indent << "additional_identification_info: " << info.str() << endl;
		}
	}

	// ISO/IEC 13818-1 : 2013 (E), Table 2-83 - Metadata pointer descriptor, page 91
	void printMetadataPointerDescriptor(const lcss::Descriptor& desc)
	{
		char text[BUFSIZ];
		size_t cur = 0;
		BYTE metadata_format = 0;
		BYTE MPEG_carriage_flag = 0;
		string indent("\t\t\t\t");
		std::stringstream tag;

		BYTE val[BUFSIZ];
		desc.value(val);

		memset(text, 0, BUFSIZ);
		getDescriptorText(desc.tag(), text, BUFSIZ);
		cout << "\t\t\t" << text << "() {" << endl;

		tag << "0x" << hex << setfill('0') << setw(2) << (int)desc.tag();
		cout << indent << "tag: " << tag.str() << endl;
		cout << indent << "length: " << (unsigned int)desc.length() << endl;

		while (cur < desc.length())
		{
			if (cur == 0)
			{
				unsigned short metadata_application_format; // Table 2-81, page 89
				memcpy(&metadata_application_format, val, 2);
				metadata_application_format = ntohs(metadata_application_format);

				if (metadata_application_format >= 0 && metadata_application_format <= 0x000F)
					sprintf_s(text, "Reserved 0x%2.2x 0x%2.2x", val[0], val[1]);
				else if (metadata_application_format == 0x0010)
					strcpy_s(text, "ISO 15706 (ISAN) encoded in its binary form 0x00 0x10");
				else if (metadata_application_format == 0x0011)
					strcpy_s(text, "ISO 15706-2 (V-ISAN) encoded in its binary form 0x00 0x11");
				else if (metadata_application_format >= 0x0012 && metadata_application_format <= 0x00FF)
					sprintf_s(text, "Reserved 0x%2.2x 0x%2.2x", val[0], val[1]);
				else if (metadata_application_format >= 0x0100 && metadata_application_format <= 0xFFFE)
					sprintf_s(text, "User defined 0x%2.2x 0x%2.2x", val[0], val[1]);
				else if (metadata_application_format == 0xFFFF)
					sprintf_s(text, "Defined by the metadata_application_format_identifier field %#4.2x %#4.2x", val[0], val[1]);
				cur += 2;
				cout << indent << "metadata_application_format: " << text << endl;
			}
			else if (cur == 2)
			{
				metadata_format = val[2];

				if (metadata_format >= 0 && metadata_format <= 0x0F)
					sprintf_s(text, "Reserved %#4.2x", metadata_format);
				else if (metadata_format == 0x10)
					sprintf_s(text, "ISO/IEC 15938-1 TeM %#4.2x", metadata_format);
				else if (metadata_format == 0x11)
					sprintf_s(text, "ISO/IEC 15938-1 BiM %#4.2x", metadata_format);
				else if (metadata_format >= 0x12 && metadata_format <= 0x3E)
					sprintf_s(text, "Reserved %#4.2x", metadata_format);
				else if (metadata_format == 0x3F)
					strcpy_s(text, "Defined by metadata application format 0x3F");
				else if (metadata_format >= 0x40 && metadata_format <= 0xFE)
					sprintf_s(text, "Private use %#4.2x", metadata_format);
				else if (metadata_format == 0xFF)
					strcpy_s(text, "Defined by metadata_format_identifer field 0xFF");

				cur++;
				cout << indent << "metadata_format: " << text << endl;
			}
			else if (cur == 3)
			{
				if (metadata_format == 0xFF)
				{
					sprintf_s(text, "%c%c%c%c (%#4.2x %#4.2x %#4.2x %#4.2x)", val[cur], val[cur + 1], val[cur + 2], val[cur + 3], val[cur], val[cur + 1], val[cur + 2], val[cur + 3]);
					cur += 4;
					cout << indent << "metadata_format_identifier: " << text << endl;
				}
				else
					cur++;
			}
			else if ((cur == 4 && metadata_format != 0xFF) || cur == 7)
			{
				cout << indent << "metadata_service_id: " << (unsigned int)val[cur++] << endl;
			}
			else if ((cur == 5 && metadata_format != 0xFF) || cur == 8)
			{
				BYTE flag = val[cur++];
				char name[BUFSIZ];
				sprintf_s(name, "%#4.2x", flag);
				cout << indent << "metadata_locator_record_flag: " << (flag & 0x80 ? "true" : "false") << endl;

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

				cout << indent << "MPEG_carriage_flags: " << tag.str() << endl;
				cout << indent << "reserved: " << "11111" << endl;
			}
			else if ((cur == 6 && metadata_format != 0xFF) || cur == 9)
			{
				if (MPEG_carriage_flag == 0x00 || MPEG_carriage_flag == 0x20 || MPEG_carriage_flag == 0x40)
				{
					unsigned short program_number;
					memcpy(&program_number, val + cur, 2);
					program_number = ntohs(program_number);
					sprintf_s(text, "%d (0x%2.2x 0x%2.2x)", program_number, val[cur], val[cur + 1]);
					cout << indent << "program_number: " << text << endl;
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

					sprintf_s(text, "\"%d %#4.2x %#4.2x\",", transport_stream_location, val[cur], val[cur + 1]);
					cout << indent << "transport_stream_location: " << text << endl;

					sprintf_s(text, "\"%d %#4.2x %#4.2x\"", transport_stream_id, val[cur + 2], val[cur + 3]);
					cout << indent << "transport_stream_id: " << text << endl;
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
							sprintf_s(hexc, "%#4.2x (%c) ", c, c);
						else
							sprintf_s(hexc, "%#4.2x ", c);
					}
					else
						sprintf_s(hexc, "0x00 ");
					tag << hexc;
				}
				cout << indent << "private_data: " << tag.str() << endl;
			}
		}
		cout << "\t\t\t}" << endl;
	}

	// ISO/IEC 13818-1 : 2013 (E), Table 2-86 - Metadata descriptor, page 94
	void printMetadataDescriptor(const lcss::Descriptor& desc, int depth)
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
		BYTE val[BUFSIZ];
		desc.value(val);
		char text[BUFSIZ];

		getDescriptorText(desc.tag(), text, BUFSIZ);
		tag << "0x" << hex << setfill('0') << setw(2) << (int)desc.tag() << " (" << text << ")";
		cout << indent << "tag: " << tag.str() << endl;
		cout << indent << "length: " << (unsigned int)desc.length() << endl;

		while (cur < desc.length())
		{
			if (cur == 0)
			{
				unsigned short metadata_application_format; // Table 2-81, page 89
				memcpy(&metadata_application_format, val, 2);
				metadata_application_format = ntohs(metadata_application_format);

				if (metadata_application_format >= 0 && metadata_application_format <= 0x000F)
					sprintf_s(text, "Reserved %#4.2x %#4.2x", val[0], val[1]);
				else if (metadata_application_format == 0x0010)
					strcpy_s(text, "ISO 15706 (ISAN) encoded in its binary form 0x00 0x10");
				else if (metadata_application_format == 0x0011)
					strcpy_s(text, "ISO 15706-2 (V-ISAN) encoded in its binary form 0x00 0x11");
				else if (metadata_application_format >= 0x0012 && metadata_application_format <= 0x00FF)
					sprintf_s(text, "Reserved %#4.2x %#4.2x", val[0], val[1]);
				else if (metadata_application_format == 0x0100)
					sprintf_s(text, "MISB ST 1402 Table 4, General, 0x0100 (default)");
				else if (metadata_application_format == 0x0101)
					sprintf_s(text, "MISB ST 1402 Table 4, Geographic Metadata, 0x0101");
				else if (metadata_application_format == 0x0102)
					sprintf_s(text, "MISB ST 1402 Table 4, Annotation Metadata, 0x0102");
				else if (metadata_application_format == 0x0103)
					sprintf_s(text, "MISB ST 1402 Table 4, Still Image on Demand, 0x0103");
				else if (metadata_application_format > 0x0103 && metadata_application_format <= 0xFFFE)
					sprintf_s(text, "User defined %#4.2x %#4.2x", val[0], val[1]);
				else if (metadata_application_format == 0xFFFF)
					sprintf_s(text, "Defined by the metadata_application_format_identifier field %#4.2x %#4.2x", val[0], val[1]);

				cur += 2;

				cout << indent << "metadata_application_format: " << text << endl;
			}
			else if (cur == 2)
			{
				metadata_format = val[2];

				if (metadata_format >= 0 && metadata_format <= 0x0F)
					sprintf_s(text, "\"Reserved %#4.2x\"", metadata_format);
				else if (metadata_format == 0x10)
					sprintf_s(text, "\"ISO/IEC 15938-1 TeM %#4.2x\"", metadata_format);
				else if (metadata_format == 0x11)
					sprintf_s(text, "\"ISO/IEC 15938-1 BiM %#4.2x\"", metadata_format);
				else if (metadata_format >= 0x12 && metadata_format <= 0x3E)
					sprintf_s(text, "\"Reserved %#4.2x\"", metadata_format);
				else if (metadata_format == 0x3F)
					strcpy_s(text, "Defined by metadata application format 0x3F");
				else if (metadata_format >= 0x40 && metadata_format <= 0xFE)
					sprintf_s(text, "\"Private use %#4.2x\"", metadata_format);
				else if (metadata_format == 0xFF) {
					strcpy_s(text, "Defined by metadata_format_identifer field 0xFF");
				}
				cur++;

				cout << indent << "metadata_format: " << text << endl;
			}
			else if (cur == 3)
			{
				if (metadata_format == 0xFF)
				{
					sprintf_s(text, "%c%c%c%c (%#4.2x %#4.2x %#4.2x %#4.2x)", val[cur], val[cur + 1], val[cur + 2], val[cur + 3], val[cur], val[cur + 1], val[cur + 2], val[cur + 3]);
					cur += 4;
					cout << indent << "metadata_format_identifier: " << text << endl;
				}
				else
					cur++;
			}
			else if ((cur == 4 && metadata_format != 0xFF) || cur == 7)
			{
				cout << indent << "metadata_service_id: " << (unsigned int)val[cur++] << endl;
			}
			else if ((cur == 5 && metadata_format != 0xFF) || cur == 8)
			{
				tag = std::stringstream();
				char name[BUFSIZ];
				BYTE flag = val[cur++];
				sprintf_s(name, "%#4.2x", flag);
				BYTE mask = 0xE0;
				BYTE decoder_config_flags = flag & mask;

				if (decoder_config_flags == 0x00)
				{
					tag << "No decoder configuration is needed. 000" << endl;
					cout << indent << "decoder_config_flags: " << tag.str();
					BYTE DSM_CC_flag = flag & 0x10;
					cout << indent << "DSM-CC_flag: " << (DSM_CC_flag ? "true" : "false") << endl;
					cout << indent << "reserved: " << "1111" << endl;
				}
				else if (decoder_config_flags == 0x20)
				{
					tag << "The decoder configuration is carried in this descriptor in the decoder_config_byte field. 001";
					cout << indent << "decoder_config_flags" << tag.str() << endl;
					BYTE DSM_CC_flag = flag & 0x10;
					cout << indent << "DSM-CC_flag: " << (DSM_CC_flag ? "true" : "false") << endl;
					cout << indent << "reserved: " << "1111" << endl;

					BYTE decoder_config_length = val[cur++];
					cout << indent << "decoder_config_length: " << (unsigned int)decoder_config_length << endl;
					std::stringstream str;
					for (int i = 0; i < decoder_config_length; i++)
					{
						int c = (int)val[cur++];
						char hexc[32];
						if (c != 0) {
							if (isprint(c))
								sprintf_s(hexc, "%c", c);
							else
								sprintf_s(hexc, "(%#4.2x)", c);
						}
						else
							sprintf_s(hexc, "(0x00)");
						str << hexc;
					}
					cout << indent << "decoder_config_byte: " << str.str() << endl;
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
	void printMetadataSTDDescriptor(const lcss::Descriptor& desc, int depth)
	{
		std::stringstream tag;
		BYTE val[BUFSIZ];
		desc.value(val);
		char text[BUFSIZ];
		string indent;
		for (int i = 0; i < depth; i++)
		{
			indent += "\t";
		}


		getDescriptorText(desc.tag(), text, BUFSIZ);
		tag << "0x" << hex << setfill('0') << setw(2) << (int)desc.tag() << " (" << text << ")";
		cout << indent << "tag: " << tag.str() << endl;
		cout << indent << "length: " << (unsigned int)desc.length() << endl;

		size_t cur = 0;
		BYTE mask = 0xC0;

		while (cur < desc.length())
		{
			if (cur == 0) // metadata_input_leak_rate
			{
				BYTE a = val[cur++] ^ mask;
				signed int n;
				BYTE v[4];
				v[0] = 0; v[1] = a; v[2] = val[cur++]; v[3] = val[cur++];
				memcpy(&n, v, 4);
				n = ntohl(n);
				cout << indent << "metadata_input_leak_rate: " << n << endl;
			}
			else if (cur == 3) // metadata_buffer_size
			{
				BYTE a = val[cur++] ^ mask;
				signed int n;
				BYTE v[4];
				v[0] = 0; v[1] = a; v[2] = val[cur++]; v[3] = val[cur++];
				memcpy(&n, v, 4);
				n = ntohl(n);
				cout << indent << "metadata_buffer_size: " << n << endl;
			}
			else if (cur == 6) // metadata_output_leak_rate
			{
				BYTE a = val[cur++] ^ mask;
				signed int n;
				BYTE v[4];
				v[0] = 0; v[1] = a; v[2] = val[cur++]; v[3] = val[cur++];
				memcpy(&n, v, 4);
				n = ntohl(n);
				cout << indent << "metadata_output_leak_rate: " << n << endl;
			}
		}
	}

	void printDescriptorValue(const lcss::Descriptor& desc, int depth)
	{
		BYTE val[BUFSIZ];
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
			printRegistrationDescriptor(desc, depth);
			break;
		case 0x25: // metadata_pointer_descriptor
			printMetadataPointerDescriptor(desc);
			break;
		case 0x26: // metadata_descriptor
			printMetadataDescriptor(desc, depth);
			break;
		case 0x27: // metadata STD descriptor
			printMetadataSTDDescriptor(desc, depth);
			break;
		default:
		{
			char text[BUFSIZ];
			getDescriptorText(desc.tag(), text, BUFSIZ);
			tag << "0x" << hex << setfill('0') << setw(2) << (int)desc.tag() << " (" << text << ")";
			cout << indent << "tag: " << tag.str() << endl;
			cout << indent << "length: " << (unsigned int)desc.length() << endl;
			for (int n = 0; n < desc.length(); n++)
			{
				int c = (int)val[n];
				char hexc[32];
				if (c != 0) {
					if (isprint(c))
						sprintf_s(hexc, "%c", c);
					else
						sprintf_s(hexc, "(%#4.2x)", c);
				}
				else
					sprintf_s(hexc, "(0x00)");
				str << hexc;
			}
			cout << indent << "value: " << str.str() << endl;
		}
		}
	}

}

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
	cout << "\t\t\tpointer_field: " << (int)pat.pointer_field() << endl;
	cout << "\t\t\ttable_id: 0 (Program Association Section)" << endl;
	cout << "\t\t\tsection_length: " << pat.section_length() << endl;
	cout << "\t\t\ttransport_stream_id: " << pat.transport_stream_id() << endl;
	cout << "\t\t\tversion_number: " << (int)pat.version_number() << endl;
	cout << "\t\t\tcurrent_next_indicator: " << pat.current_next_indicator() << endl;
	cout << "\t\t\tsection_number: " << (int)pat.section_number() << endl;
	cout << "\t\t\tlast_section_number: " << (int)pat.last_section_number() << endl;

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
	cout << "\t\t\t}" << endl;

	val << "0x" << hex << pat.CRC_32();
	cout << "\t\t\tCRC_32: " << val.str() << endl;
	cout << "\t\t}" << endl;
}

void TsWriter::printPMT(const lcss::ProgramMapTable& pmt)
{
	cout << "\t\tTS_program_map_section() {" << endl;

	cout << "\t\t\tpointer_field: " << (unsigned int)pmt.pointer_field() << endl;
	cout << "\t\t\ttable_id: " << (unsigned int)pmt.table_id() << endl;
	cout << "\t\t\tsection_syntax_indicator: " << pmt.section_syntax_indicator() << endl;
	cout << "\t\t\tsection_length: " << pmt.section_length() << endl;
	cout << "\t\t\tprogram_number: " << pmt.program_number() << endl;
	cout << "\t\t\tversion_number: " << (unsigned int)pmt.version_number() << endl;
	cout << "\t\t\tcurrent_next_indicator: " << pmt.current_next_indicator() << endl;
	cout << "\t\t\tsection_number: " << (unsigned int)pmt.section_number() << endl;
	cout << "\t\t\tlast_section_number: " << (unsigned int)pmt.last_section_number() << endl;
	cout << "\t\t\tPCR_PID: " << pmt.PCR_PID() << endl;
	cout << "\t\t\tprogram_info_length: " << pmt.program_info_length() << endl;
	cout << "\t\t\tprogram_info() {" << endl;

	if (pmt.program_info_length() > 0)
	{
		std::vector<lcss::Descriptor> program_info;
		pmt.program_infos(std::back_inserter(program_info));
		for (size_t i = 0; i < program_info.size(); ++i)
		{
			cout << "\t\t\t\tdescriptor() {" << endl;
			printDescriptorValue(program_info[i], 5);
			cout << "\t\t\t\t}" << endl;
		}
	}
	cout << "\t\t\t}" << endl;

	cout << "\t\t\tprogram_elements() {" << endl;
	// Iterate over all the Program Elements
	for (auto pe : pmt)
	{
		string indent("\t\t\t\t\t");
		cout << "\t\t\t\t{" << endl;
		char descr[BUFSIZ];
		getStreamTypeDescription(pe.stream_type(), descr, BUFSIZ);

		std::stringstream tag;
		tag << "0x" << setfill('0') << setw(2) << hex << (int)pe.stream_type() << " (" << descr << ")";
		cout << indent << "stream_type: " << tag.str() << endl;
		cout << indent << "telementary_PID: " << pe.pid() << endl;
		cout << indent << "ES_info_length: " << pe.ES_info_length() << endl;

		// Iterate over all the Descriptors in a Program Element
		for (auto desc : pe)
		{
			cout << "\t\t\t\t\tdescriptor() {" << endl;
			printDescriptorValue(desc, 6);
			cout << "\t\t\t\t\t}" << endl;
		}
		cout << "\t\t\t\t}" << endl;
	}
	cout << "\t\t\t}" << endl;

	stringstream crc;
	crc << "0x" << hex << pmt.CRC_32();
	cout << "\t\t\tCRC_32: " << crc.str() << endl;
	cout << "\t\t}" << endl;
}

void TsWriter::printPES(const lcss::PESPacket& pes)
{
	char buf[BUFSIZ];
	getStreamIdDescription(pes.stream_id_, buf, BUFSIZ);
	UINT16 pts_dts_flag = (pes.flags2_ & PTS_DTS_MASK);
	std::stringstream val;
	std::stringstream ptsflag;
	string indent("\t\t\t");

	cout << "\t\tPES_packet() {" << endl;

	cout << indent << "packet_start_code_prefix: " << "0x000001" << endl;
	val << "0x" << hex << uppercase << (int)pes.stream_id_ << " (" << buf << ")";
	cout << indent << "stream_id: " << val.str() << endl;
	cout << indent << "PES_packet_length: " << pes.PES_packet_length_ << endl;
	cout << indent << "PES_scrambling_control: " << (pes.flags1_ & 0x30 ? "true" : "false") << endl;
	cout << indent << "PES_priority: " << (pes.flags1_ & 0x08 ? "true" : "false") << endl;
	cout << indent << "data_alignment_indicator: " << (pes.flags1_ & 0x04 ? "true" : "false") << endl;
	cout << indent << "copyright: " << (pes.flags1_ & 0x02 ? "true" : "false") << endl;
	cout << indent << "original_or_copy: " << (pes.flags1_ & 0x01 ? "true" : "false") << endl;
	ptsflag << "0x" << hex << pts_dts_flag;
	cout << indent << "PTS_DTS_flags: " << ptsflag.str() << endl;
	cout << indent << "ESCR_flag: " << (pes.flags2_ & 0x20 ? "true" : "false") << endl;
	cout << indent << "ES_rate_flag: " << (pes.flags2_ & 0x10 ? "true" : "false") << endl;
	cout << indent << "DSM_trick_mode_flag: " << (pes.flags2_ & 0x08 ? "true" : "false") << endl;
	cout << indent << "additional_copy_info_flag: " << (pes.flags2_ & 0x04 ? "true" : "false") << endl;
	cout << indent << "PES_CRC_flag: " << (pes.flags2_ & 0x02 ? "true" : "false") << endl;
	cout << indent << "PES_extension_flag: " << (pes.flags2_ & 0x01 ? "true" : "false") << endl;
	cout << indent << "PES_header_data_length: " << (unsigned)pes.PES_header_data_length_ << endl;

	if (pts_dts_flag > 0)
	{
		if (pts_dts_flag == 0xC0)
		{
			cout.precision(12);
			std::stringstream pts;
			std::stringstream dts;
			pts.precision(12);
			dts.precision(12);

			pts << pes.pts() << "/90-kHz = " << pes.ptsInSeconds() << " seconds";
			dts << pes.dts() << "/90-kHz = " << pes.dtsInSeconds() << " seconds";
			cout << indent << "PTS: " << pts.str() << endl;
			cout << indent << "DTS: " << dts.str() << endl;
		}
		else if (pts_dts_flag == 0x80)
		{
			cout.precision(12);
			std::stringstream pts;
			pts.precision(12);

			pts << pes.pts() << "/90-kHz = " << pes.ptsInSeconds() << " seconds";
			cout << indent << "PTS: " << pts.str() << endl;
		}
	}
	cout << "\t\t}" << endl;
}
