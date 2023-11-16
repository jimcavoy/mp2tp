#ifndef TSPACKER_H
#define TSPACKER_H

#include "tstype.h"

#include <memory>

namespace lcss
{
	/// <summary>
	/// 
	/// </summary>
	class TSPacker
	{
	public:
		TSPacker();
		
	private:
		class Impl;
		std::unique_ptr<Impl> _pimpl;
	};
}


#endif
