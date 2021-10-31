#pragma once

#ifndef SAT_MAP_UTILS_H
#define SAT_MAP_UTILS_H

#include <CoordTopocentric.h>
#include <CoordGeodetic.h>
#include <Observer.h>
#include <SGP4.h>
#include <vector>

const double PI = 3.14159265359;
const double DEG_2_RAD = PI / 180;
const double EARTH_RADIUS = 6378.135;

namespace Utils {

	DateTime FindVisibleTime(DateTime curTime, Tle tle, SGP4 sat, Observer obs, float visCone);

	double GetAltitude(SGP4 sat, DateTime time = DateTime::Now());
	double GetVelocity(SGP4 sat, DateTime time = DateTime::Now());
		



	int GetIndexFromId(std::vector<Tle>& allTles, int id);
		
}

#endif // !SAT_MAP_UTILS_H
