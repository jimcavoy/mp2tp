#include "MP2TFileDemux.h"

#include "AccessUnit.h"

#ifdef WIN32
#define strncpy strncpy_s
#endif


namespace
{
	char TAG_HDMV[] = { (char)0x48, (char)0x44, (char)0x4D, (char)0x56, (char)0xFF, (char)0x1B, (char)0x44, (char)0x3F, 0 };
	char TAG_HDPR[] = { (char)0x48, (char)0x44, (char)0x50, (char)0x52, (char)0xFF, (char)0x1B, (char)0x67, (char)0x3F, 0 };
}

MP2TFileDemux::MP2TFileDemux(
	std::ofstream& ofile,
	std::ofstream& ofileklv,
	std::ofstream& ofileaudio,
	std::ofstream& ofileexi)
	: ofileH264_(ofile)
	, ofileKLV_(ofileklv)
	, ofileAudio_(ofileaudio)
	, ofileEXI_(ofileexi)
{

}

MP2TFileDemux::~MP2TFileDemux()
{

}


void MP2TFileDemux::onPacket(lcss::TransportPacket& pckt)
{
	// Get the TS packet payload minus header
	const BYTE* data = pckt.getData();

	if (pckt.payloadUnitStart())
	{
		if (pckt.PID() == 0) // Program Association Table
		{
			pat_.parse(data);
		}
		else if (pat_.find(pckt.PID()) != pat_.end()) // Program Map Table
		{
			pmt_ = lcss::ProgramMapTable(data, pckt.data_byte());
			if (pmt_.parse())
			{
				initTypeMap();
			}
		}
		else
		{
			lcss::PESPacket pes;
			UINT16 bytesParsed = pes.parse(data);
			if (bytesParsed > 0)
			{
				switch (packetType(pckt.PID()))
				{
				case STREAM_TYPE::H264:
				case STREAM_TYPE::HDMV:
				{
					if (curVideoSample_.get() != nullptr && curVideoSample_->length() > 0)
					{
						ofileH264_.write(curVideoSample_->data(), curVideoSample_->length());
					}

					curVideoSample_ = std::make_unique<AccessUnit>();
					curVideoSample_->pts_ = pes.pts();
					curVideoSample_->dts_ = pes.dts();
					curVideoSample_->insert((char*)data + bytesParsed, pckt.data_byte() - bytesParsed);
				}
				break;
				case STREAM_TYPE::KLVA:
				{
					if (curKlvSample_.get() != nullptr && curKlvSample_->length() > 0)
					{
						// sync delivery of metadata using PES packets for carriage
						if (pes.stream_id() == 0xFC)
						{
							lcss::MetadataAUWrapper wrapper;
							wrapper.parse((BYTE*)curKlvSample_->data(), curKlvSample_->length());
							lcss::MetadataAUWrapper::const_iterator it;
							for (it = wrapper.begin(); it != wrapper.end(); ++it)
							{
								ofileKLV_.write((const char*)it->AU_cell_data_bytes(), it->AU_cell_data_length());
							}
						}
						// private_stream 1, async delivery of metadata
						else
							ofileKLV_.write(curKlvSample_->data(), curKlvSample_->length());
					}

					UINT64 pts = 0;
					if (curVideoSample_.get() != nullptr)
						pts = pes.pts() == 0 ? curVideoSample_->pts_ : pes.pts();

					curKlvSample_ = std::make_unique<AccessUnit>();
					curKlvSample_->insert((char*)data + bytesParsed, pckt.data_byte() - bytesParsed);
					curKlvSample_->pts_ = pts;
				}
				break;
				case STREAM_TYPE::$EXI:
				{
					if (curExiSample_.get() != nullptr && curExiSample_->length() > 0)
						ofileEXI_.write(curExiSample_->data(), curExiSample_->length());

					curExiSample_ = std::make_unique<AccessUnit>();
					curExiSample_->pts_ = pes.pts();
					curExiSample_->dts_ = pes.dts();
					curExiSample_->insert((char*)data + bytesParsed, pckt.data_byte() - bytesParsed);
				}
				break;
				case STREAM_TYPE::AUDIO:
				{
					if (curAudioSample_.get() != nullptr && curAudioSample_->length() > 0)
						ofileAudio_.write(curAudioSample_->data(), curAudioSample_->length());

					curAudioSample_ = std::make_unique<AccessUnit>();
					curAudioSample_->pts_ = pes.pts();
					curAudioSample_->dts_ = pes.dts();
					curAudioSample_->insert((char*)data + bytesParsed, pckt.data_byte() - bytesParsed);
				}
				break;
				}
			}
		}
	}
	else
	{
		auto it = pat_.find(pckt.PID());
		if (it != pat_.end() && it->second > 0)
		{
			pmt_.add(data, pckt.data_byte());
			if (pmt_.parse())
			{
				initTypeMap();
			}
		}

		switch (packetType(pckt.PID()))
		{
		case STREAM_TYPE::H264:
		case STREAM_TYPE::HDMV:
			if (curVideoSample_.get() != nullptr)
				curVideoSample_->insert((char*)data, pckt.data_byte());
			break;
		case STREAM_TYPE::AUDIO:
			if (curAudioSample_.get() != nullptr)
				curAudioSample_->insert((char*)data, pckt.data_byte());
			break;
		case STREAM_TYPE::KLVA:
			if (curKlvSample_.get() != nullptr)
				curKlvSample_->insert((char*)data, pckt.data_byte());
			break;
		case STREAM_TYPE::$EXI:
			if (curExiSample_.get() != nullptr)
				curExiSample_->insert((char*)data, pckt.data_byte());
			break;
		}
	}
}

void MP2TFileDemux::initTypeMap()
{
	for (auto pe : pmt_)
	{
		switch (pe.stream_type())
		{
		case 0x06:
		case 0x15:
		{
			char value[255]{};
			char format_identifier[5]{};

			for (auto desc : pe)
			{
				// registration_descriptor
				if (desc.tag() == 0x05)
				{
					desc.value((BYTE*)value);
					strncpy(format_identifier, value, 4);
					break;
				}
				// metadata_descriptor
				else if (desc.tag() == 0x26)
				{
					desc.value((BYTE*)value);
					strncpy(format_identifier, value + 3, 4);
					break;
				}
			}

			if (strcmp(format_identifier, "KLVA") == 0)
			{
				pid2type_.insert({ pe.pid(), STREAM_TYPE::KLVA });
			}
			else if (strcmp(format_identifier, "$EXI") == 0)
			{
				pid2type_.insert({ pe.pid(), STREAM_TYPE::$EXI });
			}
		}
		break;
		case 0x1B:
		{
			char value[255]{};
			for (auto desc : pe)
			{
				// registration_descriptor
				if (desc.tag() == 0x05)
				{
					desc.value((BYTE*)value);
					break;
				}
			}

			if (strcmp(value, TAG_HDMV) == 0 || strcmp(value, TAG_HDPR) == 0)
			{
				pid2type_.insert(map_type::value_type(pe.pid(), STREAM_TYPE::HDMV));
			}
			else
			{
				pid2type_.insert(map_type::value_type(pe.pid(), STREAM_TYPE::H264));
			}
		}
		break;
		case 0x02:
		{
			pid2type_.insert(map_type::value_type(pe.pid(), STREAM_TYPE::VIDEO));
		}
		break;
		case 0x03:
		case 0x04:
		case 0x0F:
		{
			pid2type_.insert(map_type::value_type(pe.pid(), STREAM_TYPE::AUDIO));
		}
		break;
		}
	}
}

MP2TFileDemux::STREAM_TYPE MP2TFileDemux::packetType(unsigned short pid)
{
	STREAM_TYPE type = STREAM_TYPE::UNKNOWN;

	map_type::iterator it = pid2type_.find(pid);
	if (it != pid2type_.end())
	{
		type = it->second;
	}

	return type;
}
