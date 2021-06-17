#pragma once

#include "tstype.h"
#include <map>

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
	BYTE pointer_field_;
	BYTE table_id_;
	UINT16 section_length_;
	UINT16 transport_stream_id_;
	BYTE reserved_:					2;
	BYTE version_number_:			5;
	BYTE current_next_indicator_:	1;
	BYTE section_number_;
	BYTE last_section_number_;
	UINT32 CRC_32_;
private:
	MapType pid_to_program_;
};

}