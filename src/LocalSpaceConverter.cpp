#include "LocalSpaceConverter.h"

namespace rrewind
{
	LocalSpaceConverter::LocalSpaceConverter()
	{
		// A topographic coordinate system seemed to be the best fit for this kind of transformation.
		// See https://proj.org/operations/conversions/topocentric.html

		// TODO: Remove hard code of anchor location
		mProjSrs = proj_create(0, "+proj=pipeline +step +proj=cart +ellps=WGS84 +units=m +step +proj=topocentric "
			"+ellps=WGS84 +lat_0=45.6162690837484 +lon_0=9.2825406854031 +h_0=0 +units=dm");
	}

	std::pair<double, double> LocalSpaceConverter::convert(int localX, int localY)
	{
		PJ_XYZ pt;
		pt.x = localX;
		pt.y = localY;

		// Z must be set, otherwise conversion yields unexpected results
		pt.z = 0; 

		PJ_COORD coordPt;
		coordPt.xyz = pt;

		// Perform the transform
		PJ_COORD convPt = proj_trans(mProjSrs, PJ_INV, coordPt);

		// Convert from radians to degrees. Pair order is lon/lat
		return std::make_pair(proj_todeg(convPt.lp.lam), proj_todeg(convPt.lp.phi));
	}
}