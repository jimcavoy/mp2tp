#ifndef TSPRSR_H
#define TSPRSR_H

#include "tstype.h"

#include <memory>

namespace lcss
{

class TransportPacket;

/// <summary>
/// TSParser decompose an input MPEG-2 TS stream into Transport Stream packets.
/// The client application uses this class to create a demultiplex process. 
/// </summary>
class TSParser
{
public:
	TSParser();
	virtual ~TSParser();

	/// <summary>
	/// The function parses a MPEG-2 TS stream that is passed in as a raw 
	/// byte sequence.  It decompose the stream into Transport Packets 
	/// that is provided onPacket function.
	/// </summary>
	/// <param name="buf">The raw byte sequence of a MPEG-2 TS stream.</param>
	/// <param name="size">The size of the raw byte sequence.</param>
	virtual void parse(const BYTE* buf, UINT32 size);

	/// <summary>
	/// A callback function that returns the most recent Transport 
	/// Packet being parsed by the parse function.  Override this 
	/// function to handle this Transport Packet.
	/// </summary>
	/// <param name="pckt">The most recent Transport Packet being parsed.</param>
	virtual void onPacket(lcss::TransportPacket& pckt);

	/// <summary>
	/// Call this function to query how many Transport Packet have been parsed.
	/// </summary>
	/// <returns>The number of Transport Packet that was parsed.</returns>
	UINT64 packetCount() const;

	/// <summary>
	/// Set the Transport Packet size.  The default is 188 bytes.
	/// </summary>
	/// <param name="sz">The size of a Transport Packet.</param>
	void setPacketSize(UINT32 sz);

private:
	class TSParserImpl;
	std::unique_ptr<lcss::TSParser::TSParserImpl> _pimpl;
};

}

#endif