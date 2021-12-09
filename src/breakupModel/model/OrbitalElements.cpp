#include "OrbitalElements.h"

bool Epoch::isInvalid() const  {
    return year < 0 || fraction < 0;
}

std::tm Epoch::toTm() const {
    std::tm tmInstance{};
    //Year
    tmInstance.tm_year = year;

    //Month and Day
    const int dayInYear = static_cast<int>(fraction);
    tmInstance.tm_yday = dayInYear;
    int dayInMonth = dayInYear;
    for (const auto&[month, days] : monthToDaysMap) {
        if (dayInMonth <= days) {
            tmInstance.tm_mon = month;
            tmInstance.tm_mday = dayInMonth;
            break;
        } else {
            dayInMonth -= days;
        }
    }
    //Hours
    double fractionOfDay = fraction - dayInYear;
    double hours = fractionOfDay * 24;
    tmInstance.tm_hour = static_cast<int>(hours);

    //Minutes
    fractionOfDay = hours - tmInstance.tm_hour;
    double minutes = fractionOfDay * 60;
    tmInstance.tm_min = static_cast<int>(minutes);

    //Seconds
    fractionOfDay = minutes - tmInstance.tm_min;
    double seconds = fractionOfDay * 60;
    tmInstance.tm_sec = static_cast<int>(seconds);

    return tmInstance;
}

std::array<double, 6> OrbitalElements::getAsArray() const {
    return std::array<double, 6>{_semiMajorAxis, _eccentricity, _inclination,
                                 _longitudeOfTheAscendingNode, _argumentOfPeriapsis, _eccentricAnomaly};
}

double OrbitalElements::operator[](size_t index) const {
    switch (index) {
        case OrbitalElement::SEMI_MAJOR_AXIS:
            return _semiMajorAxis;
        case OrbitalElement::ECCENTRICITY:
            return _eccentricity;
        case OrbitalElement::INCLINATION:
            return _inclination;
        case OrbitalElement::LONGITUDE_OF_THE_ASCENDING_NODE:
            return _longitudeOfTheAscendingNode;
        case OrbitalElement::ARGUMENT_OF_PERIAPSIS:
            return _argumentOfPeriapsis;
        case OrbitalElement::ECCENTRIC_ANOMALY:
            return _eccentricAnomaly;
        default:
            throw std::out_of_range{"The Orbital elements only consists of six elements --> range[0;5]!"};
    }
}

double OrbitalElements::getSemiMajorAxis() const {
    return _semiMajorAxis;
}

double OrbitalElements::getEccentricity() const {
    return _eccentricity;
}

double OrbitalElements::getInclination(AngularUnit angularUnit) const {
    return convertAngle(_inclination, angularUnit);
}

double OrbitalElements::getLongitudeOfTheAscendingNode(AngularUnit angularUnit) const {
    return convertAngle(_longitudeOfTheAscendingNode, angularUnit);
}

double OrbitalElements::getArgumentOfPeriapsis(AngularUnit angularUnit) const {
    return convertAngle(_argumentOfPeriapsis, angularUnit);
}

double OrbitalElements::getAnomaly(AngularUnit angularUnit, OrbitalAnomalyType anomalyType) const {
    return convertEccentricAnomaly(_eccentricAnomaly, _eccentricity, angularUnit, anomalyType);
}

Epoch OrbitalElements::getEpoch() const {
    return _epoch;
}

bool operator==(const OrbitalElements &lhs, const OrbitalElements &rhs) {
    return lhs._semiMajorAxis == rhs._semiMajorAxis &&
           lhs._eccentricity == rhs._eccentricity &&
           lhs._inclination == rhs._inclination &&
           lhs._longitudeOfTheAscendingNode == rhs._longitudeOfTheAscendingNode &&
           lhs._argumentOfPeriapsis == rhs._argumentOfPeriapsis &&
           lhs._eccentricAnomaly == rhs._eccentricAnomaly;
}

bool operator!=(const OrbitalElements &lhs, const OrbitalElements &rhs) {
    return !(rhs == lhs);
}


double OrbitalElements::convertAngle(double angle, AngularUnit targetAngularUnit) {
    return targetAngularUnit == AngularUnit::RADIAN ? angle : util::radToDeg(angle);
}

double OrbitalElements::convertEccentricAnomaly(double anomaly, double eccentricity, AngularUnit targetAngularUnit,
                                                OrbitalAnomalyType targetOrbitalAnomalyType) {
    switch (targetOrbitalAnomalyType) {
        case OrbitalAnomalyType::ECCENTRIC:
            break;
        case OrbitalAnomalyType::MEAN:
            anomaly = util::eccentricAnomalyToMeanAnomaly(anomaly, eccentricity);

            break;
        case OrbitalAnomalyType::TRUE:
            anomaly = util::eccentricAnomalyToTrueAnomaly(anomaly, eccentricity);
            break;
    }
    if (targetAngularUnit == AngularUnit::DEGREE) {
        anomaly = util::radToDeg(anomaly);
    }
    return anomaly;
}

std::ostream &operator<<(std::ostream &os, const OrbitalElements &elements) {
    os << "OrbitalElements{"
       << "_semiMajorAxis: " << elements._semiMajorAxis << " _eccentricity: " << elements._eccentricity
       << " _inclination: " << elements._inclination << " _longitudeOfTheAscendingNode: "
       << elements._longitudeOfTheAscendingNode << " _argumentOfPeriapsis: " << elements._argumentOfPeriapsis
       << " _eccentricAnomaly: " << elements._eccentricAnomaly << '}';
    return os;
}
