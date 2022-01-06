#pragma once

#include "tspmt.h"

#include <vector>

namespace lcss
{

/////////////////////////////////////////////////////////////////////////////
// Network Information Table
// Ref: ETSI EN 300 468 V1.11.1 (2010-04), page 19
class NetworkInformationTable
{
private:
	typedef std::vector<Descriptor> DescriptorArray;
public:
	class Stream
	{
	public:
		Stream()
			:transport_stream_id_(0)
			,original_network_id_(0)
		{}
		~Stream(){}

		UINT16 transport_stream_id_;
		UINT16 original_network_id_;
		NetworkInformationTable::DescriptorArray descriptors_;
	};
private:
	typedef std::vector<NetworkInformationTable::Stream> StreamArray;
public:
	NetworkInformationTable();
	~NetworkInformationTable();

	bool parse(const BYTE* table);

	BYTE	pointer_field()					const;
	BYTE	table_id()						const;
	UINT16	network_id()					const;
	BYTE	version_number()				const;
	bool	current_next_indicator()		const;
	BYTE	section_number()				const;
	BYTE	last_section_number()			const;
	UINT32	CRC_32()						const;

	template<class BackInsertIter>
	void network_descriptors(BackInsertIter backit) const
	{
		std::copy(network_descriptors_.begin(),
			network_descriptors_.end(),
			backit);
	}

	template<class BackInsertIter>
	void transport_streams(BackInsertIter backit) const
	{
		std::copy(streams_.begin(),
			streams_.end(),
			backit);
	}
private:
	std::vector<BYTE> nit_;
	UINT32 CRC_32_{};
	DescriptorArray network_descriptors_;
	StreamArray streams_;
};

}