#ifndef RREWIND_LOCALSPACECONVERTER_H
#define RREWIND_LOCALSPACECONVERTER_H

#include "Forward.h"

#include <proj.h>

namespace rrewind
{
	class LocalSpaceConverter
	{
	public:
		LocalSpaceConverter(/*double anchorLat, double anchorLon*/);

		std::pair<double, double> convert(int localX, int localY);

	private:
		//std::unique_ptr<PJ> mProjSrs;
		PJ *mProjSrs;
	};
}

#endif