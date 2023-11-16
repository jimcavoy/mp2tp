#include "tspacker.h"

namespace lcss
{
	class TSPacker::Impl
	{
	public:
		Impl()
		{

		};
	};

	TSPacker::TSPacker()
		:_pimpl(std::make_unique<lcss::TSPacker::Impl>())
	{

	}
}