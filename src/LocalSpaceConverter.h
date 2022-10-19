#ifndef RREWIND_LOCALSPACECONVERTER_H
#define RREWIND_LOCALSPACECONVERTER_H

#include "Forward.h"

#include <proj.h>

namespace rrewind
{
	/**
	 * This class provides coordinate conversions to geodetic lat/lon from a "local space" cartesian 
	 * coordinate system that is anchored from a given lat/lon.
	 */
	class LocalSpaceConverter
	{
	public:
		/**
		 * Construct a coordinate converter.
		 *
		 * @todo The constructor should accept the lat/lon anchor point.
		 */
		LocalSpaceConverter();

		/**
		 * Convert from local space cartesian coordinates to geodetic lat/lon. 
		 * 
		 * @warn Conversion is accurate only for locations close to the anchor point.
		 * 
		 * @todo Consider using one of the many available containers for lat/lon coordinates instead of a pair.
		 * 
		 * @param localX The cartesian X offset from the anchor point to convert
		 * @param localY The cartesian Y offset from the anchor point to convert
		 * @return The geodetic (decimal degrees) location of the provided cartesian coordinates, in lat/lon order
		 */
		std::pair<double, double> convert(int localX, int localY);

	private:
		// TODO: This should be wrapped in a unique_ptr, but it did not seem to work initially
		PJ *mProjSrs;
	};
}

#endif