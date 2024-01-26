#pragma once

#include "tstype.h"
#include <vector>
#include <cstddef>

namespace lcss
{
/////////////////////////////////////////////////////////////////////////////
// MetadataAUCell
// The class represents a Metadata Access Unit cell (metadata_AU_cell)
// Ref: ISO/IEC 13818-1 : 2013 (E), Table 2-108 - Metadata AU cell, pg 129
class MetadataAUCell
{
private:
	typedef std::vector<BYTE> sodb_type; // string of data bits collection type

public:
	MetadataAUCell();
	~MetadataAUCell();

	MetadataAUCell(const MetadataAUCell& cp);
	MetadataAUCell& operator=(const MetadataAUCell& rhs);

	MetadataAUCell(MetadataAUCell&& cp) noexcept;
	MetadataAUCell& operator=(MetadataAUCell&& rhs) noexcept;

	void swap(MetadataAUCell& src);

	size_t parse(BYTE* sodb, size_t len);

	void clear();

// metadata AU cell header properties

	short		metadata_service_id()			const;
	short		sequence_number()				const;
	short		cell_fragmentation_indication() const;
	bool		decoder_config_flag()			const;
	bool		random_access_indicator()		const;
	void		header(BYTE* header)			const;

	size_t		AU_cell_data_length()			const { return sodb_.size()-5; }
	const BYTE* AU_cell_data_bytes()			const { return sodb_.data()+5; }
	

private:
	sodb_type	sodb_;
};

/////////////////////////////////////////////////////////////////////////////
// MetadataAUWrapper
// The class represents a Metadata Access Unit Wrapper (metadata_AU_wrapper)
// Ref: ISO/IEC 13818-1 : 2013 (E), Table 2-107 - Metadata Access Unit Wrapper, pg 129
class MetadataAUWrapper
{
public:
	typedef std::vector<MetadataAUCell> metadata_au_cells_type;
	typedef metadata_au_cells_type::iterator iterator;
	typedef metadata_au_cells_type::const_iterator const_iterator;
public:
	MetadataAUWrapper();
	~MetadataAUWrapper();

	MetadataAUWrapper(const MetadataAUWrapper& other);
	MetadataAUWrapper& operator=(const MetadataAUWrapper& rhs);

	MetadataAUWrapper(MetadataAUWrapper&& other) noexcept;
	MetadataAUWrapper& operator=(MetadataAUWrapper&& rhs) noexcept;

	// Return the number of metadata_AU_cells that instance of 
	// this class contains, otherwise 0
	size_t parse(BYTE* sodb, size_t len);

	iterator begin() { return metadata_au_cells_.begin(); }
	iterator end() { return metadata_au_cells_.end(); }

	const_iterator begin() const { return metadata_au_cells_.begin(); }
	const_iterator end() const { return metadata_au_cells_.end(); }

	size_t length() const { return metadata_au_cells_.size(); }

private:
	metadata_au_cells_type metadata_au_cells_;
};

} // namespace lcss
