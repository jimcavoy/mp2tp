#pragma once

#include "tstype.h"

#include <map>
#include <vector>
#include <memory>

#ifndef WIN32
#include <memory.h>
#include <arpa/inet.h>
#endif

/// <summary>
/// Classes required to represent entities that compose a Program Map Table
/// Ref: ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Reserved, Table 2-33 page 50 
/// </summary>
namespace lcss
{

	/////////////////////////////////////////////////////////////////////////////
	// class Descriptor
	class Descriptor
	{
	public:
		Descriptor(BYTE tag = 0);
		~Descriptor();

		Descriptor(const Descriptor& orig);
		Descriptor& operator=(const Descriptor& rhs);
		void swap(Descriptor& src);

		void setValue(const BYTE* newval, UINT16 len);
		void value(BYTE* value) const;

		BYTE tag() const;
		BYTE length() const;

	private:
		BYTE _tag;
		std::vector<BYTE> _value;
	};

	/////////////////////////////////////////////////////////////////////////////
	// class ProgramElement
	class ProgramElement
	{
	public:
		typedef std::vector<Descriptor> CollectionType;
		typedef CollectionType::iterator iterator;
		typedef CollectionType::const_iterator const_iterator;
	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="ProgramElement"/> class.
		/// </summary>
		/// <param name="type">The stream type.</param>
		/// <param name="pid">The elementary PID.</param>
		ProgramElement(BYTE type, UINT16 pid);
		~ProgramElement();

		ProgramElement(const ProgramElement& orig);
		ProgramElement& operator=(const ProgramElement& rhs);
		void swap(ProgramElement& src);

		void addDescriptor(const Descriptor& desc);

		// Iterators for iterating over a descriptor collection of type lcss::Descriptor
		iterator begin();
		const_iterator begin() const;

		iterator end();
		const_iterator end() const;

		size_t size() const;

		BYTE stream_type() const;
		UINT16 pid() const;
		UINT16 ES_info_length() const;
		UINT16 raw_ES_info_length() const;

	private:
		BYTE	stream_type_;
		UINT16  elementary_PID_;

		CollectionType  descriptors_;
	};

	/////////////////////////////////////////////////////////////////////////////
	// ProgramMapTable
	// 
	class ProgramMapTable
	{
	public:
		typedef std::vector<ProgramElement> MapType;
		typedef MapType::iterator iterator;
		typedef MapType::const_iterator const_iterator;
		typedef std::vector<Descriptor> DescriptorArray;
	public:
		ProgramMapTable();
		~ProgramMapTable();
		ProgramMapTable(const ProgramMapTable& orig) = delete;
		ProgramMapTable& operator=(const ProgramMapTable& rhs) = delete;

		// Methods
		void add(const BYTE* buffer, int len);
		bool canParse() const;
		bool parse();
		void clear();

		bool hasPCR(UINT16 pid) const;

		void addProgramElement(const ProgramElement& pe);
		void removeProgramElement(const ProgramElement& pe);

		// Fields
		BYTE	pointer_field()				const;
		BYTE	table_id()					const;
		bool	section_syntax_indicator()	const;
		UINT16	section_length()			const;
		UINT16	program_number()			const;
		BYTE	version_number()			const;
		bool	current_next_indicator()	const;
		BYTE	section_number()			const;
		BYTE	last_section_number()		const;
		UINT16	PCR_PID()					const;
		UINT16	program_info_length()		const;
		UINT32	CRC_32()					const;

		template<typename BackInsertIter>
		void program_infos(BackInsertIter backit) const;

		// Iterators to traverse a ProgramElements in the ProgramMapTable
		MapType::iterator begin();
		MapType::iterator end();

		MapType::const_iterator begin() const;
		MapType::const_iterator end() const;

		template<typename BackInsertIter>
		void serialize(BackInsertIter backit) const;

	private:
		class Impl;
		std::unique_ptr<Impl> _pimpl;
	};

	/////////////////////////////////////////////////////////////////////////////
	// eqDescriptor
	// Predicate Functor
	class eqDescriptor
	{
	public:
		eqDescriptor(int tag)
			:tag_(tag) {}

		bool operator()(lcss::Descriptor& d)
		{
			return d.tag() == tag_;
		}

	private:
		int tag_;
	};

}



