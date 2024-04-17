#include <mp2tp/tspacker.h>

#include <cassert>
#include <vector>
#ifdef WIN32
#include <WinSock2.h>
#else
#include <memory.h>
#include <arpa/inet.h>
#endif

namespace
{
	BYTE tsheader_pes[] = {
		0x47, 0x40, 0x00, 0x10, 0x00, 0x00, 0x01, 0xBD,
		0x00, 0x00};

	BYTE tsheader_adpfd_pes[] = {
		0x47, 0x40, 0x00, 0x30, 0x07, 0x50,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x01, 0xBD, 0x00, 0x00};

	BYTE tsheader_onepacket[] = {
		0x47, 0x40, 0x00, 0x30};

	BYTE tsheader[] = {
		0x47, 0x00, 0x00, 0x11};

	BYTE tsheader_payload_only[] = {
		0x47, 0x00, 0x00, 0x10};

	BYTE continuity_value[] = {
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
		0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F};

	BYTE continuity_value_adaptation[] = {
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
		0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F};

	void setPID(BYTE *header, uint16_t pid)
	{
		uint16_t nPid = htons(pid);
		uint8_t bpid[2];
		memcpy(bpid, (void *)&nPid, 2);
		bpid[0] = header[1] | bpid[0];
		memcpy(header + 1, (void *)&bpid, 2);
	}

	void writePTS(uint8_t *bs, int fourbits, int64_t pts)
	{
		int val;

		val = fourbits << 4 | (((pts >> 30) & 0x07) << 1) | 1;
		*bs++ = val;
		val = (((pts >> 15) & 0x7fff) << 1) | 1;
		*bs++ = val >> 8;
		*bs++ = val;
		val = (((pts) & 0x7fff) << 1) | 1;
		*bs++ = val >> 8;
		*bs++ = val;
	}

	void writePcrBits(uint8_t *buf, int64_t pcr)
	{
		int64_t pcr_low = pcr % 300, pcr_high = pcr / 300;

		*buf++ = pcr_high >> 25;
		*buf++ = pcr_high >> 17;
		*buf++ = pcr_high >> 9;
		*buf++ = pcr_high >> 1;
		*buf++ = pcr_high << 7 | pcr_low >> 8 | 0x7e;
		*buf++ = pcr_low;
	}
}

namespace lcss
{
	class TSPacker::Impl
	{
	public:
		Impl(){

		};
	};

	TSPacker::TSPacker()
		: _pimpl(std::make_unique<lcss::TSPacker::Impl>())
	{
	}

	TSPacker::~TSPacker()
	{
	}

	lcss::TransportPacket TSPacker::packetize(const uint8_t *buf, size_t bufsiz, uint16_t pid, uint8_t cc)
	{
		assert(bufsiz < 185);
		lcss::TransportPacket pckt;

		std::vector<uint8_t> rawByteSeq(lcss::TransportPacket::TS_SIZE, 0xFF);

		uint8_t tsh[4];
		memcpy(tsh, tsheader_onepacket, 4);
		setPID(tsh, pid);
		uint8_t c = continuity_value[cc % 16];
		tsh[3] = c;

		for (int i = 0; i < 4; i++)
		{
			rawByteSeq.emplace(rawByteSeq.begin() + i, tsh[i]);
		}

		for (uint32_t i = 0; i < bufsiz; i++)
		{
			rawByteSeq.emplace(rawByteSeq.begin() + (4 + i), buf[i]);
		}

		pckt.parse(rawByteSeq.data());

		return pckt;
	}


	std::vector<lcss::TransportPacket> TSPacker::packetize(const uint8_t* buf, size_t bufsiz, uint16_t pid, size_t* cc)
	{
		std::vector<lcss::TransportPacket> tps;
		std::vector<uint8_t> tsd;
		size_t cur = 0;
		bool onepacket = true;

		while (cur < bufsiz)
		{
			if (tsd.size() == lcss::TransportPacket::TS_SIZE)
			{
				tps.push_back(lcss::TransportPacket(tsd.data(), tsd.size()));
				tsd.clear();
			}
			else if (tsd.size() + (bufsiz - cur) <= lcss::TransportPacket::TS_SIZE)
			{
				// There are two case:
				// 1. All KLV set can fit into one TS packet
				// 2. The KLV set is split across multiple TS packets
				if (onepacket) // case 1
				{
					uint8_t header[4];
					memcpy(header, tsheader_onepacket, 4);
					setPID(header, pid);
					// set continuity counter
					uint8_t c = continuity_value[*cc % 16];
					(*cc)++;
					header[3] = c;
					tsd.insert(tsd.end(), &header[0], &header[4]);
					// PMT 
					while (cur < bufsiz)
					{
						tsd.push_back(buf[cur++]);
					}
					// Stuffing
					size_t len = lcss::TransportPacket::TS_SIZE - tsd.size();
					for (size_t i = 0; i < len; i++)
					{
						tsd.push_back(0xFF);
					}
					tps.push_back(lcss::TransportPacket(tsd.data(), tsd.size()));
				}
				else // Case 2
				{
					// last packet
					// Setup the ts header and adaptation field
					uint8_t tsh[4];
					memcpy(tsh, tsheader, 4);
					setPID(tsh, pid);
					// set continuity_counter
					uint8_t c = continuity_value_adaptation[(*cc) % 16];
					(*cc)++;
					tsh[3] = c;
					// write the header
					tsd.insert(tsd.end(), &tsh[0], &tsh[4]);

					const uint16_t len = (uint16_t)188 - (5 + (bufsiz - cur));

					tsd.push_back((uint8_t)len);
					tsd.push_back(0);
					// fill with stuffing
					for (int i = 0; i < len - 1; ++i)
						tsd.push_back(0xFF);
					// filling the packet with data
					while (cur < bufsiz)
						tsd.push_back(buf[cur++]);
					tps.push_back(lcss::TransportPacket(tsd.data(), tsd.size()));
				}
			}
			// The data will fit across multiple TS Packets
			else if (tsd.size() == 0)
			{
				uint8_t tsh[4];
				memcpy(tsh, tsheader_payload_only, 4);
				if (onepacket) //first packet
				{
					onepacket = false;
					tsh[1] = 0x40; // set payload_unit_start_indicator to 1 (true)
				}
				setPID(tsh, pid);
				// set continuity_counter
				uint8_t c = continuity_value[*cc % 16];
				(*cc)++;
				tsh[3] = c;
				tsd.insert(tsd.end(), &tsh[0], &tsh[4]);
			}
			else
			{
				tsd.push_back(buf[cur++]);
			}
		}

		return tps;
	}


	std::vector<lcss::TransportPacket> TSPacker::packetizePES(const lcss::AccessUnit &au, uint16_t pid, size_t *cc, uint64_t pcr)
	{
		std::vector<lcss::TransportPacket> pes;
		size_t cur{};
		bool onepacket = true;
		int pesHeaderSize = 15; // TS Header + Adaptation Field + PES packet header (start code, stream id, PES packet length)
		if (au.PTS() > 0)
		{
			pesHeaderSize += 5;
		}
		if (au.DTS() > 0)
		{
			pesHeaderSize += 5;
		}

		std::vector<uint8_t> tsd;

		while (cur < au.length())
		{
			if (tsd.size() == lcss::TransportPacket::TS_SIZE)
			{
				pes.push_back(lcss::TransportPacket(tsd.data(), tsd.size()));
				tsd.clear();
			} // not enough data to fill a TS packet so add filler
			else if ((tsd.size() + (au.length() - cur) + pesHeaderSize) <= lcss::TransportPacket::TS_SIZE)
			{
				// There are two case:
				// 1. All KLV set can fit into one TS packet
				// 2. The KLV set is split across multiple TS packets
				if (onepacket) // case 1
				{
					// Setup the ts header and adaptation field
					uint8_t tsh[4];
					memcpy(tsh, tsheader_onepacket, 4);
					setPID(tsh, pid);
					// set continuity_counter
					uint8_t c = continuity_value_adaptation[(*cc) % 16];
					(*cc)++;
					tsh[3] = c;
					// write the header
					tsd.insert(tsd.end(), &tsh[0], &tsh[4]);

					uint16_t len = 189 - (pesHeaderSize + (uint16_t)au.length());

					tsd.push_back((uint8_t)len); // Adaptation field length
					tsd.push_back(0x40);		 // Adaptation Field Flag, Random Access = true

					// fill with stuffing
					for (int i = 0; i < len - 1; ++i)
						tsd.push_back(0xFF);

					tsd.push_back(0);
					tsd.push_back(0);
					tsd.push_back(1); // packet_start_code_prefix
					uint8_t stream_id = au.streamId();
					tsd.push_back(stream_id); // stream_id

					// PES_packet_length
					uint8_t bytePesLen[2]{};
					uint16_t peslen{};
					if (au.streamId() != lcss::AccessUnit::Video)
					{
						uint16_t pesLen = (uint16_t)au.length() + 3;
						if (au.PTS() > 0)
							pesLen += 5;
						if (au.DTS() > 0)
							pesLen += 5;

						peslen = htons(pesLen);
					}
					memcpy(bytePesLen, &peslen, 2);
					tsd.push_back(bytePesLen[0]);
					tsd.push_back(bytePesLen[1]);

					// PTS
					if (au.PTS() > 0)
					{
						uint8_t pts[5]{};
						tsd.push_back(0x80); // Flags
						tsd.push_back(0x80); // PTS present
						tsd.push_back(0x05); // PES_header_data_length = 5
						writePTS(pts, 0x80 >> 6, au.PTS());
						for (size_t i = 0; i < 5; i++)
						{
							tsd.push_back(pts[i]);
						}
					}
					else
					{
						tsd.push_back(0x85);
						tsd.push_back(0x00);
						tsd.push_back(0x00);
					}

					// filling the packet with data
					while (cur < au.length())
						tsd.push_back(au[cur++]);
					pes.push_back(lcss::TransportPacket(tsd.data(), tsd.size()));
					tsd.clear();
				}
				else // last packet
				{
					// Setup the ts header and adaptation field
					BYTE tsh[4];
					memcpy(tsh, tsheader, 4);
					setPID(tsh, pid);
					// set continuity_counter and adaption field control value
					BYTE c = continuity_value_adaptation[(*cc) % 16];
					(*cc)++;
					tsh[3] = c;
					// write the header
					tsd.insert(tsd.end(), &tsh[0], &tsh[4]);

					const uint16_t len = 188 - (5 + ((uint16_t)au.length() - (uint16_t)cur));

					// Adaptation field
					tsd.push_back((uint8_t)len);
					tsd.push_back(0);
					// fill with stuffing
					for (int i = 0; i < len - 1; ++i)
						tsd.push_back(0xFF);
					// filling the packet with data
					while (cur < au.length())
						tsd.push_back(au[cur++]);
					pes.push_back(lcss::TransportPacket(tsd.data(), tsd.size()));
					tsd.clear();
				}
				break;
			}
			else if (tsd.size() == 0)
			{
				if (onepacket && pcr == 0) // first packet
				{
					onepacket = false;
					pesHeaderSize = 6; // TS Header + Adaptation Field for the next iteration
					// write PES header
					uint8_t tshpes[10];
					memcpy(tshpes, tsheader_pes, 10);
					setPID(tshpes, pid);

					// set continuity_counter
					uint8_t c = continuity_value[*cc % 16];
					(*cc)++;
					tshpes[3] = c;

					// set stream_id
					tshpes[7] = au.streamId();

					// set PES_packet_length
					uint8_t s[2]{};
					uint16_t pes_len{};
					if (au.streamId() != lcss::AccessUnit::Video)
					{
						uint16_t pesLen = (uint16_t)au.length() + 3;
						if (au.PTS() > 0)
							pesLen += 5;
						if (au.DTS() > 0)
							pesLen += 5;

						pes_len = htons(pesLen);
					}
					memcpy(s, (void *)&pes_len, 2);
					tshpes[8] = s[0];
					tshpes[9] = s[1];
					tsd.insert(tsd.end(), &tshpes[0], &tshpes[10]);

					// PTS
					if (au.PTS() > 0)
					{
						uint8_t pts[5]{};
						tsd.push_back(0x80); // Flags
						tsd.push_back(0x80); // PTS present
						tsd.push_back(0x05); // PES_header_data_length = 5
						writePTS(pts, 0x80 >> 6, au.PTS());
						for (size_t i = 0; i < 5; i++)
						{
							tsd.push_back(pts[i]);
						}
					}
					else
					{
						tsd.push_back(0x85);
						tsd.push_back(0x00);
						tsd.push_back(0x00);
					}
				}
				else if (onepacket && pcr > 0)
				{
					onepacket = false;
					pesHeaderSize = 6; // TS Header + Adaptation Field for the next iteration
					// write PES header with adaptation field containing a PCR
					uint8_t tshpes[18];
					memcpy(tshpes, tsheader_adpfd_pes, 18);
					setPID(tshpes, pid);

					// set continuity_counter
					uint8_t c = continuity_value_adaptation[*cc % 16];
					(*cc)++;
					tshpes[3] = c;

					uint8_t pcrBuf[6]{};
					writePcrBits(pcrBuf, pcr);
					tshpes[6] = pcrBuf[0];
					tshpes[7] = pcrBuf[1];
					tshpes[8] = pcrBuf[2];
					tshpes[9] = pcrBuf[3];
					tshpes[10] = pcrBuf[4];
					tshpes[11] = pcrBuf[5];

					// set stream_id
					tshpes[15] = au.streamId();

					// set PES_packet_length
					uint8_t s[2]{};
					uint16_t pes_len{};
					if (au.streamId() != lcss::AccessUnit::Video)
					{
						uint16_t pesLen = (uint16_t)au.length() + 3;
						if (au.PTS() > 0)
							pesLen += 5;
						if (au.DTS() > 0)
							pesLen += 5;

						pes_len = htons(pesLen);
					}
					memcpy(s, (void *)&pes_len, 2);
					tshpes[16] = s[0];
					tshpes[17] = s[1];
					tsd.insert(tsd.end(), &tshpes[0], &tshpes[18]);

					// PTS
					if (au.PTS() > 0)
					{
						uint8_t pts[5]{};
						tsd.push_back(0x80); // Flags
						tsd.push_back(0x80); // PTS present
						tsd.push_back(0x05); // PES_header_data_length = 5
						writePTS(pts, 0x80 >> 6, au.PTS());
						for (size_t i = 0; i < 5; i++)
						{
							tsd.push_back(pts[i]);
						}
					}
					else
					{
						tsd.push_back(0x85);
						tsd.push_back(0x00);
						tsd.push_back(0x00);
					}
				}
				else // subsequent packets
				{
					uint8_t tsh[4];
					memcpy(tsh, tsheader_payload_only, 4);
					setPID(tsh, pid);

					// set continuity_counter
					BYTE c = continuity_value[*cc % 16];
					(*cc)++;
					tsh[3] = c;

					tsd.insert(tsd.end(), &tsh[0], &tsh[4]);
				}
			}
			else // default case
			{
				tsd.push_back(au[cur++]);
			}
		}

		return pes;
	}
}