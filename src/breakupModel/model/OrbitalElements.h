#pragma once

#include <array>
#include <stdexcept>
#include <ostream>
#include <ctime>
#include <map>
#include "breakupModel/util/UtilityKepler.h"
#include "breakupModel/util/UtilityFunctions.h"

/**
 * Unit of Angles
 */
enum class AngularUnit {
    DEGREE, RADIAN
};

/**
 * Three different Orbital Anomalies are possible
 */
enum class OrbitalAnomalyType {
    ECCENTRIC, MEAN, TRUE
};

/**
 * The orbital Elements in the order in which they are saved in the often used "uniform" array of size six.
 * This enum fulfills the more "readability" purpose.
 */
enum OrbitalElement {
    SEMI_MAJOR_AXIS = 0,
    ECCENTRICITY = 1,
    INCLINATION = 2,
    LONGITUDE_OF_THE_ASCENDING_NODE = 3,
    ARGUMENT_OF_PERIAPSIS = 4,
    ECCENTRIC_ANOMALY = 5
};

/**
 * This struct saves an Epoch for Orbital Elements. It contains of two elements:<br>
 * - The year (e. g. 2006)<br>
 * - The day + fraction of a day (e. g. 31.25992506 --> translates to day 31 of the year (would be January) and
 *   6 hours 14 minutes and 17.52 second)
 *  Calculation of the hours follows this scheme: floor(0.25992506 * 24) = 6 hours
 *  The Epoch is invalid if the values inside are negative
 */
struct Epoch {

    /**
     * Mapping of Month to number of Days
     * (Counting from zero in case of months, [see ctime std::tm])
     */
    static const inline std::map<int, int> monthToDaysMap{
            {0,  31},    //January 31 days
            {1,  28},    //February 28 days
            {2,  31},    //March 31 days
            {3,  30},    //April 30 days
            {4,  31},    //May 31 days
            {5,  30},    //June 30 days
            {6,  31},    //July 31 days
            {7,  31},    //August 31 days
            {8,  30},    //September 30 days
            {9,  31},    //October 31 days
            {10, 30},    //November 30 days
            {11, 30}     //December 31 days
    };

    /**
     * The year of the Epoch
     */
    int year;

    /**
     * The day + the fraction of the day
     */
    double fraction;

    Epoch()
            : year{-1},
              fraction{-1} {}

    Epoch(int year, double fraction)
            : year{year},
              fraction{fraction} {}

    /**
     * Returns true if this Epoch contains invalid numbers.
     * @return true if invalid
     */
    [[nodiscard]] bool isInvalid() const;

    /**
     * Transforms the epoch into an ctime std::tm instance.
     * @return std::tm from ctime
     */
    [[nodiscard]] std::tm toTm() const;
};

class OrbitalElements {

    /**
     * The semi-major-axis in [m]
     */
    double _semiMajorAxis{0};

    /**
     * The eccentricity (unit-less)
     */
    double _eccentricity{0};

    /**
     * The inclination in [rad]
     */
    double _inclination{0};

    /**
     * The longitude-of-the-ascending-node or also called RAAN in [rad]
     */
    double _longitudeOfTheAscendingNode{0};

    /**
     * The argument-of-periapsis in [rad]
     */
    double _argumentOfPeriapsis{0};

    /**
     * One orbital anomaly given in [rad]
     */
    double _eccentricAnomaly{0};

    /**
     * The epoch (time stamp) of this orbital elements
     */
    Epoch _epoch{};

public:

    OrbitalElements() = default;

    /**
    * Creates the Orbital Elements.
    * @param a - semir-major axis [m]
    * @param e - eccentricity
    * @param i - inclination [rad]
    * @param W - longitude of the ascending node (big omega) [rad]
    * @param w - argument of periapsis (small omega) [rad]
    * @param EA - eccentric Anomaly [rad]
    * @param epoch - the epoch of these Orbital Elements (default empty)
    * This is the "inverse" of getAsArray.
    * @note Use the OrbitalElementsFactory instead of this constructor!
    */
    OrbitalElements(double semiMajorAxis, double eccentricity, double inclination, double longitudeOfTheAscendingNode,
                    double argumentOfPeriapsis, double eccentricAnomaly, const Epoch &epoch = Epoch{}) :
            _semiMajorAxis{semiMajorAxis},
            _eccentricity{eccentricity},
            _inclination{inclination},
            _longitudeOfTheAscendingNode{longitudeOfTheAscendingNode},
            _argumentOfPeriapsis{argumentOfPeriapsis},
            _eccentricAnomaly{eccentricAnomaly},
            _epoch{epoch} {}

    /**
     * Creates the Orbital Elements from an array which contains the six elements in the following order:
     * @param uniformOrbitalElements<br>
     * a - semir-major axis [m]<br>
     * e - eccentricity<br>
     * i - inclination [rad]<br>
     * W - longitude of the ascending node (big omega) [rad]<br>
     * w - argument of periapsis (small omega) [rad]<br>
     * EA - eccentric Anomaly [rad]<br>
     * @param epoch - the epoch of these Orbital Elements (default empty)
     * This is the "inverse" of getAsArray.
     * @note Use the OrbitalElementsFactory instead of this constructor!
     */
    explicit OrbitalElements(const std::array<double, 6> &uniformOrbitalElements, Epoch epoch = Epoch{})
            : _semiMajorAxis{uniformOrbitalElements[0]},
              _eccentricity{uniformOrbitalElements[1]},
              _inclination{uniformOrbitalElements[2]},
              _longitudeOfTheAscendingNode{uniformOrbitalElements[3]},
              _argumentOfPeriapsis{uniformOrbitalElements[4]},
              _eccentricAnomaly{uniformOrbitalElements[5]},
              _epoch{epoch} {}

    /**
     * Returns the orbital Element in an array. The semi-major-axis in [m], the eccentricity unit-less,
     * the angles in [RAD] and lastly the eccentric anomaly in [RAD] too.
     * This is the "inverse" of the OrbitalElements(const std::array<double, 6>) constructor.
     * @return array<a, e, i, W, w, eccentric-anomaly>
     */
    [[nodiscard]] std::array<double, 6> getAsArray() const;

    /**
     * Returns one element from the uniform "view" of the Keplerian Elements.
     * @param index in the array
     * @return element in array<a, e, i, W, w, eccentric-anomaly>
     * @throws an out_of_range exception if index out of range [0;5]
     */
    double operator[](size_t index) const;

    /**
     * Returns the semi-major-axis in [m]
     * @return semi-major-axis
     */
    [[nodiscard]] double getSemiMajorAxis() const;

    /**
     * Returns the eccentricity (unit-less)
     * @return eccentricity
     */
    [[nodiscard]] double getEccentricity() const;

    /**
     * Returns the inclination in [DEG] or [RAD] (default)
     * @param angularUnit
     * @return inclination
     */
    [[nodiscard]] double getInclination(AngularUnit angularUnit = AngularUnit::RADIAN) const;

    /**
    * Returns the longitude-of-the-ascending-node in [DEG] or [RAD] (default)
    * @param angularUnit
    * @return longitude-of-the-ascending-node
    */
    [[nodiscard]] double getLongitudeOfTheAscendingNode(AngularUnit angularUnit = AngularUnit::RADIAN) const;

    /**
    * Returns the argument-of-periapsis in [DEG] or [RAD] (default)
    * @param angularUnit
    * @return argument-of-periapsis
    */
    [[nodiscard]] double getArgumentOfPeriapsis(AngularUnit angularUnit = AngularUnit::RADIAN) const;

    /**
    * Returns an orbital anomaly in [DEG] or [RAD] (default); Available types are MEAN/ ECCENTRIC (default) / TRUE Anomaly.
    * @param angularUnit
    * @return orbital anomaly
    */
    [[nodiscard]] double getAnomaly(AngularUnit angularUnit = AngularUnit::RADIAN,
                                    OrbitalAnomalyType orbitalAnomalyType = OrbitalAnomalyType::ECCENTRIC) const;

    /**
     * Returns the epoch of these Orbital Elements
     * @return Epoch
     */
    [[nodiscard]] Epoch getEpoch() const;

    /**
     * Compares two OrbitalElements for equality.
     * @param lhs - OrbitalElements
     * @param rhs - OrbitalElements
     * @return true if they are exactly the same
     */
    friend bool operator==(const OrbitalElements &lhs, const OrbitalElements &rhs);

    /**
     * Compares two OrbitalElements for inequality.
     * @param lhs - OrbitalElements
     * @param rhs - OrbitalElements
     * @return true if they differ
     */
    friend bool operator!=(const OrbitalElements &lhs, const OrbitalElements &rhs);

    /**
     * Prints the orbital elements in an human friendly format to os.
     * @param os - ostream
     * @param elements - the Orbital Elements
     * @return os
     */
    friend std::ostream &operator<<(std::ostream &os, const OrbitalElements &elements);

private:

    /**
     * Converts a given angle in rad into a given target unit
     * @param angle - angle in [rad]
     * @param targetAngularUnit - target unit
     * @return the value in the target unit
     */
    static double convertAngle(double angle, AngularUnit targetAngularUnit);

    /**
     * Converts a given eccentric anomaly into a given target anomaly in a specific target angular unit.
     * @param eccentricAnomaly - eccentric anomaly in [rad]
     * @param eccentricity - the eccentricity
     * @param targetAngularUnit - the target angular unit
     * @param targetOrbitalAnomalyType - target anomaly MEAN/ ECCENTRIC/ TRUE Anomaly
     * @return value of chosen anomaly in target angular unit
     */
    static double convertEccentricAnomaly(double eccentricAnomaly, double eccentricity, AngularUnit targetAngularUnit,
                                          OrbitalAnomalyType targetOrbitalAnomalyType);

};