#pragma once

#ifndef SATELLITE_RECORD_H
#define SATELLITE_RECORD_H



class SatelliteRecord {
public:
    char szName[128];
    char szObjId[128];
    char szEpoch[256];



    char szClassification;
    //int iIntlDesignation;
    double dbEpochJulian;
    double dbMeanMotionDOT;
    double dbMeanMotionDDOT;
    double dbBSTAR;
    int iEphemerisType;
    int iElementNumber;

    //int iSatNum;
    double dbInclination;
    double dbRightAscNode;
    double dbEccentricity;
    double dbArgOfPerigee;
    double dbMeanAnomaly;
    double dbMeanMotion;
    int iRevsAtEpoch;
};

class SGP4Sat {
public:
    double no;
    double con41;
    double cc1;
    double cc3;
    double cc4;
    double cc5;
    double x1mth2;
    double mdot;
    double nodedot;
    double argpdot;
    double nodecf;
    double t2cof, t3cof, t4cof, t5cof;
    double aycof;
    double omgcof;
    double xmcof;
    double xlcof;
    double delmotemp;
    double delmo;
    double sinmao;
    double x7thm1;
    double d1, d2, d3, d4;

    double dbInclination;
    double dbRightAscNode;
    double dbEccentricity;
    double dbArgOfPerigee;
    double dbMeanAnomaly;
    double dbMeanMotion;
    double dbEta;
    double dbBSTAR;

};
#endif // !SATELLITE_RECORD_H
