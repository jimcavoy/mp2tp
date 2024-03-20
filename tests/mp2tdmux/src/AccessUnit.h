#pragma once
#include <vector>

/////////////////////////////////////////////////////////////////////////////
// AccessUnit
class AccessUnit
{
public:
	typedef std::vector<char> sodb_type; // string of data bits collection type
	typedef sodb_type::iterator iterator;
	typedef sodb_type::const_iterator const_iterator;
public:
	AccessUnit();
	AccessUnit(char* sodb, unsigned int len);
	AccessUnit(const AccessUnit& cp);
	AccessUnit& operator=(const AccessUnit& rhs);
	~AccessUnit();

	void swap(AccessUnit& src);

	void insert(char* sodb, unsigned int len);

	void clear();

	size_t length() const { return sodb_.size(); }

	iterator begin() { return sodb_.begin(); }
	iterator end() { return sodb_.end(); }

	const_iterator begin() const { return sodb_.begin(); }
	const_iterator end() const { return sodb_.end(); }

	char* data() { return sodb_.data(); }

	bool isKey() const { return isKey_; }
	void toogleKey() { isKey_ = isKey_ ? false : true; }
private:
	sodb_type   sodb_;
	bool        isKey_;

public:
	uint64_t pts_;
	uint64_t dts_;
};
