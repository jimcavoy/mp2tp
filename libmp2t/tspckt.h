#ifndef TSPCKT_H
#define TSPCKT_H

#include "tstype.h"

#include <memory>

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

	TransportPacket(TransportPacket&& src) noexcept = default;
	TransportPacket& operator=(TransportPacket&& rhs) noexcept = default;

	bool TEI() const; // Transport Error Indicator
	bool payloadUnitStart() const; // is PES or PSI otherwise false
	bool transportPriority() const;
	unsigned short PID() const;
	char scramblingControl() const;
	char adaptationFieldExist() const;
	char cc() const; // Continunity Counter

	const AdaptationField* getAdaptationField() const;
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
	void clear();

private:
	class Impl;
	std::unique_ptr<Impl> _pimpl;
};

}

#endif