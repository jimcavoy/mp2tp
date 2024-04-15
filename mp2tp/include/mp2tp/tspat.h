#pragma once

#include "tstype.h"
#include <map>
#include <memory>

namespace lcss
{

/// <summary>
/// ProgramAssociationTable implements the definition of a program_association_section
/// as outlined in ISO/IEC 13818-1 : 2013(E) Table 2-30 page 47.
/// </summary>
class ProgramAssociationTable
{
public:
	typedef std::map<uint16_t,uint16_t> MapType;
	typedef MapType::iterator iterator;
	typedef MapType::const_iterator const_iterator;
public:
	ProgramAssociationTable();
	~ProgramAssociationTable();

	ProgramAssociationTable(const ProgramAssociationTable& other);
	ProgramAssociationTable& operator=(const ProgramAssociationTable& rhs);

	ProgramAssociationTable(ProgramAssociationTable&&) noexcept;
	ProgramAssociationTable& operator=(ProgramAssociationTable&&) noexcept;

	/// <summary>
	/// Parse a raw byte sequence of a program_association_section into this class instance.
	/// Ref: ISO/IEC 13818-1 : 2013(E) Table 2-30 page 47.
	/// </summary>
	/// <param name="table">Raw byte sequence of a program_association_section.</param>
	void parse( const BYTE* table );

	/// <summary>
	/// Add a program with the associated PID for the Program Map Table
	/// </summary>
	/// <param name="program">The program number.</param>
	/// <param name="pid">The PID for the Program Map Table for that program.</param>
	/// <returns>If successful returns an iterator pointing to the new entry; otherwise,
	/// return an iterator pointing to end().</returns>
	MapType::iterator addProgram(uint16_t program, uint16_t pid);

	MapType::iterator begin();
	MapType::iterator end();

	MapType::const_iterator begin() const;
	MapType::const_iterator end() const;

	/// <summary>
	/// Find the program associated with a PID.
	/// </summary>
	/// <param name="pid">The PID for the Program Map Table.</param>
	/// <returns>Returns an iterator to a pair containing the PID
	/// to the Program Map Table for the program and the program number.</returns>
	MapType::iterator find(uint16_t pid);

	/// <summary>
	/// Query the number of programs in the stream.
	/// </summary>
	/// <returns>The number of programs.</returns>
	size_t size() const;

	template<typename BackInsertIter>
	void serialize(BackInsertIter backit) const;

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
	class Impl; ///< ProgramAssociationTable implementation class
	std::unique_ptr<Impl> _pimpl; ///< smart pointer to the implementation class instance
};



}