#include "LocalSpaceConverter.h"

namespace rrewind
{
	LocalSpaceConverter::LocalSpaceConverter(/*double anchorLat, double anchorLon*/)
	{
		mProjSrs = proj_create(0, "+proj=pipeline +step +proj=cart +ellps=WGS84 +units=m +step +proj=topocentric +ellps=WGS84 +lat_0=45.6162690837484 +lon_0=9.2825406854031 +h_0=0 +units=dm");
	}

	std::pair<double, double> LocalSpaceConverter::convert(int localX, int localY)
	{
		PJ_XYZ pt;
		pt.x = localX;
		pt.y = localY;
		pt.z = 0;

		PJ_COORD coordPt;
		coordPt.xyz = pt;

		PJ_COORD convPt = proj_trans(mProjSrs, PJ_INV, coordPt);

		// Lon, lat
		return std::make_pair(proj_todeg(convPt.lp.lam), proj_todeg(convPt.lp.phi));
	}
}