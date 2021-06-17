#ifndef TSPCKT_H
#define TSPCKT_H

#include "tstype.h"

#include <memory>
#include <vector>

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
	void swap(TransportPacket& src);

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
	void parse(BYTE* buf);
	void push_back(BYTE b);

	// returns the full TS packet data.  Header + payload
	const BYTE* data() const;
	size_t length() const;

	void move(std::vector<BYTE>& seq);

private:
	std::vector<BYTE> _data;
	mutable std::unique_ptr<lcss::AdaptationField> _adptFd;
};

}

#endif