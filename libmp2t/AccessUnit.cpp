#include "AccessUnit.h"

#include <iterator>

namespace lcss
{
	class AccessUnit::Impl
	{
	public:
		Impl() = default;
		~Impl() = default;
		Impl(const Impl& src)
			: _pts(src._pts)
			, _dts(src._dts)
			, _stream_id(src._stream_id)
		{
			std::copy(src._sodb.begin(), src._sodb.end(), std::back_inserter(_sodb));
		}

	public:
		AccessUnit::sodb_type _sodb;
		uint64_t _pts{};  // units of 90 kHz clock
		uint64_t _dts{};  // units of 90 kHz clock
		uint8_t _stream_id{};
	};

	/////////////////////////////////////////////////////////////////////////////
	// AccessUnit
	AccessUnit::AccessUnit()
		:_pimpl(new lcss::AccessUnit::Impl{})
	{
	}

	AccessUnit::AccessUnit(uint8_t* sodb, size_t len)
		:_pimpl(new lcss::AccessUnit::Impl{})
	{
		std::copy(sodb, sodb + len, std::back_inserter(_pimpl->_sodb));
	}

	AccessUnit::AccessUnit(uint8_t* sodb, size_t len, uint8_t streamId, uint64_t pts, uint64_t dts)
		:_pimpl(new lcss::AccessUnit::Impl{})
	{
		std::copy(sodb, sodb + len, std::back_inserter(_pimpl->_sodb));
		_pimpl->_pts = pts;
		_pimpl->_dts = dts;
		_pimpl->_stream_id = streamId;
	}

	AccessUnit::~AccessUnit() = default;

	AccessUnit::AccessUnit(const lcss::AccessUnit& src)
		:_pimpl(std::make_unique<lcss::AccessUnit::Impl>(*src._pimpl))
	{

	}

	AccessUnit& AccessUnit::operator=(const AccessUnit& rhs)
	{
		if (this != &rhs)
		{
			_pimpl.reset(new AccessUnit::Impl(*rhs._pimpl));
		}
		return *this;
	}

	AccessUnit::AccessUnit(AccessUnit&& src) noexcept
	{
		*this = std::move(src);
	}

	AccessUnit& AccessUnit::operator=(AccessUnit&& rhs) noexcept
	{
		if (this != &rhs)
		{
			_pimpl = std::move(rhs._pimpl);
		}
		return *this;
	}

	void AccessUnit::insert(const uint8_t* sodb, unsigned int len)
	{
		_pimpl->_sodb.insert(_pimpl->_sodb.end(), &sodb[0], &sodb[len]);
	}

	void AccessUnit::clear()
	{
		_pimpl->_sodb.clear();
		_pimpl->_pts = 0;
		_pimpl->_dts = 0;
		_pimpl->_stream_id = 0;
	}

	size_t AccessUnit::length() const
	{
		return _pimpl->_sodb.size();
	}

	AccessUnit::iterator AccessUnit::begin()
	{
		return _pimpl->_sodb.begin();
	}

	AccessUnit::iterator AccessUnit::end()
	{
		return _pimpl->_sodb.end();
	}

	AccessUnit::const_iterator AccessUnit::begin() const
	{
		return _pimpl->_sodb.begin();
	}

	AccessUnit::const_iterator AccessUnit::end() const
	{
		return _pimpl->_sodb.end();
	}

	const uint8_t* AccessUnit::data() const
	{
		return _pimpl->_sodb.data();
	}

	uint8_t AccessUnit::operator[](size_t index) const
	{
		if (index > _pimpl->_sodb.size())
			return 0;
		return _pimpl->_sodb[index];
	}

	void AccessUnit::setPTS(uint64_t pts)
	{
		_pimpl->_pts = pts;
	}

	uint64_t AccessUnit::PTS() const
	{
		return _pimpl->_pts;
	}
	void AccessUnit::setDTS(uint64_t dts)
	{
		_pimpl->_dts = dts;
	}
	uint64_t AccessUnit::DTS() const
	{
		return _pimpl->_dts;
	}
	void AccessUnit::setStreamId(uint8_t id)
	{
		_pimpl->_stream_id = id;
	}
	uint8_t AccessUnit::streamId() const
	{
		return _pimpl->_stream_id;
	}
}