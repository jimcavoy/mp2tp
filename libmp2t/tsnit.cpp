#include "stdafx.h"
#include "tsnit.h"

#include <algorithm>
#include <iterator>

#ifndef WIN32
#include <memory.h>
#include <arpa/inet.h>
#endif

using namespace std;

const UINT16 NIT_SECTION_LEN = 0x03FF;
const BYTE NIT_VERSION_NO = 0x3E;

namespace lcss
{

	class NetworkInformationTable::Impl
	{
	public:
		Impl() {}
		~Impl() {}

	public:
		std::vector<BYTE> nit_;
		UINT32 CRC_32_{};
		NetworkInformationTable::DescriptorArray network_descriptors_;
		NetworkInformationTable::StreamArray streams_;
	};

	NetworkInformationTable::NetworkInformationTable()
	{
		_pimpl = std::make_unique<lcss::NetworkInformationTable::Impl>();
	}

	NetworkInformationTable::~NetworkInformationTable()
	{

	}

	bool NetworkInformationTable::parse(const BYTE* table)
	{
		if (table == nullptr)
			return false;

		_pimpl->nit_.push_back(table[0]);
		UINT16 offset = _pimpl->nit_[0] == 0 ? 1 : _pimpl->nit_[0]; // pointer_field
		UINT16 cur = offset;
		UINT16 value = 0;

		// stuffing bytes
		for (auto i = 1; i < _pimpl->nit_[0]; i++)
			_pimpl->nit_.push_back(table[i]);

		if (table[cur] != 0x40) // table_id must be 0x40
			return false;

		_pimpl->nit_.push_back(table[cur++]); // table_id

		memcpy(&value, table + cur, 2);
		UINT16 section_length = ntohs(value) & NIT_SECTION_LEN;
		_pimpl->nit_.push_back(table[cur++]); //section_syntax_indicator, reserved_future_use, reserved, section_length
		_pimpl->nit_.push_back(table[cur++]);
		UINT16 crcPos = cur + section_length - 4;
		UINT32 crc{};
		memcpy(&crc, table + crcPos, 4);
		_pimpl->CRC_32_ = ntohl(crc);

		_pimpl->nit_.push_back(table[cur++]); //network_id
		_pimpl->nit_.push_back(table[cur++]);

		_pimpl->nit_.push_back(table[cur++]); //reserved, version_number, current_next_indicator

		_pimpl->nit_.push_back(table[cur++]); //section_number

		_pimpl->nit_.push_back(table[cur++]); //last_section_number

		memcpy(&value, table + cur, 2);
		UINT16 network_descriptors_length = ntohs(value) & NIT_SECTION_LEN;
		_pimpl->nit_.push_back(table[cur++]); // reserved, network_descriptors_length
		_pimpl->nit_.push_back(table[cur++]);

		UINT16 end = network_descriptors_length + cur;

		while (cur < end)
		{
			BYTE bValue{};
			memcpy(&bValue, table + cur, 1);
			Descriptor desc(bValue);
			cur++;

			memcpy(&bValue, table + cur, 1);
			UINT16 len = bValue;
			cur++;

			if (len > 0)
			{
				desc.setValue(table + cur, len);
				cur += len;
			}

			lcss::eqDescriptor pred(desc.tag());
			lcss::NetworkInformationTable::DescriptorArray::iterator result = find_if(_pimpl->network_descriptors_.begin(), _pimpl->network_descriptors_.end(), pred);
			if (result == _pimpl->network_descriptors_.end()) // prevent adding duplicates
				_pimpl->network_descriptors_.push_back(desc);
		}

		memcpy(&value, table + cur, 2);
		UINT16 transport_stream_loop_length = ntohs(value) & NIT_SECTION_LEN;
		_pimpl->nit_.push_back(table[cur++]); // reserved_future_use, transport_stream_loop_length
		_pimpl->nit_.push_back(table[cur++]);

		end = transport_stream_loop_length + cur;

		while (cur < end)
		{
			NetworkInformationTable::Stream stream;
			memcpy(&value, table + cur, 2);
			stream.transport_stream_id_ = ntohs(value);
			cur += 2;

			memcpy(&value, table + cur, 2);
			stream.original_network_id_ = ntohs(value);
			cur += 2;

			memcpy(&value, table + cur, 2);
			UINT16 transport_descriptors_length = ntohs(value) & NIT_SECTION_LEN;
			cur += 2;

			UINT16 tsEnd = transport_descriptors_length + cur;

			while (cur < tsEnd)
			{
				BYTE bValue;
				memcpy(&bValue, table + cur, 1);
				Descriptor desc(bValue);
				cur++;

				memcpy(&bValue, table + cur, 1);
				UINT16 len = bValue;
				cur++;

				if (len > 0)
				{
					desc.setValue(table + cur, len);
					cur += len;
				}

				stream.descriptors_.push_back(desc);
			}
			_pimpl->streams_.push_back(stream);
		}

		return true;
	}

	BYTE NetworkInformationTable::pointer_field() const
	{
		return _pimpl->nit_[0];
	}

	BYTE NetworkInformationTable::table_id() const
	{
		UINT16 cur = _pimpl->nit_[0] == 0 ? 1 : _pimpl->nit_[0];
		if (cur > _pimpl->nit_.size() - 1)
			return 0xFF;
		return _pimpl->nit_[cur];
	}

	UINT16 NetworkInformationTable::network_id() const
	{
		UINT16 cur = _pimpl->nit_[0] == 0 ? 0 : _pimpl->nit_[0];
		cur += 4;
		BYTE chVal[2]{};
		UINT16 value;
		chVal[0] = _pimpl->nit_[cur++];
		chVal[1] = _pimpl->nit_[cur];
		memcpy(&value, chVal, 2);;
		return ntohs(value);
	}

	BYTE NetworkInformationTable::version_number() const
	{
		UINT16 cur = _pimpl->nit_[0] == 0 ? 0 : _pimpl->nit_[0];
		cur += 6;
		return (_pimpl->nit_[cur] & NIT_VERSION_NO) >> 1;
	}

	bool NetworkInformationTable::current_next_indicator() const
	{
		UINT16 cur = _pimpl->nit_[0] == 0 ? 0 : _pimpl->nit_[0];
		cur += 6;
		return (_pimpl->nit_[cur] & 0x01) == 0x01 ? true : false;
	}

	BYTE NetworkInformationTable::section_number() const
	{
		UINT16 cur = _pimpl->nit_[0] == 0 ? 0 : _pimpl->nit_[0];
		cur += 7;
		return _pimpl->nit_[cur];
	}

	BYTE NetworkInformationTable::last_section_number() const
	{
		UINT16 cur = _pimpl->nit_[0] == 0 ? 0 : _pimpl->nit_[0];
		cur += 8;
		return _pimpl->nit_[cur];
	}

	UINT32 NetworkInformationTable::CRC_32() const
	{
		return _pimpl->CRC_32_;
	}

	template<typename BackInsertIter>
	void NetworkInformationTable::network_descriptors(BackInsertIter backit) const
	{
		std::copy(_pimpl->network_descriptors_.begin(),
			_pimpl->network_descriptors_.end(),
			backit);
	}

	template<typename BackInsertIter>
	void NetworkInformationTable::transport_streams(BackInsertIter backit) const
	{
		std::copy(_pimpl->streams_.begin(),
			_pimpl->streams_.end(),
			backit);
	}

	template void NetworkInformationTable::network_descriptors<std::back_insert_iterator<std::vector<Descriptor>>>(std::back_insert_iterator<std::vector<Descriptor>>) const;

	template void NetworkInformationTable::transport_streams<std::back_insert_iterator<std::vector<NetworkInformationTable::Stream>>>(std::back_insert_iterator<NetworkInformationTable::StreamArray>) const;

} // lcss