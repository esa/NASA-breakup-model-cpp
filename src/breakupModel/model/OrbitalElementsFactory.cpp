#include "OrbitalElementsFactory.h"

OrbitalElements OrbitalElementsFactory::createFromTLEData(const std::array<double, 6> &tleData, const Epoch &epoch) const {
    auto degKepler = tleData;
    degKepler[0] = util::meanMotionToSemiMajorAxis(degKepler[0]);
    return createFromOnlyDegree(degKepler, OrbitalAnomalyType::MEAN, epoch);
}

OrbitalElements OrbitalElementsFactory::createFromOnlyRadians(const std::array<double, 6> &standardKepler,
                                                      OrbitalAnomalyType orbitalAnomalyType, const Epoch &epoch) const {
    double anomaly = standardKepler[5];
    if (orbitalAnomalyType == OrbitalAnomalyType::MEAN) {
        anomaly = util::meanAnomalyToEccentricAnomaly(anomaly, standardKepler[1]);
    } else if (orbitalAnomalyType == OrbitalAnomalyType::TRUE) {
        anomaly = util::trueAnomalyToEccentricAnomaly(anomaly, standardKepler[1]);
    }   //else ECCENTRIC --> right format

    return OrbitalElements(standardKepler[0], standardKepler[1], standardKepler[2],
                           standardKepler[3], standardKepler[4], anomaly, epoch);
}

OrbitalElements OrbitalElementsFactory::createFromOnlyDegree(const std::array<double, 6> &standardKepler,
                                                 OrbitalAnomalyType orbitalAnomalyType, const Epoch &epoch) const {
    auto radKepler = standardKepler;
    for (unsigned int i = 2; i < 6; ++i) {
        radKepler[i] = util::degToRad(radKepler[i]);
    }
    return createFromOnlyRadians(radKepler, orbitalAnomalyType, epoch);
}

OrbitalElements OrbitalElementsFactory::createOrbitalElements(double a, double eccentricity, double inclination,
                                                              AngularUnit inclinationU, double raan, AngularUnit raanU,
                                                              double argOfPer, AngularUnit argOfPerU, double anomaly,
                                                              AngularUnit anomalyU, OrbitalAnomalyType anomalyType) const {

    //Convert angles to RADIAN
    inclination = inclinationU == AngularUnit::RADIAN ? inclination  : util::degToRad(inclination);
    raan        = raanU        == AngularUnit::RADIAN ? raan         : util::degToRad(raan);
    argOfPer    = argOfPerU    == AngularUnit::RADIAN ? argOfPer     : util::degToRad(argOfPer);
    anomaly     = anomalyU     == AngularUnit::RADIAN ? anomaly      : util::degToRad(anomaly);

    //Convert Anomaly to eccentric
    if (anomalyType == OrbitalAnomalyType::MEAN) {
        anomaly = util::meanAnomalyToEccentricAnomaly(anomaly, eccentricity);
    } else if (anomalyType == OrbitalAnomalyType::TRUE) {
        anomaly = util::trueAnomalyToEccentricAnomaly(anomaly, eccentricity);
    } //else it is already eccentric

    return OrbitalElements{a,eccentricity, inclination, raan, argOfPer, anomaly};
}
