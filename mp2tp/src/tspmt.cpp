#include "stdafx.h"
#include <mp2tp/tspmt.h>

#include <algorithm>
#include <iterator>

#ifndef WIN32
#include <memory.h>
#include <arpa/inet.h>
#endif

#ifdef _DEBUG_NEW
#define new DEBUG_CLIENTBLOCK
#endif

using namespace std;

const UINT16 PMT_SECTION_LEN = 0x03FF;
const BYTE PMT_VERSION_NO = 0x3E;
const UINT16 PMT_CUR_NEXT_INDICATOR = 0x0001;
const UINT16 PMT_PCR_PID = 0x1FFF;
const UINT16 PMT_PROGRAM_INFO_LEN = 0x0FFF;

unsigned int dvbpsi_crc32_table[256] =
{
  0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9,
  0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
  0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
  0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
  0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9,
  0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
  0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011,
  0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
  0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
  0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
  0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81,
  0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
  0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49,
  0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
  0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
  0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
  0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae,
  0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
  0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
  0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
  0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
  0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
  0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066,
  0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
  0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e,
  0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
  0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
  0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
  0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
  0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
  0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686,
  0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
  0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
  0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
  0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f,
  0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
  0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47,
  0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
  0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
  0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
  0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7,
  0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
  0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f,
  0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
  0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
  0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
  0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f,
  0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
  0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
  0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
  0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
  0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
  0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30,
  0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
  0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088,
  0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
  0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
  0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
  0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
  0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
  0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0,
  0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
  0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
  0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};

namespace
{
	char TAG_HDMV[] = { (char)0x48, (char)0x44, (char)0x4D, (char)0x56, (char)0xFF, (char)0x1B, (char)0x44, (char)0x3F, 0 };
	char TAG_HDPR[] = { (char)0x48, (char)0x44, (char)0x50, (char)0x52, (char)0xFF, (char)0x1B, (char)0x67, (char)0x3F, 0 };
}

namespace lcss
{
	class ProgramMapTable::Impl
	{
	public:
		Impl() {}
		~Impl() {}
		Impl(const Impl& other)
		{
			std::copy(other._pmt.begin(), other._pmt.end(), std::back_inserter(_pmt));
			std::copy(other._buffer.begin(), other._buffer.end(), std::back_inserter(_buffer));
			std::copy(other._program_info.begin(), other._program_info.end(), std::back_inserter(_program_info));
			std::copy(other._program_elmts.begin(), other._program_elmts.end(), std::back_inserter(_program_elmts));
		}

	public:
		void calcCRC();
		void calcLen();
		UINT16 programInfoLength() const;
		void set_section_length(UINT16 len);

		template<class BackInsertIter>
		void serialize(BackInsertIter backit) const;

	public:
		std::vector<BYTE> _pmt;
		std::vector<BYTE> _buffer;
		uint32_t CRC_32_{0};
		ProgramMapTable::DescriptorArray _program_info;
		ProgramMapTable::MapType _program_elmts;
	};

	uint8_t ProgramMapTable::default_seq[17] = {
			0x00,					// pointer_field
			0x02,					// table_id
			0xB0, 0x0D,				// section_syntax_indicator, '0', reserved, section_length
			0x00, 0x01,				// program_number
			0xC1,					// reserved, version_number, current_next_indicator
			0x00,					// section_number
			0x00,					// last_section_number
			0xE1, 0x00,				// reserved, PCR_PID
			0xF0, 0x00,				// reserved, program_info_length
			0x00, 0x00, 0x00, 0x00	// CRC_32
	};
}

/////////////////////////////////////////////////////////////////////////////
	// ProgramMapTable::serialize
template<class BackInsertIter>
void lcss::ProgramMapTable::Impl::serialize(BackInsertIter backit) const
{
	std::copy(_pmt.begin(), _pmt.end(), backit);
	if (programInfoLength() > 0)
	{
		BYTE val[BUFSIZ]{};
		for (const lcss::Descriptor& descr : _program_info)
		{
			*++backit = descr.tag();
			*++backit = descr.length();
			descr.value(val);
			for (UINT32 i = 0; i < descr.length(); i++)
				*++backit = val[i];
		}
	}

	for (const lcss::ProgramElement& pe : _program_elmts)
	{
		BYTE bValue[2]{};
		*++backit = pe.stream_type();
		UINT16 value = htons(pe.pid());
		memcpy(bValue, &value, 2);
		BYTE b = bValue[0] | 0xE0;
		*++backit = b;
		*++backit = bValue[1];
		value = htons(pe.raw_ES_info_length());
		memcpy(bValue, &value, 2);
		*++backit = bValue[0];
		*++backit = bValue[1];
		if (pe.ES_info_length() > 0)
		{
			for (const lcss::Descriptor& d : pe)
			{
				BYTE val[BUFSIZ]{};
				*++backit = d.tag();
				*++backit = d.length();
				d.value(val);
				for (UINT32 i = 0; i < d.length(); i++)
				{
					*++backit = val[i];
				}
			}
		}
	}

	*++backit = (CRC_32_ >> 24) & 0xff;
	*++backit = (CRC_32_ >> 16) & 0xff;
	*++backit = (CRC_32_ >> 8) & 0xff;
	*++backit = (CRC_32_) & 0xff;
}

/////////////////////////////////////////////////////////////////////////////
// ProgramMapTable
lcss::ProgramMapTable::ProgramMapTable()
	:_pimpl(std::make_unique<lcss::ProgramMapTable::Impl>())
{

}

lcss::ProgramMapTable::ProgramMapTable(const BYTE* buffer, int len)
	:_pimpl(std::make_unique<lcss::ProgramMapTable::Impl>())
{
	add(buffer, len);
}

lcss::ProgramMapTable::~ProgramMapTable()
{

}

lcss::ProgramMapTable::ProgramMapTable(const ProgramMapTable& other)
	:_pimpl(std::make_unique<lcss::ProgramMapTable::Impl>(*other._pimpl))
{

}

lcss::ProgramMapTable& lcss::ProgramMapTable::operator=(const ProgramMapTable& rhs)
{
	if (this != &rhs)
	{
		_pimpl.reset(new lcss::ProgramMapTable::Impl(*rhs._pimpl));
	}
	return *this;
}

lcss::ProgramMapTable::ProgramMapTable(lcss::ProgramMapTable&& other) noexcept
{
	*this = std::move(other);
}

lcss::ProgramMapTable& lcss::ProgramMapTable::operator=(lcss::ProgramMapTable&& rhs) noexcept
{
	if (this != &rhs)
	{
		_pimpl = std::move(rhs._pimpl);
	}
	return *this;
}

void lcss::ProgramMapTable::add(const BYTE* buffer, int len)
{
	std::copy(buffer, buffer + len, std::back_inserter(_pimpl->_buffer));
}

bool lcss::ProgramMapTable::canParse() const
{
	UINT16 len = section_length();

	if (len == 0)
	{
		return false;
	}

	if (len < 184)
	{
		return true;
	}

	return (_pimpl->_buffer.size() - 4) >= len ? true : false;
}

// Function name   : ProgramMapTable::parse
// Description     : This function implements the definition 
// of a TS_program_map_section() as outline in ISO/IEC 13818-1:2013(E) 
// Table 2-33 page 50.  Returns true if operation
// successful otherwise false
// Return type     : bool 
bool lcss::ProgramMapTable::parse()
{
	if (!canParse())
		return false;

	_pimpl->_pmt.push_back(_pimpl->_buffer[0]);
	UINT16 offset = _pimpl->_pmt[0] == 0 ? 1 : _pimpl->_pmt[0]; // pointer_field
	UINT16 cur = offset;
	UINT16 value = 0;

	// stuffing bytes
	for (size_t i = 1; i < _pimpl->_pmt[0]; i++)
		_pimpl->_pmt.push_back(_pimpl->_buffer[i]);

	if (_pimpl->_buffer[cur] != 2) // table_id must equal 0x02
		return false;

	_pimpl->_pmt.push_back(_pimpl->_buffer[cur++]); // table_id

	memcpy(&value, _pimpl->_buffer.data() + cur, 2);
	UINT16 section_length = ntohs(value) & PMT_SECTION_LEN;
	_pimpl->_pmt.push_back(_pimpl->_buffer[cur++]); // section_syntax_indicator, reserved, section_length
	_pimpl->_pmt.push_back(_pimpl->_buffer[cur++]);
	UINT16 crcPos = cur + section_length - 4;
	UINT32 crc{};
	memcpy(&crc, _pimpl->_buffer.data() + crcPos, 4);
	_pimpl->CRC_32_ = ntohl(crc);

	_pimpl->_pmt.push_back(_pimpl->_buffer[cur++]); // program_number
	_pimpl->_pmt.push_back(_pimpl->_buffer[cur++]);

	_pimpl->_pmt.push_back(_pimpl->_buffer[cur++]); // reserved, version_number, current_next_indicator

	_pimpl->_pmt.push_back(_pimpl->_buffer[cur++]); // section_number
	_pimpl->_pmt.push_back(_pimpl->_buffer[cur++]); // last_section_number

	_pimpl->_pmt.push_back(_pimpl->_buffer[cur++]); // reserved, PCR_PID
	_pimpl->_pmt.push_back(_pimpl->_buffer[cur++]);

	memcpy(&value, _pimpl->_buffer.data() + cur, 2);
	UINT16 program_info_length = ntohs(value) & PMT_SECTION_LEN;
	BYTE b = _pimpl->_buffer[cur++];
	BYTE b1 = b | 0xF0;
	b1 = b1 & 0xF3;
	_pimpl->_pmt.push_back(b1); // reserved, program_info_length
	_pimpl->_pmt.push_back(_pimpl->_buffer[cur++]);

	UINT16 end = program_info_length + cur;

	while (cur < end)
	{
		BYTE bValue;
		memcpy(&bValue, _pimpl->_buffer.data() + cur, 1);
		Descriptor desc(bValue);
		cur++;

		memcpy(&bValue, _pimpl->_buffer.data() + cur, 1);
		UINT16 len = bValue;
		cur++;

		if (len > 0)
		{
			desc.setValue(_pimpl->_buffer.data() + cur, len);
			cur += len;
		}

		lcss::eqDescriptor pred(desc.tag());
		lcss::ProgramMapTable::DescriptorArray::iterator result = find_if(_pimpl->_program_info.begin(), _pimpl->_program_info.end(), pred);
		if (result == _pimpl->_program_info.end()) // prevent adding duplicates
			_pimpl->_program_info.push_back(desc);
	}

	while (cur < crcPos)
	{
		BYTE byte;
		memcpy(&byte, _pimpl->_buffer.data() + cur, 1);
		BYTE stype = byte;
		cur++;

		memcpy(&value, _pimpl->_buffer.data() + cur, 2);
		UINT16 elem_pid_ = ntohs(value) & PMT_PCR_PID;
		cur += 2;

		memcpy(&value, _pimpl->_buffer.data() + cur, 2);
		UINT16 ES_info_len = ntohs(value);

		lcss::ProgramElement pe(stype, elem_pid_);
		UINT16 len = ES_info_len & PMT_SECTION_LEN;
		cur += 2;
		int i = 0;

		while (i < len) // retrieve the descriptors
		{
			BYTE value8;
			memcpy(&value8, _pimpl->_buffer.data() + cur, 1);
			Descriptor desc(value8);
			cur++; i++;
			value8 = 0;

			memcpy(&value8, _pimpl->_buffer.data() + cur, 1);
			UINT16 desc_len = value8;
			cur++; i++;

			if (desc_len > 0)
			{
				desc.setValue(_pimpl->_buffer.data() + cur, desc_len);
				cur += desc_len;
				i += desc_len;
			}
			pe.addDescriptor(desc);
		}
		_pimpl->_program_elmts.push_back(pe);
	}
	// calculate CRC because reserved bits that are set to 0 are now set to 1
	// as per standard
	_pimpl->calcCRC(); 
	return true;
}

bool lcss::ProgramMapTable::hasPCR(UINT16 pid) const
{
	return pid == PCR_PID();
}

void lcss::ProgramMapTable::addProgramElement(const lcss::ProgramElement& pe)
{
	_pimpl->_program_elmts.push_back(pe);

	_pimpl->calcLen();
	_pimpl->calcCRC();
}

void lcss::ProgramMapTable::removeProgramElement(const lcss::ProgramElement& pe)
{
	MapType::iterator it;
	for (it = _pimpl->_program_elmts.begin(); it != _pimpl->_program_elmts.end(); ++it)
	{
		if (pe.stream_type() == it->stream_type() && pe.pid() == it->pid())
		{
			_pimpl->_program_elmts.erase(it);
			break;
		}
	}

	_pimpl->calcLen();
	_pimpl->calcCRC();
}

BYTE lcss::ProgramMapTable::pointer_field() const
{
	return _pimpl->_pmt[0];
}

BYTE lcss::ProgramMapTable::table_id() const
{
	UINT16 cur = _pimpl->_pmt[0] == 0 ? 1 : _pimpl->_pmt[0];
	if (cur > _pimpl->_pmt.size() - 1)
		return 0xFF;
	return _pimpl->_pmt[cur];
}

bool lcss::ProgramMapTable::section_syntax_indicator() const
{
	UINT16 cur = _pimpl->_pmt[0] == 0 ? 0 : _pimpl->_pmt[0];
	cur += 2;
	bool ret = (_pimpl->_pmt[cur] & 0x80) == 0x80 ? true : false;
	return ret;
}

UINT16 lcss::ProgramMapTable::section_length() const
{
	// pre-condition
	if (_pimpl->_buffer.empty())
	{
		return 0;
	}

	UINT16 sl = 0;
	if (_pimpl->_pmt.empty())
	{
		UINT16 cur = _pimpl->_buffer[0] == 0 ? 0 : _pimpl->_buffer[0];
		cur += 2;
		BYTE chVal[2]{};
		UINT16 value;
		if (cur > _pimpl->_buffer.size() || cur + 1 > _pimpl->_buffer.size())
		{
			return 0;
		}
		chVal[0] = _pimpl->_buffer[cur++];
		chVal[1] = _pimpl->_buffer[cur];
		memcpy(&value, chVal, 2);
		sl = ntohs(value) & PMT_SECTION_LEN;
	}
	else
	{
		UINT16 cur = _pimpl->_pmt[0] == 0 ? 0 : _pimpl->_pmt[0];
		cur += 2;
		BYTE chVal[2]{};
		UINT16 value;
		if (cur > _pimpl->_buffer.size() || cur + 1 > _pimpl->_buffer.size())
		{
			return 0;
		}
		chVal[0] = _pimpl->_pmt[cur++];
		chVal[1] = _pimpl->_pmt[cur];
		memcpy(&value, chVal, 2);
		sl = ntohs(value) & PMT_SECTION_LEN;
	}
	return sl;
}

void lcss::ProgramMapTable::Impl::set_section_length(UINT16 len)
{
	BYTE chLen[2];
	UINT16 nLen = htons(len);
	memcpy(chLen, &nLen, 2);
	UINT16 cur = _pmt[0] == 0 ? 0 : _pmt[0];
	cur += 2;
	_pmt[cur++] = chLen[0];
	_pmt[cur] = chLen[1];
}

UINT16 lcss::ProgramMapTable::program_number() const
{
	UINT16 cur = _pimpl->_pmt[0] == 0 ? 0 : _pimpl->_pmt[0];
	cur += 4;
	BYTE chVal[2]{};
	UINT16 value;
	chVal[0] = _pimpl->_pmt[cur++];
	chVal[1] = _pimpl->_pmt[cur];
	memcpy(&value, chVal, 2);
	return ntohs(value);
}

BYTE lcss::ProgramMapTable::version_number() const
{
	UINT16 cur = _pimpl->_pmt[0] == 0 ? 0 : _pimpl->_pmt[0];
	cur += 6;
	return (_pimpl->_pmt[cur] & PMT_VERSION_NO) >> 1;
}

bool lcss::ProgramMapTable::current_next_indicator() const
{
	UINT16 cur = _pimpl->_pmt[0] == 0 ? 0 : _pimpl->_pmt[0];
	cur += 6;
	return (_pimpl->_pmt[cur] & 0x01) == 0x01 ? true : false;
}

BYTE lcss::ProgramMapTable::section_number() const
{
	UINT16 cur = _pimpl->_pmt[0] == 0 ? 0 : _pimpl->_pmt[0];
	cur += 7;
	return _pimpl->_pmt[cur];
}

BYTE lcss::ProgramMapTable::last_section_number() const
{
	UINT16 cur = _pimpl->_pmt[0] == 0 ? 0 : _pimpl->_pmt[0];
	cur += 8;
	return _pimpl->_pmt[cur];
}

UINT16 lcss::ProgramMapTable::PCR_PID() const
{
	UINT16 cur = _pimpl->_pmt[0] == 0 ? 0 : _pimpl->_pmt[0];
	cur += 9;
	BYTE chVal[2]{};
	UINT16 value;
	chVal[0] = _pimpl->_pmt[cur++];
	chVal[1] = _pimpl->_pmt[cur];
	memcpy(&value, chVal, 2);
	UINT16 pid = ntohs(value) & PMT_PCR_PID;
	return pid;
}

UINT16 lcss::ProgramMapTable::program_info_length() const
{
	return _pimpl->programInfoLength();
}

UINT16 lcss::ProgramMapTable::Impl::programInfoLength() const
{
	UINT16 cur = _pmt[0] == 0 ? 0 : _pmt[0];
	cur += 11;
	BYTE chVal[2]{};
	UINT16 value;
	chVal[0] = _pmt[cur++];
	chVal[1] = _pmt[cur];
	memcpy(&value, chVal, 2);
	UINT16 len = ntohs(value) & PMT_PROGRAM_INFO_LEN;
	return len;
}

UINT32 lcss::ProgramMapTable::CRC_32() const
{
	return _pimpl->CRC_32_;
}

lcss::ProgramMapTable::MapType::iterator lcss::ProgramMapTable::begin()
{
	return _pimpl->_program_elmts.begin();
}

lcss::ProgramMapTable::MapType::iterator lcss::ProgramMapTable::end()
{
	return _pimpl->_program_elmts.end();
}

lcss::ProgramMapTable::MapType::const_iterator lcss::ProgramMapTable::begin() const
{
	return _pimpl->_program_elmts.begin();
}

lcss::ProgramMapTable::MapType::const_iterator lcss::ProgramMapTable::end() const
{
	return _pimpl->_program_elmts.end();
}


void lcss::ProgramMapTable::Impl::calcCRC()
{
	std::vector<BYTE> pmt;
	serialize(std::back_inserter(pmt));
	CRC_32_ = 0xffffffff;

	for (size_t i = 1; i < pmt.size() - 4; i++)
	{
		CRC_32_ = (CRC_32_ << 8) ^ dvbpsi_crc32_table[(CRC_32_ >> 24) ^ (pmt[i])];
	}
}

void lcss::ProgramMapTable::Impl::calcLen()
{
	UINT16 len = 13; // 9 for the pmt header + 4 for CRC

	ProgramMapTable::iterator it;

	for (it = _program_elmts.begin(); it != _program_elmts.end(); ++it)
	{
		len += 5;
		len += it->ES_info_length();
	}

	len += programInfoLength();
	len = len | 0xB000;
	set_section_length(len);
}

/////////////////////////////////////////////////////////////////////////////
// Descriptor

lcss::Descriptor::Descriptor(BYTE tag)
	:_tag(tag)
{

}

lcss::Descriptor::~Descriptor()
{

}

lcss::Descriptor::Descriptor(const lcss::Descriptor& orig)
	: _tag(orig._tag)
{
	std::copy(orig._value.begin(), orig._value.end(), std::back_inserter(_value));
}

lcss::Descriptor& lcss::Descriptor::operator=(const lcss::Descriptor& rhs)
{
	Descriptor temp(rhs);
	swap(temp);

	return *this;
}

void lcss::Descriptor::swap(lcss::Descriptor& src)
{
	std::swap(_tag, src._tag);
	_value.swap(src._value);
}

void lcss::Descriptor::setValue(const BYTE* newval, UINT16 len)
{
	_value.clear();
	std::copy(newval, newval + len, std::back_inserter(_value));
}

void lcss::Descriptor::value(BYTE* value) const
{
#ifdef WIN32
	std::copy(_value.begin(), _value.end(), stdext::checked_array_iterator<BYTE*>(value, _value.size()));
#else
	std::copy(_value.begin(), _value.end(), value);
#endif
}

BYTE lcss::Descriptor::tag() const
{
	return _tag;
}

BYTE lcss::Descriptor::length() const
{
	return (BYTE)_value.size();
}

/////////////////////////////////////////////////////////////////////////////
// ProgramElement
lcss::ProgramElement::ProgramElement(BYTE type, UINT16 pid)
	:stream_type_(type)
	, elementary_PID_(pid)
{

}

lcss::ProgramElement::~ProgramElement()
{

}

lcss::ProgramElement::ProgramElement(const lcss::ProgramElement& orig)
{
	stream_type_ = orig.stream_type_;
	elementary_PID_ = orig.elementary_PID_;

	std::copy(orig.descriptors_.begin(), orig.descriptors_.end(), std::back_inserter(descriptors_));
}

lcss::ProgramElement& lcss::ProgramElement::operator=(const lcss::ProgramElement& rhs)
{
	ProgramElement temp(rhs);
	swap(temp);

	return *this;
}

void lcss::ProgramElement::swap(lcss::ProgramElement& src)
{
	std::swap(stream_type_, src.stream_type_);
	std::swap(elementary_PID_, src.elementary_PID_);
	descriptors_.swap(src.descriptors_);
}

void lcss::ProgramElement::addDescriptor(const lcss::Descriptor& desc)
{
	bool insert = true;
	lcss::ProgramElement::iterator it;
	for (it = begin(); it != end(); ++it)
	{
		if (it->tag() == desc.tag())
		{
			insert = false;
			break;
		}
	}

	if (insert)
		descriptors_.push_back(desc);
}

lcss::ProgramElement::iterator lcss::ProgramElement::begin()
{
	return descriptors_.begin();
}

lcss::ProgramElement::const_iterator lcss::ProgramElement::begin() const
{
	return descriptors_.begin();
}

lcss::ProgramElement::iterator lcss::ProgramElement::end()
{
	return descriptors_.end();
}

lcss::ProgramElement::const_iterator lcss::ProgramElement::end() const
{
	return descriptors_.end();
}

size_t lcss::ProgramElement::size() const
{
	return descriptors_.size();
}

BYTE lcss::ProgramElement::stream_type() const
{
	return stream_type_;
}

UINT16 lcss::ProgramElement::pid() const
{
	return elementary_PID_;
}

UINT16 lcss::ProgramElement::ES_info_length() const
{
	UINT16 len = 0;
	for (auto& d : descriptors_)
	{
		len += 2;
		len += d.length();
	}
	return len;
}

UINT16 lcss::ProgramElement::raw_ES_info_length() const
{
	UINT16 len = ES_info_length();
	return len | 0xF000;
}

template<typename BackInsertIter>
void lcss::ProgramMapTable::serialize(BackInsertIter backit) const
{
	std::copy(_pimpl->_pmt.begin(), _pimpl->_pmt.end(), backit);
	if (program_info_length() > 0)
	{
		BYTE val[BUFSIZ];
		for (const lcss::Descriptor& descr : _pimpl->_program_info)
		{
			*++backit = descr.tag();
			*++backit = descr.length();
			descr.value(val);
			for (UINT32 i = 0; i < descr.length(); i++)
				*++backit = val[i];
		}
	}

	for (const lcss::ProgramElement& pe : _pimpl->_program_elmts)
	{
		BYTE bValue[2]{};
		*++backit = pe.stream_type();
		UINT16 value = htons(pe.pid());
		memcpy(bValue, &value, 2);
		BYTE b = bValue[0] | 0xE0;
		*++backit = b;
		*++backit = bValue[1];
		value = htons(pe.raw_ES_info_length());
		memcpy(bValue, &value, 2);
		*++backit = bValue[0];
		*++backit = bValue[1];
		if (pe.ES_info_length() > 0)
		{
			for (const lcss::Descriptor& d : pe)
			{
				BYTE val[BUFSIZ]{};
				*++backit = d.tag();
				*++backit = d.length();
				d.value(val);
				for (UINT32 i = 0; i < d.length(); i++)
				{
					*++backit = val[i];
				}
			}
		}
	}

	*++backit = (_pimpl->CRC_32_ >> 24) & 0xff;
	*++backit = (_pimpl->CRC_32_ >> 16) & 0xff;
	*++backit = (_pimpl->CRC_32_ >> 8) & 0xff;
	*++backit = (_pimpl->CRC_32_) & 0xff;
}

template void lcss::ProgramMapTable::serialize<std::back_insert_iterator<std::vector<BYTE>>>(std::back_insert_iterator<std::vector<BYTE>>) const;

template<typename BackInsertIter>
void lcss::ProgramMapTable::program_infos(BackInsertIter backit) const
{
	std::copy(_pimpl->_program_info.begin(),
		_pimpl->_program_info.end(),
		backit);
}

template void lcss::ProgramMapTable::program_infos<std::back_insert_iterator<std::vector<lcss::Descriptor>>>(std::back_insert_iterator<std::vector<lcss::Descriptor>>) const;