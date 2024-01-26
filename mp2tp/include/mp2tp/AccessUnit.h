#pragma once
#include <vector>
#include <memory>

namespace lcss
{
	/////////////////////////////////////////////////////////////////////////////
	// AccessUnit
	class AccessUnit
	{
	public:
		enum STREAMID
		{
			Video = 0xE0,
			MetadataAsync = 0xBD,
			MetadataSync = 0xFC
		};

	public:
		typedef std::vector<uint8_t> sodb_type; // string of data bits collection type
		typedef sodb_type::iterator iterator;
		typedef sodb_type::const_iterator const_iterator;
	public:
		AccessUnit();
		AccessUnit(const uint8_t* sodb, size_t len);
		AccessUnit(const uint8_t* sodb, size_t len, uint8_t streamId, uint64_t pts, uint64_t dts);
		~AccessUnit();

		AccessUnit(const AccessUnit& cp);
		AccessUnit& operator=(const AccessUnit& rhs);

		AccessUnit(AccessUnit&& cp) noexcept;
		AccessUnit& operator=(AccessUnit&& rhs) noexcept;


		void insert(const uint8_t* sodb, unsigned int len);

		void clear();

		size_t length() const;

		iterator begin();
		iterator end();

		const_iterator begin() const;
		const_iterator end() const;

		const uint8_t* data() const;
		uint8_t operator[](size_t index) const;

		void setPTS(uint64_t pts);
		uint64_t PTS() const;

		void setDTS(uint64_t dts);
		uint64_t DTS() const;

		void setStreamId(uint8_t id);
		uint8_t streamId() const;

	private:
		class Impl;
		std::unique_ptr<lcss::AccessUnit::Impl> _pimpl;
	};

}