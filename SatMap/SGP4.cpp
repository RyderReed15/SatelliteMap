#include "SGP4.h"
#include <sstream>
#include <iostream>
#include <iomanip>

void PropogateSGP4(SGP4Sat sRecord, double dbTime);
double MakeJulianDate(int year, int month, int day, int hour, int min, double seconds) {
    return 367 * year - floor(7 * (year + floor((month + 9) / 12)) * .25) + floor(275 * month / 9) + day + 1721013.5 + (seconds + min * 60.0 + hour * 3600.0) / 86400.0;

}

void InitSGP4(SatelliteRecord sRecord) {
    SGP4Sat newSat;

    double dbEccentricitySqr = sRecord.dbEccentricity * sRecord.dbEccentricity;
    double omeoSq = 1 - dbEccentricitySqr;
    double rteoSq = sqrt(omeoSq);
    double cosio = cos(sRecord.dbInclination);
    double cosioSq = (cosio * cosio);

    double ak = pow(xke / sRecord.dbMeanMotion, 2 / 3.f);
    newSat.d1 = .75 * j2 * (3 * cosioSq - 1) / (rteoSq * omeoSq);
    double del_ = newSat.d1 / (ak * ak);
    double adel = ak * (1.0 - del_ * del_ - del_ *
        (1.0 / 3.0 + 134.0 * del_ * del_ / 81.0));
    del_ = newSat.d1 / (adel * adel);
    double no = sRecord.dbMeanMotion / (1.0 + del_);

    double ao = pow(xke / no, 2 / 3.f);
    double sinio = sin(sRecord.dbInclination);
    double po = ao * omeoSq;
    double con42 = 1.0 - 5.0 * cosioSq;
    newSat.con41 = -con42 - cosioSq - cosioSq;
    double ainv = 1.0 / ao;
    double posq = po * po;
    double rp = ao * (1.0 - sRecord.dbEccentricity);

    newSat.no = no;

    double dbAltitude = pow(no * tumin, -2 / 3.f);
    double dbAltApogee = dbAltitude * (1 + sRecord.dbEccentricity) - 1;
    double dbAltPerigee = dbAltitude * (1 - sRecord.dbEccentricity) - 1;

    if (omeoSq >= 0 || no >= 0) {
        if (rp < 220 / EARTH_RADIUS + 1) {

        }
        double sfour = SS;
        double qzms24 = QZMS2T;
        double dbPerigee = (rp - 1) * EARTH_RADIUS;

        if (dbPerigee < 156) {
            sfour = dbPerigee - 78;
            if (dbPerigee < 98) {
                sfour = 20;
            }
            double qzms24temp = (120.0 - sfour) / EARTH_RADIUS;
            qzms24 = qzms24temp * qzms24temp * qzms24temp * qzms24temp;
            sfour = sfour / EARTH_RADIUS + 1.0;
        }

        double pInvSqr = 1 / posq;

        double tsi = 1 / (ao - sfour);
        /// <summary>
        /// /
        /// </summary>
        /// <param name="sRecord"></param>
        newSat.dbEta = ao * sRecord.dbEccentricity * tsi;
        double etasq = newSat.dbEta * newSat.dbEta;
        double eeta = sRecord.dbEccentricity * newSat.dbEta;
        double psisq = fabs(1.0 - etasq);
        double coef = qzms24 * pow(tsi, 4.0);
        double coef1 = coef / pow(psisq, 3.5);
        double cc2 = coef1 * no * (ao * (1.0 + 1.5 * etasq + eeta *
            (4.0 + etasq)) + 0.375 * j2 * tsi / psisq * newSat.con41 *
            (8.0 + 3.0 * etasq * (8.0 + etasq)));
        newSat.cc1 = sRecord.dbBSTAR * cc2;
        newSat.cc3 = 0.0;

        if (sRecord.dbEccentricity > .0001) {
            newSat.cc3 = -2 * coef * tsi * j3oj2 * no * sinio / sRecord.dbEccentricity;
        }

        newSat.x1mth2 = 1.0 - cosioSq;
        newSat.cc4 = 2.0 * no * coef1 * ao * omeoSq * \
            (newSat.dbEta * (2.0 + 0.5 * etasq) + sRecord.dbEccentricity *
                (0.5 + 2.0 * etasq) - j2 * tsi / (ao * psisq) *
                (-3.0 * newSat.con41 * (1.0 - 2.0 * eeta + etasq *
                    (1.5 - 0.5 * eeta)) + 0.75 * newSat.x1mth2 *
                    (2.0 * etasq - eeta * (1.0 + etasq)) * cos(2.0 * sRecord.dbArgOfPerigee)));
        newSat.cc5 = 2.0 * coef1 * ao * omeoSq * (1.0 + 2.75 *
            (etasq + eeta) + eeta * etasq);
        double cosio4 = cosioSq * cosioSq;
        double temp1 = 1.5 * j2 * pInvSqr * no;
        double temp2 = 0.5 * temp1 * j2 * pInvSqr;
        double temp3 = -0.46875 * j4 * pInvSqr * pInvSqr * no;
        newSat.mdot = no + 0.5 * temp1 * pInvSqr * newSat.con41 + 0.0625 * \
            temp2 * rteoSq * (13.0 - 78.0 * cosioSq + 137.0 * cosio4);
        newSat.argpdot = (-0.5 * temp1 * con42 + 0.0625 * temp2 *
            (7.0 - 114.0 * cosioSq + 395.0 * cosio4) +
            temp3 * (3.0 - 36.0 * cosioSq + 49.0 * cosio4));
        double xhdot1 = -temp1 * cosio;
        newSat.nodedot = xhdot1 + (0.5 * temp2 * (4.0 - 19.0 * cosioSq) +
            2.0 * temp3 * (3.0 - 7.0 * cosioSq)) * cosio;
        double xpidot = newSat.argpdot + newSat.nodedot;
        newSat.omgcof = sRecord.dbBSTAR * newSat.cc3 * cos(sRecord.dbArgOfPerigee);
        newSat.xmcof = 0.0;
        if (sRecord.dbEccentricity > .0001) {
            newSat.xmcof = -2 / 3.f * coef * sRecord.dbBSTAR / eeta;
        }
        newSat.nodecf = 3.5 * omeoSq * xhdot1 * newSat.cc1;
        newSat.t2cof = 1.5 * newSat.cc1;
        newSat.xlcof = 0;
        if (fabs(cosio + 1.0) > 1.5e-12) {
            newSat.xlcof = -0.25 * j3oj2 * sinio * (3.0 + 5.0 * cosio) / (1.0 + cosio);
            }
        
        else {

            newSat.xlcof = -0.25 * j3oj2 * sinio * (3.0 + 5.0 * cosio) / 1.5e-12;
        }
        newSat.aycof = -0.5 * j3oj2 * sinio;
        
        newSat.delmotemp = 1.0 + newSat.dbEta * cos(sRecord.dbMeanAnomaly);
        newSat.delmo = newSat.delmotemp * newSat.delmotemp * newSat.delmotemp;
        newSat.sinmao = sin(sRecord.dbMeanAnomaly);
        newSat.x7thm1 = 7.0 * cosioSq - 1.0;
        if (2 * PI / no >= 225.0) {
            std::cout << "DEEP" << std::endl;
        }


        double cc1sq = newSat.cc1 * newSat.cc1;
        newSat.d2 = 4.0 * ao * tsi * cc1sq;
        double temp = newSat.d2 * tsi * newSat.cc1 / 3.0;
        newSat.d3 = (17.0 * ao + sfour) * temp;
        newSat.d4 = 0.5 * temp * ao * tsi * (221.0 * ao + 31.0 * sfour) * \
            newSat.cc1;
        newSat.t3cof = newSat.d2 + 2.0 * cc1sq;
        newSat.t4cof = 0.25 * (3.0 * newSat.d3 + newSat.cc1 *
            (12.0 * newSat.d2 + 10.0 * cc1sq));
        newSat.t5cof = 0.2 * (3.0 * newSat.d4 +
            12.0 * newSat.cc1 * newSat.d3 +
            6.0 * newSat.d2 * newSat.d2 +
            15.0 * cc1sq * (2.0 * newSat.d2 + cc1sq));

    }

    newSat.dbArgOfPerigee = sRecord.dbArgOfPerigee;
    newSat.dbEccentricity = sRecord.dbEccentricity;
    newSat.dbInclination = sRecord.dbInclination;
    newSat.dbMeanAnomaly = sRecord.dbMeanAnomaly;
    newSat.dbMeanMotion = sRecord.dbMeanMotion;
    newSat.dbRightAscNode = sRecord.dbRightAscNode;

    newSat.dbBSTAR = sRecord.dbBSTAR;
    PropogateSGP4(newSat, 0.0);

}

void PropogateSGP4(SGP4Sat sRecord, double dbTime) {
    double xmdf = sRecord.dbMeanAnomaly + sRecord.mdot * dbTime;
    double argpdf = sRecord.dbArgOfPerigee + sRecord.argpdot * dbTime;
    double nodedf = sRecord.dbRightAscNode + sRecord.nodedot * dbTime;
    double argpm = argpdf;
    double mm = xmdf;
    double t2 = dbTime * dbTime;
    double nodem = nodedf + sRecord.nodecf * t2;
    double tempa = 1.0 - sRecord.cc1 * dbTime;
    double tempe = sRecord.dbBSTAR * sRecord.cc4 * dbTime;
    double templ = sRecord.t2cof * t2;


    double delomg = sRecord.omgcof * dbTime;

    double delmtemp = 1.0 + sRecord.dbEta * cos(xmdf);
    double delm = sRecord.xmcof * \
        (delmtemp * delmtemp * delmtemp -
            sRecord.delmo);
    double temp = delomg + delm;
    mm = xmdf + temp;
    argpm = argpdf - temp;
    double t3 = t2 * dbTime;
    double t4 = t3 * dbTime;
    tempa = tempa - sRecord.d2 * t2 - sRecord.d3 * t3 - sRecord.d4 * t4;
    tempe = tempe + sRecord.dbBSTAR * sRecord.cc5 * (sin(mm) - sRecord.sinmao);
    templ = templ + sRecord.t3cof * t3 + t4 * (sRecord.t4cof + dbTime * sRecord.t5cof);

    double nm = sRecord.no;
    double em = sRecord.dbEccentricity;
    double inclm = sRecord.dbInclination;

    double am = pow((xke / nm), 2 / 3.f) * tempa * tempa;
    nm = xke / pow(am, 1.5);
    em = em - tempe;

    if (em < 1.0e-6)
        em = 1.0e-6;
    mm = mm + sRecord.no * templ;
    double xlm = mm + argpm + nodem;
    double emsq = em * em;
    temp = 1.0 - emsq;

    nodem = (nodem >= 0 ? (int)(nodem * 100000) % (int)(2 * PI * 100000) : -((int)(-nodem * 100000) % (int)(2 * PI * 100000))) / 100000.f;
    argpm = (int)(argpm * 100000) % (int)(2 * PI * 100000) / 100000.f;
    xlm = (int)(xlm * 100000) % (int)(2 * PI * 100000) / 100000.f;
    mm = (int)((xlm - argpm - nodem) * 100000) % (int)(2 * PI * 100000) / 100000.f;

    double sinim = sin(inclm);
    double cosim = cos(inclm);

    double ep = em;
    double xincp = inclm;
    double argpp = argpm;
    double nodep = nodem;
    double mp = mm;
    double sinip = sinim;
    double cosip = cosim;

    double axnl = ep * cos(argpp);
    temp = 1.0 / (am * (1.0 - ep * ep));
    double aynl = ep * sin(argpp) + temp * sRecord.aycof;
    double xl = mp + argpp + nodep + temp * sRecord.xlcof * axnl;


    double u = (int)((xl - nodep) * 100000) % (int)(2 * PI * 100000) / 100000.f;
    double eo1 = u;
    double tem5 = 9999.9;
    double ktr = 1;
    double sineo1, coseo1;
    while (fabs(tem5) >= 1.0e-12 && ktr <= 10){


        sineo1 = sin(eo1);
        coseo1 = cos(eo1);
        tem5 = 1.0 - coseo1 * axnl - sineo1 * aynl;
        tem5 = (u - aynl * coseo1 + axnl * sineo1 - eo1) / tem5;
        if (fabs(tem5) >= 0.95)
            tem5 = (tem5 > 0 ? 0.95 : -0.95);
        eo1 = eo1 + tem5;
        ktr = ktr + 1;
    }

    double ecose = axnl * coseo1 + aynl * sineo1;
    double esine = axnl * sineo1 - aynl * coseo1;
    double el2 = axnl * axnl + aynl * aynl;
    double pl = am * (1.0 - el2);

    double rl = am * (1.0 - ecose);
    double rdotl = sqrt(am) * esine / rl;
    double rvdotl = sqrt(pl) / rl;
    double betal = sqrt(1.0 - el2);
    temp = esine / (1.0 + betal);
    double sinu = am / rl * (sineo1 - aynl - axnl * temp);
    double cosu = am / rl * (coseo1 - axnl + aynl * temp);
    double su = atan2(sinu, cosu);
    double sin2u = (cosu + cosu) * sinu;
    double cos2u = 1.0 - 2.0 * sinu * sinu;
    temp = 1.0 / pl;
    double temp1 = 0.5 * j2 * temp;
    double temp2 = temp1 * temp;

    double mrt = rl * (1.0 - 1.5 * temp2 * betal * sRecord.con41) + 0.5 * temp1 * sRecord.x1mth2 * cos2u;
    su = su - 0.25 * temp2 * sRecord.x7thm1 * sin2u;
    double xnode = nodep + 1.5 * temp2 * cosip * sin2u;
    double xinc = xincp + 1.5 * temp2 * cosip * sinip * cos2u;
    double mvt = rdotl - nm * temp1 * sRecord.x1mth2 * sin2u / xke;
    double rvdot = rvdotl + nm * temp1 * (sRecord.x1mth2 * cos2u + 1.5 * sRecord.con41) / xke;

    double sinsu = sin(su);
    double cossu = cos(su);
    double snod = sin(xnode);
    double cnod = cos(xnode);
    double sini = sin(xinc);
    double cosi = cos(xinc);
    double xmx = -snod * cosi;
    double xmy = cnod * cosi;
    double ux = xmx * sinsu + cnod * cossu;
    double uy = xmy * sinsu + snod * cossu;
    double uz = sini * sinsu;
    double vx = xmx * cossu - cnod * sinsu;
    double vy = xmy * cossu - snod * sinsu;
    double vz = sini * cossu;

    double _mr = mrt * EARTH_RADIUS;
    double r[3] = { _mr * ux, _mr * uy, _mr * uz };
    double vkmpersec = EARTH_RADIUS * xke / 60.0;
    double v[3] = { (mvt * ux + rvdot * vx) * vkmpersec,(mvt * uy + rvdot * vy) * vkmpersec,(mvt * uz + rvdot * vz) * vkmpersec };

    std::cout << r[0] << " | " << v[0] << std::endl;
}

void ConvertToSGP4Units(SatelliteRecord sRecord) {
    sRecord.dbMeanMotion /= DOT_PRODUCT;

    sRecord.dbMeanMotionDOT /= DOT_PRODUCT * 1440;
    sRecord.dbMeanMotionDDOT /= DOT_PRODUCT * 1440 * 1440;

    sRecord.dbInclination *= DEG_2_RAD;
    sRecord.dbRightAscNode *= DEG_2_RAD;
    sRecord.dbArgOfPerigee *= DEG_2_RAD;
    sRecord.dbMeanAnomaly *= DEG_2_RAD;

    std::stringstream sEpoch;
    sEpoch << sRecord.szEpoch;
    std::string line;
    int year, month, day, hour, min;
    double seconds;
    std::getline(sEpoch, line, '-');

    year = strtol(line.c_str(), 0, 10);
    std::getline(sEpoch, line, '-');

    month = strtol(line.c_str(), 0, 10);
    std::getline(sEpoch, line, 'T');

    day = strtol(line.c_str(), 0, 10);

    std::getline(sEpoch, line, ':');
    hour = strtol(line.c_str(), 0, 10);

    std::getline(sEpoch, line, ':');
    min = strtol(line.c_str(), 0, 10);

    std::getline(sEpoch, line);
    seconds = strtod(line.c_str(), 0);

    sRecord.dbEpochJulian = MakeJulianDate(year, month, day, hour, min, seconds);
    InitSGP4(sRecord);

}