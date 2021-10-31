#pragma once


#ifndef SGP4_H
#define SGP4_H
#include <cmath>
#include "SatelliteRecord.h"

const double PI = 3.14159265359;
const double DEG_2_RAD = PI / 180;
const double DOT_PRODUCT = 1440 / (2 * PI);
const double EARTH_RADIUS = 6378.135;

const double SS = 78 / EARTH_RADIUS + 1;
const double QZMS2T = ((120 - 78) / EARTH_RADIUS) * ((120 - 78) / EARTH_RADIUS) * ((120 - 78) / EARTH_RADIUS) * ((120 - 78) / EARTH_RADIUS);

//WGS72 Constants

const double mu = 398600.8;
const double xke = 60.0 / sqrt(EARTH_RADIUS * EARTH_RADIUS * EARTH_RADIUS / mu);
const double tumin = 1.0 / xke;
const double j2 = 0.001082616;
const double j3 = -0.00000253881;
const double j4 = -0.00000165597;
const double j3oj2 = j3 / j2;


void ConvertToSGP4Units(SatelliteRecord sRecord);
#endif // !SGP4_H
