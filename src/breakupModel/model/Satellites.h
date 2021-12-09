#pragma once

#include <vector>
#include <array>
#include <variant>
#include <tuple>
#include <algorithm>
#include <memory>

#include "Satellite.h"

/**
 * This class implements the Satellites in an SoA (Structure of Array) way.
 * This is especially useful for the BreakupSimulation to vectorized calculation and save memory for shared
 * properties of the fragment satellites created.
 * @note This class provides a method to copy the SoA structure into an AoS approach (std::vector<Satellite>)
 */
class Satellites {

public:

    /*
     * Shared Properties
     */

    /**
     * The NORAD Catalog ID of the first Satellite in the SoA
     * That means this contains Satellites from [StartID, StartID+size]
     */
    size_t startId{};

    /**
     * The SatType of the Satellites in the SoA
     */
    SatType satType{SatType::DEBRIS};

    /**
     * The position base of the Satellites in the SoA
     * This is a cartesian position vector in [m]
     */
    std::array<double, 3> position{};

    /*
     * Unique Properties
     */

    /**
     * The name of each of the Satellites in the SoA
     */
    std::vector<std::shared_ptr<const std::string>> name;

    /**
     * The characteristic length of each satellite in [m]
     */
    std::vector<double> characteristicLength;

    /**
     * The area-to-mass ratio of each satellite in [m^2/kg]
     */
    std::vector<double> areaToMassRatio;

    /**
     * The mass of each satellite in [kg]
     */
    std::vector<double> mass;

    /**
     * The area/ Radar-Cross-Section of each satellite in [m^2]
     */
    std::vector<double> area;

    /**
     * The ejection velocity of each satellite in [m/s]
     * This is a cartesian velocity vector.
     */
    std::vector<std::array<double, 3>> ejectionVelocity;

    /**
     * The velocity of each satellite in [m/s]
     * This is a cartesian velocity vector in which each element is the sum of ejection and (parental) base velocity
     */
    std::vector<std::array<double, 3>> velocity;

    Satellites() = default;

    Satellites(size_t startID, SatType satType, std::array<double, 3> position, size_t size)
            : startId{startID},
              satType{satType},
              position{position} {
        this->resize(size);
    }

    /**
     * Returns this Structure of Arrays as an Array of Structures.
     * @return vector of Satellites
     */
    std::vector<Satellite> getAoS() const;

    /**
     * Returns a tuple view of this Satellites collection containing in the order of appearance:
     * characteristic Length, velocity, ejection velocity
     * @return vector of tuple of characteristic Length, velocity, ejection velocity
     */
    std::vector<std::tuple<double &, std::array<double, 3> &, std::array<double, 3>&>> getVelocityTuple();

    /**
    * Returns a tuple view of this Satellites collection containing in the order of appearance:
    * characteristic Length, area-to-mass-ratio, area, mass
    * @return vector of tuple of characteristic Length, area-to-mass-ratio, area, mass
    */
    std::vector<std::tuple<double &, double &, double &, double &>> getAreaMassTuple();

    /**
    * Returns a tuple view of this Satellites collection containing in the order of appearance:
    * characteristic Length, mass, velocity and name pointer
    * @return vector of tuple of characteristic Length, mass, velocity and name pointer
    */
    std::vector<std::tuple<double &, double &, std::array<double, 3> &, std::shared_ptr<const std::string> &>> getCMVNTuple();

    /**
    * Returns a tuple view of this Satellites collection containing in the order of appearance:
    * velocity and name pointer
    * @return vector of tuple of velocity and name pointer
    */
    std::vector<std::tuple<std::array<double, 3> &, std::shared_ptr<const std::string> &>> getVNTuple();

    /**
     * Returns the size of this element.
     * @return size
     */
    size_t size() const {
        return characteristicLength.size();
    }

    /**
     * Resizes the Satellites SoA to a new size.
     * @param newSize
     */
    void resize(size_t newSize) {
        name.resize(newSize);
        characteristicLength.resize(newSize);
        areaToMassRatio.resize(newSize);
        mass.resize(newSize);
        area.resize(newSize);
        ejectionVelocity.resize(newSize);
        velocity.resize(newSize);
    }

    /**
     * Removes the last element from this Satellites Structure.
     * This resizes the interior vectors to a size one smaller than before the method call.
     */
    void popBack();

    /**
     * This resizes the Structure by one additional Slot and returns references to the
     * characteristic length, area-mass-ratio, area and mass of the new element.
     * @return tuple of references to characteristic length, area-mass-ratio, area and mass
     */
    std::tuple<double &, double &, double &, double &> appendElement();

};
