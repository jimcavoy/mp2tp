#include "tsmetadata.h"

#include <iterator>

#ifdef WIN32
#include <WinSock2.h>
#else
#include <memory.h>
#include <arpa/inet.h>
#endif

namespace lcss 
{
/////////////////////////////////////////////////////////////////////////////
// MetadataAUCell
MetadataAUCell::MetadataAUCell()
{
}

MetadataAUCell::~MetadataAUCell()
{

}

MetadataAUCell::MetadataAUCell(const MetadataAUCell& cp)
{
	sodb_.clear();
	std::copy(cp.sodb_.begin(), cp.sodb_.end(), std::back_inserter(sodb_));
}

MetadataAUCell& MetadataAUCell::operator=(const MetadataAUCell& rhs)
{
	MetadataAUCell temp(rhs);
	swap(temp);

	return *this;
}

MetadataAUCell::MetadataAUCell(MetadataAUCell&& other) noexcept
{
	*this = std::move(other);
}

MetadataAUCell& MetadataAUCell::operator=(MetadataAUCell&& rhs) noexcept
{
	if (this != &rhs)
	{
		sodb_ = std::move(rhs.sodb_);
	}
	return *this;
}

size_t MetadataAUCell::parse(BYTE* sodb, size_t len)
{
	size_t nRet = 0;
	short au_cell_data_len = 0;
	if(len > 5)
	{
		memcpy(&au_cell_data_len,sodb+3,2);
		au_cell_data_len = ntohs(au_cell_data_len);
		nRet = (size_t)au_cell_data_len + 5;
		if(nRet <= len)
		{
			std::copy(sodb, sodb+nRet, std::back_inserter(sodb_));
		}
		else
		{
			nRet = 0; // parse error
		}
	}
	return nRet;
}

void MetadataAUCell::swap(MetadataAUCell& src)
{
	sodb_.clear();
	std::copy(src.sodb_.begin(), src.sodb_.end(), std::back_inserter(sodb_));
}

void MetadataAUCell::clear()
{
	sodb_.clear();
}

short MetadataAUCell::metadata_service_id() const
{
	return (short) sodb_[0];
}

short MetadataAUCell::sequence_number() const
{
	return (short) sodb_[1];
}

short MetadataAUCell::cell_fragmentation_indication() const
{
	BYTE flag = sodb_[2];
	short cell_frag_ind = flag >> 6;
	return cell_frag_ind;
}

bool MetadataAUCell::decoder_config_flag() const
{
	BYTE flag = sodb_[2];
	bool ret = flag & 0x20 ? true : false;
	return ret;
}

bool MetadataAUCell::random_access_indicator() const
{
	BYTE flag = sodb_[2];
	bool ret = flag & 0x10 ? true : false;
	return ret;
}

void MetadataAUCell::header(BYTE* head) const
{
	for(int i = 0; i < 5; i++)
		head[i] = sodb_[i];
}

/////////////////////////////////////////////////////////////////////////////
// MetadataAUWrapper

MetadataAUWrapper::MetadataAUWrapper()
{

}

MetadataAUWrapper::~MetadataAUWrapper()
{

}

MetadataAUWrapper::MetadataAUWrapper(const MetadataAUWrapper& other)
{
	std::copy(other.begin(), other.end(), std::back_inserter(metadata_au_cells_));
}

MetadataAUWrapper& MetadataAUWrapper::operator=(const MetadataAUWrapper& rhs)
{
	if (this != &rhs)
	{
		std::copy(rhs.begin(), rhs.end(), std::back_inserter(metadata_au_cells_));
	}
	return *this;
}

MetadataAUWrapper::MetadataAUWrapper(MetadataAUWrapper&& other) noexcept
{
	*this = std::move(other);
}

MetadataAUWrapper& MetadataAUWrapper::operator=(MetadataAUWrapper&& rhs) noexcept
{
	if (this != &rhs)
	{
		metadata_au_cells_ = std::move(rhs.metadata_au_cells_);
	}
	return *this;
}

size_t MetadataAUWrapper::parse(BYTE* sodb, size_t len)
{
	size_t pos = 0;

	while(pos < len)
	{
		MetadataAUCell cell;
		pos = cell.parse(sodb+pos,len);
		if(pos > 0)
			metadata_au_cells_.push_back(cell);
		else if( pos == 0 ) // parse error
			return pos;
	}

	return metadata_au_cells_.size();
}

} // namespace lcss