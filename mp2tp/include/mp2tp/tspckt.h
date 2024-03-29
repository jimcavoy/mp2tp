#ifndef TSPCKT_H
#define TSPCKT_H

#include "tstype.h"

#include <memory>
#include <ostream>

namespace lcss
{

class AdaptationField;

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

	bool TEI() const; // Transport Error Indicator
	bool payloadUnitStart() const; // is PES or PSI otherwise false
	bool transportPriority() const;
	uint16_t PID() const;
	uint8_t scramblingControl() const;
	uint8_t adaptationFieldExist() const;
	uint8_t cc() const; // Continuity Counter
	uint8_t incrementCC(); 

	const AdaptationField* getAdaptationField() const;
	// returns the payload size
	BYTE data_byte() const;

	// returns only the payload
	void getData(BYTE* data, int len) const;
	const BYTE* getData() const;

	void serialize(BYTE* buf, int len) const;
	void parse(const BYTE* buf);
	void push_back(BYTE b);

	// returns the full TS packet data.  Header + payload
	const BYTE* data() const;
	size_t length() const;

private:
	class Impl;
	std::unique_ptr<Impl> _pimpl;

	friend std::ostream& operator<<(std::ostream& os, const lcss::TransportPacket& packet);
};

}

#endif