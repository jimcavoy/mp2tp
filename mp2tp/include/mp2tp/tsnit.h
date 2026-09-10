#pragma once

#include "tspmt.h"

#include <vector>
#include <memory>

namespace lcss
{

	/////////////////////////////////////////////////////////////////////////////
	// Network Information Table
	// Ref: ETSI EN 300 468 V1.11.1 (2010-04), page 19

	/// @brief NetworkInformationTable represents an instance defined in ETSI EN 300 468 V1.11.1 (2010-04), page 19
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

			uint16_t transport_stream_id_{ 0 };
			uint16_t original_network_id_{ 0 };
			NetworkInformationTable::DescriptorArray descriptors_;
		};
	private:
		typedef std::vector<NetworkInformationTable::Stream> StreamArray;
	public:
		NetworkInformationTable();
		~NetworkInformationTable();

		NetworkInformationTable(const NetworkInformationTable& other);
		NetworkInformationTable& operator=(const NetworkInformationTable& rhs);

		NetworkInformationTable(NetworkInformationTable&&) noexcept;
		NetworkInformationTable& operator=(NetworkInformationTable&&) noexcept;

		// Fields
		uint8_t		pointer_field()					const;
		uint8_t		table_id()						const;
		uint16_t	network_id()					const;
		uint8_t		version_number()				const;
		bool		current_next_indicator()		const;
		uint8_t		section_number()				const;
		uint8_t		last_section_number()			const;
		uint32_t	CRC_32()						const;

		// Methods
		bool parse(const uint8_t* table);

		template<typename BackInsertIter>
		void network_descriptors(BackInsertIter backit) const;

		template<typename BackInsertIter>
		void transport_streams(BackInsertIter backit) const;

	private:
		class Impl;
		std::unique_ptr<Impl> _pimpl;
	};

}