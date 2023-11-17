#include "tspacker.h"

#include <vector>
#include <WinSock2.h>

namespace
{
	BYTE tsheader_pes[] = {
		0x47, 0x40, 0x00, 0x10, 0x00, 0x00, 0x01, 0xBD,
		0x00, 0x00
	};

	BYTE tsheader_onepacket[] = {
		0x47, 0x40, 0x00, 0x30
	};

	BYTE tsheader[] = {
		0x47, 0x00, 0x00, 0x11
	};

	BYTE tsheader_payload_only[] = {
		0x47, 0x00, 0x00, 0x10
	};

	BYTE continuity_value[] = {
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
		0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
	};

	BYTE continuity_value_adaptation[] = {
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
		0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F
	};

	void setPID(BYTE* header, uint16_t pid)
	{
		BYTE bpid[2];
		memcpy(bpid, (void*)&pid, 2);
		bpid[0] = header[1] | bpid[0];
		memcpy(header + 1, (void*)&bpid, 2);
	}
}

namespace lcss
{
	class TSPacker::Impl
	{
	public:
		Impl()
		{

		};
	};

	TSPacker::TSPacker()
		:_pimpl(std::make_unique<lcss::TSPacker::Impl>())
	{

	}

	TSPacker::~TSPacker()
	{

	}

	lcss::TransportPacket TSPacker::packetize(uint8_t* buf, uint32_t bufsiz, uint16_t pid, uint8_t cc)
	{
		lcss::TransportPacket pckt;

		std::vector<uint8_t> rawByteSeq(lcss::TransportPacket::TS_SIZE, 0xFF);

		uint8_t tsh[4];
		memcpy(tsh, tsheader_onepacket, 4);
		uint16_t networkOrderPID = htons(pid);
		setPID(tsh, networkOrderPID);
		uint8_t c = continuity_value[cc % 16];
		tsh[3] = c;

		for (int i = 0; i < 4; i++)
		{
			rawByteSeq.emplace(rawByteSeq.begin() + i, tsh[i]);
		}

		for (uint32_t i = 0; i < bufsiz; i++)
		{
			rawByteSeq.emplace(rawByteSeq.begin() + (4+i), buf[i]);
		}

		pckt.parse(rawByteSeq.data());

		return pckt;
	}
}