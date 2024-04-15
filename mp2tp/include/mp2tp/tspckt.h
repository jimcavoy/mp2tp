#ifndef TSPCKT_H
#define TSPCKT_H

#include "tstype.h"

#include <memory>
#include <ostream>

namespace lcss
{

class AdaptationField;

/// @brief TransportPacket implements the transport_packet() as defined in
/// Ref: ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Reserved, Table 2-2 page 21
class TransportPacket
{
public:
	enum { TS_SIZE = 188};

public:
	TransportPacket();
	TransportPacket(const BYTE* data);
	TransportPacket(const BYTE* data, size_t len);

	~TransportPacket();

	TransportPacket(const TransportPacket& src);
	TransportPacket& operator=(const TransportPacket& rhs);

	TransportPacket(TransportPacket&& src) noexcept;
	TransportPacket& operator=(TransportPacket&& rhs) noexcept;

	bool TEI() const; ///< Transport Error Indicator
	bool payloadUnitStart() const; ///< Is true for PES or PSI otherwise false
	bool transportPriority() const;
	uint16_t PID() const; ///< PID is a 13-bit field indicating the type of the data stored in the packet payload.
	uint8_t scramblingControl() const;
	uint8_t adaptationFieldExist() const;
	uint8_t cc() const; ///<  Continuity Counter
	uint8_t incrementCC(); ///< Increment the continuity counter for this instance.

	const AdaptationField* getAdaptationField() const;
	BYTE data_byte() const; 

	// returns only the payload
	void getData(BYTE* data, int len) const;
	const BYTE* getData() const;

	void serialize(BYTE* buf, int len) const;
	void parse(const BYTE* buf);
	void push_back(BYTE b);

	const BYTE* data() const;
	size_t length() const;

private:
	class Impl;
	std::unique_ptr<Impl> _pimpl;

	friend std::ostream& operator<<(std::ostream& os, const lcss::TransportPacket& packet);
};

}

#endif