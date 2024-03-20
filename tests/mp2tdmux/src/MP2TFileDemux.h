#pragma once

#include <mp2tp/libmp2tp.h>

#include <memory>
#include <fstream>

class AccessUnit;

namespace lcss
{
	class ProgramMapTable;
}

class MP2TFileDemux :
	public lcss::TSParser
{
	enum class STREAM_TYPE
	{
		UNKNOWN,
		H264,
		HDMV,
		VIDEO,
		AUDIO,
		KLVA,
		$EXI
	};

	typedef std::map<unsigned short, MP2TFileDemux::STREAM_TYPE> map_type;

public:
	MP2TFileDemux(
		std::ofstream& ofileh264,
		std::ofstream& ofileklv,
		std::ofstream& ofileaudio,
		std::ofstream& ofileexi);
	virtual ~MP2TFileDemux();

	//
	// TSParser overrides
	virtual void onPacket(lcss::TransportPacket& pckt);

private:

	void initTypeMap();
	STREAM_TYPE packetType(unsigned short pid);

private:
	lcss::ProgramAssociationTable			pat_;
	lcss::ProgramMapTable					pmt_;
	map_type								pid2type_;

public:
	std::ofstream& ofileH264_;
	std::ofstream& ofileKLV_;
	std::ofstream& ofileAudio_;
	std::ofstream& ofileEXI_;

	std::unique_ptr<AccessUnit>	curVideoSample_;
	std::unique_ptr<AccessUnit>	curAudioSample_;
	std::unique_ptr<AccessUnit>	curKlvSample_;
	std::unique_ptr<AccessUnit>	curExiSample_;
};