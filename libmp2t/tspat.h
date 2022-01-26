#pragma once

#include "tstype.h"
#include <map>
#include <memory>

namespace lcss
{

class ProgramAssociationTable
{
public:
	typedef std::map<UINT32,UINT32> MapType;
	typedef MapType::iterator iterator;
	typedef MapType::const_iterator const_iterator;
public:
	ProgramAssociationTable();
	~ProgramAssociationTable();

	void parse( const BYTE* table );

	MapType::iterator begin();
	MapType::iterator end();

	MapType::const_iterator begin() const;
	MapType::const_iterator end() const;

	MapType::iterator find(UINT32 pid);

	size_t size() const;

public:
	BYTE	pointer_field()				const;
	BYTE	table_id()					const;
	UINT16	section_length()			const;
	UINT16	transport_stream_id()		const;
	BYTE	version_number()			const;
	bool	current_next_indicator()	const;
	BYTE	section_number()			const;
	BYTE	last_section_number()		const;
	UINT32	CRC_32()					const;

private:
	class Impl;
	std::unique_ptr<Impl> _pimpl;
};

}