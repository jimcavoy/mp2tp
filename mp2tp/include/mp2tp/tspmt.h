#pragma once

#include <map>
#include <vector>
#include <memory>
#include <cstdint>

#ifndef WIN32
#include <memory.h>
#include <arpa/inet.h>
#endif


namespace lcss
{

	/////////////////////////////////////////////////////////////////////////////
	// class Descriptor
	
	/// @brief Descriptor represents additional information associated with a 
	/// ProgramElement
	class Descriptor
	{
	public:
		Descriptor(uint8_t tag = 0);
		~Descriptor();

		Descriptor(const Descriptor& orig);
		Descriptor& operator=(const Descriptor& rhs);
		void swap(Descriptor& src);

		void setValue(const uint8_t* newval, uint16_t len);
		void value(uint8_t* value) const;

		uint8_t tag() const;
		uint8_t length() const;

	private:
		uint8_t _tag;
		std::vector<uint8_t> _value;
	};

	/////////////////////////////////////////////////////////////////////////////
	// class ProgramElement

	/// @brief ProgramElement describes the elementary stream in a ProgramMapTable.
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
		ProgramElement(uint8_t type, uint16_t pid);
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

		uint8_t stream_type() const;
		uint16_t pid() const;
		uint16_t ES_info_length() const;
		uint16_t raw_ES_info_length() const;

	private:
		uint8_t	stream_type_;
		uint16_t  elementary_PID_;

		CollectionType  descriptors_;
	};

	/////////////////////////////////////////////////////////////////////////////
	// ProgramMapTable
	/// <summary>
	/// ProgramMapTable implements the TS_program_map_section as defined in
	/// Ref: ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Reserved, Table 2-33 page 50 
	/// </summary>
	class ProgramMapTable
	{
	public:
		typedef std::vector<ProgramElement> MapType;
		typedef MapType::iterator iterator;
		typedef MapType::const_iterator const_iterator;
		typedef std::vector<Descriptor> DescriptorArray;

		static uint8_t default_seq[17];
	public:
		ProgramMapTable();
		ProgramMapTable(const uint8_t* buffer, int len);
		~ProgramMapTable();

		ProgramMapTable(const ProgramMapTable& orig);
		ProgramMapTable& operator=(const ProgramMapTable& rhs);

		ProgramMapTable(ProgramMapTable&&) noexcept;
		ProgramMapTable& operator=(ProgramMapTable&&) noexcept;

		// Methods
		void add(const uint8_t* buffer, int len);
		bool canParse() const;
		bool parse();

		bool hasPCR(uint16_t pid) const;

		void addProgramElement(const ProgramElement& pe);
		void removeProgramElement(const ProgramElement& pe);

		// Fields
		uint8_t		pointer_field()				const;
		uint8_t		table_id()					const;
		bool		section_syntax_indicator()	const;
		uint16_t	section_length()			const;
		uint16_t	program_number()			const;
		uint8_t		version_number()			const;
		bool		current_next_indicator()	const;
		uint8_t		section_number()			const;
		uint8_t		last_section_number()		const;
		uint16_t	PCR_PID()					const;
		uint16_t	program_info_length()		const;
		uint32_t	CRC_32()					const;

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

	/// @brief eqDescriptor is a predicate functor to determine if a 
	/// Descriptor is equal to a tag that is passed in the 
	/// constructor
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



