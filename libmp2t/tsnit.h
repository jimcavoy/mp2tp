#pragma once

#include "tspmt.h"

#include <vector>
#include <memory>

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
			Stream() {}
			~Stream() {}

			UINT16 transport_stream_id_{ 0 };
			UINT16 original_network_id_{ 0 };
			NetworkInformationTable::DescriptorArray descriptors_;
		};
	private:
		typedef std::vector<NetworkInformationTable::Stream> StreamArray;
	public:
		NetworkInformationTable();
		~NetworkInformationTable();

		NetworkInformationTable(const NetworkInformationTable& other);
		NetworkInformationTable& operator=(const NetworkInformationTable& rhs);

		NetworkInformationTable(NetworkInformationTable&&) noexcept = default;
		NetworkInformationTable& operator=(NetworkInformationTable&&) noexcept = default;

		// Fields
		BYTE	pointer_field()					const;
		BYTE	table_id()						const;
		UINT16	network_id()					const;
		BYTE	version_number()				const;
		bool	current_next_indicator()		const;
		BYTE	section_number()				const;
		BYTE	last_section_number()			const;
		UINT32	CRC_32()						const;

		// Methods
		bool parse(const BYTE* table);

		template<typename BackInsertIter>
		void network_descriptors(BackInsertIter backit) const;

		template<typename BackInsertIter>
		void transport_streams(BackInsertIter backit) const;

	private:
		class Impl;
		std::unique_ptr<Impl> _pimpl;
	};

}