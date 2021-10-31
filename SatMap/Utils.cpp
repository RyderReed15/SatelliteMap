#include "Utils.h"

DateTime Utils::FindVisibleTime(DateTime curTime, Tle tle, SGP4 sat, Observer obs, float visCone) {
	int IntervalsPerWeek = 6 * 60 * 24 * 7;
	DateTime time = tle.Epoch().Add(curTime - tle.Epoch());
	CoordTopocentric top;
	int i = 0;
	do {
		time = time.AddSeconds(10);
		Eci eci = sat.FindPosition(time);
		top = obs.GetLookAngle(eci);
		i++;
		//Add time of day check
	} while (i < IntervalsPerWeek && abs(90 - (top.elevation * 180 / PI)) > visCone);
	if (i >= IntervalsPerWeek) {
		return 0;
	}
	return time;


}

double Utils::GetAltitude(SGP4 sat, DateTime time) {
	Eci eci = sat.FindPosition(time);
	return eci.ToGeodetic().altitude;
}
double Utils::GetVelocity(SGP4 sat, DateTime time) {
	Eci eci = sat.FindPosition(time);
	return eci.Velocity().Magnitude();
}



int Utils::GetIndexFromId(std::vector<Tle>& allTles, int id) {
	for (int i = 0; i < allTles.size(); i++)
	{
		if (allTles[i].NoradNumber() == id) return i;
	}
	return -1;
}
