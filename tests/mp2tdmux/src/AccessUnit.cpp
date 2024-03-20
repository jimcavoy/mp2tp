#include "AccessUnit.h"

#include <iterator>

/////////////////////////////////////////////////////////////////////////////
// AccessUnit
AccessUnit::AccessUnit()
	:isKey_(false)
	, pts_(0)
	, dts_(0)
{

}

AccessUnit::AccessUnit(char* sodb, unsigned int len)
	:isKey_(false)
	, pts_(0)
	, dts_(0)
{
	std::copy(sodb, sodb + len, std::back_inserter(sodb_));
}

AccessUnit::AccessUnit(const AccessUnit& cp)
{
	sodb_.clear();
	std::copy(cp.begin(), cp.end(), std::back_inserter(sodb_));
	isKey_ = cp.isKey_;
	pts_ = cp.pts_;
	dts_ = cp.dts_;
}

AccessUnit& AccessUnit::operator=(const AccessUnit& rhs)
{
	AccessUnit temp(rhs);
	swap(temp);

	return *this;
}

AccessUnit::~AccessUnit()
{

}

void AccessUnit::insert(char* sodb, unsigned int len)
{
	std::copy(sodb, sodb + len, std::back_inserter(sodb_));
}

void AccessUnit::swap(AccessUnit& src)
{
	std::swap(isKey_, src.isKey_);
	std::swap(pts_, src.pts_);
	std::swap(dts_, src.dts_);
	sodb_.swap(src.sodb_);
}

void AccessUnit::clear()
{
	sodb_.clear();
	isKey_ = false;
	pts_ = 0;
	dts_ = 0;
}
