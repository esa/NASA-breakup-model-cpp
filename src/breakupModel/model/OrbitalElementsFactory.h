#pragma once

#include <utility>
#include <tuple>
#include "OrbitalElements.h"
#include "breakupModel/util/UtilityKepler.h"
#include "breakupModel/util/UtilityFunctions.h"

/**
 * Provides methods to create Orbital Elements.
 */
class OrbitalElementsFactory {

public:

    OrbitalElementsFactory() = default;

    /**
    * Calculates the cartesian velocity and cartesian position of this satellite by using the Keplerian Elements.
    * This function sets the internal members _velocity and _position to the corresponding values.
    * @param keplerianElements array holds the arguments in the following order:<br>
    * mm - mean motion [revolutions/day]<br>
    * e - eccentricity<br>
    * i - inclination [deg]<br>
    * W - longitude of the ascending node (big omega) [deg]<br>
    * w - argument of periapsis (small omega) [deg]<br>
    * MA - Mean Anomaly [deg]<br>
    * @param epoch - the Epoch of these Elements (default empty)
    */
    [[nodiscard]] OrbitalElements createFromTLEData(const std::array<double, 6> &tleData,
                                                    const Epoch &epoch = Epoch{}) const;

    /**
    * Constructs the Orbital Elements from the standard keplerian Elements in the following order:
    * @param keplerianElements array holds the arguments in the following order:<br>
    * a - semir-major axis [m]<br>
    * e - eccentricity<br>
    * i - inclination [rad]<br>
    * W - longitude of the ascending node (big omega) [rad]<br>
    * w - argument of periapsis (small omega) [rad]<br>
    * A - an anomaly in [rad]<br>
    * @param orbitalAnomalyType - this defines the last element in the array, either: ECCENTRIC, MEAN or TRUE
    * @param epoch - the Epoch of these Elements (default empty)
    */
    [[nodiscard]] OrbitalElements
    createFromOnlyRadians(const std::array<double, 6> &standardKepler, OrbitalAnomalyType orbitalAnomalyType,
                          const Epoch &epoch = Epoch{}) const;

    /**
    * Constructs the Orbital Elements from the standard keplerian Elements in the following order:
    * @param keplerianElements array holds the arguments in the following order:<br>
    * a - semir-major axis [m]<br>
    * e - eccentricity<br>
    * i - inclination [deg]<br>
    * W - longitude of the ascending node (big omega) [deg]<br>
    * w - argument of periapsis (small omega) [deg]<br>
    * A - an anomaly in [deg]<br>
    * @param orbitalAnomalyType - this defines the last element in the array, either: ECCENTRIC, MEAN or TRUE
    * @param epoch - the Epoch of these Elements (default empty)
    */
    [[nodiscard]] OrbitalElements
    createFromOnlyDegree(const std::array<double, 6> &standardKepler, OrbitalAnomalyType orbitalAnomalyType,
                         const Epoch &epoch = Epoch{}) const;

    /**
     * Creates the orbital elements from the user's wish units:
     * @param a - semi-major-axis in [m]
     * @param eccentricity - eccentricity (unit-less)
     * @param inclination - inclination
     * @param inclinationU - unit of inclination, either [deg] or [rad]
     * @param raan - longitude of the ascending node (big omega)
     * @param raanU - unit of raan, either [deg] or [rad]
     * @param argOfPer - argument of periapsis (small omega)
     * @param argOfPerU - unit of argument of periapsis, either [deg] or [rad]
     * @param anomaly - the value of the anomaly
     * @param anomalyU - the unit of the anomaly, either [deg] or [rad]
     * @param anomalyType - the type (MEAN/ ECCENTRIC/ TRUE)
     * @return OrbitalElements
     */
    [[nodiscard]] OrbitalElements
    createOrbitalElements(double a, double eccentricity, double inclination, AngularUnit inclinationU, double raan,
                          AngularUnit raanU, double argOfPer, AngularUnit argOfPerU, double anomaly,
                          AngularUnit anomalyU, OrbitalAnomalyType anomalyType) const;


};
