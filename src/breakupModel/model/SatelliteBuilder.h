#pragma once

#include "Satellite.h"
#include "breakupModel/util/UtilityFunctions.h"
#include <string>
#include <sstream>
#include <array>
#include <exception>
#include "OrbitalElements.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

/**
 * General Idea is that there a multiple ways to construct a valid satellite object to run the simulation with it.
 * So instead of having countless constructor for every combination of valid input data, this class provides a more
 * convenient way. Just add values and the class will tell if it works.
 * @note The following things are needed or (optional):<br>
 * - ID<br>
 * - (Name)<br>
 * - (Type) if not given the Builder will choose SPACECRAFT<br>
 * - mass OR area<br>
 * - velocity vector OR Keplerian elements<br>
 *
 * @remark Explanation:<br>
 * - mass is necessary, either directly or it is derived with the density formular from the radar cross section (area)
 * or the characteristic length<br>
 * - velocity vector is needed for the non-catastrophic collision
 */
class SatelliteBuilder {

    Satellite _satellite;

    bool _hasID;
    bool _hasMass;
    bool _hasVelocity;
    bool _hasPosition;  //Unused, maybe useful for further modeling after breakup

public:

    SatelliteBuilder()
            : _satellite{},
              _hasID{false},
              _hasMass{false},
              _hasVelocity{false},
              _hasPosition{false} {}

    /**
     * Resets the Builder to the initial state.
     * @return this
     */
    SatelliteBuilder &reset();

    /**
     * Adds an ID to the satellite or replaces an already existing ID. Can therefore be called multiple times if wished.
     * @param id from e. g. the NORAD Catalog
     * @return this
     */
    SatelliteBuilder &setID(unsigned long id);

    /**
     * Adds an optional name to the satellite.
     * @param name of the satellite e. g. "Iridium 33"
     * @return this
     */
    SatelliteBuilder &setName(const std::string &name);

    /**
    * Adds an optional name to the satellite.
    * @param name of the satellite e. g. "Iridium 33"
    * @return this
    */
    SatelliteBuilder &setName(std::string &&name);

    /**
     * Adds the SatelliteType. If not used the builder will use the default from Satellite class: SPACECRAFT.
     * @param satType either SPACECRAFT, ROCKET_BODY, DEBRIS, UNKNOWN
     * @return this
     */
    SatelliteBuilder &setSatType(SatType satType);

    /**
     * Adds the SatelliteType. If not used the builder will use the default from Satellite class: SPACECRAFT.
     * @param satType as string
     * @return this
     * @throws May throw an runtime_error Exception if the satType String can not be mapped to a SatType!
     */
    SatelliteBuilder &setSatType(const std::string &satType);

    /**
     * Adds the SatelliteType. If not used the builder will use the default from Satellite class: SPACECRAFT.
     * @param satType as string
     * @return this
     * @throws May throw an runtime_error Exception if the satType String can not be mapped to a SatType!
     */
    SatelliteBuilder &setSatType(std::string &&satType);

    /**
     * Sets the mass and only the mass of the satellite.
     * The area and characteristic length can not be determined by the mass, so they are each set to -1.
     * @param mass in [kg]
     * @return this
     * @attention This will override previous attempts of setting the mass!
     */
    SatelliteBuilder &setMass(double mass);

    /**
     * Sets the area and derives the mass and the characteristic length. The characteristic length or radius is
     * determined by assuming the area is a circle. The mass is calculated by using this radius as a sphere of
     * the density described in Equation 1.
     * @param area in [m^2]
     * @return this
     * @attention This will override previous attempts of setting the mass!
     */
    SatelliteBuilder &setMassByArea(double area);

    /**
     * Sets the velocity.
     * @param velocity a 3 dimensional velocity vector
     * @return this
     * @attention This will override previous attempts of setting the velocity (e. g. with Keplerian Elements)
     */
    SatelliteBuilder &setVelocity(const std::array<double, 3> &velocity);

    /**
    * Sets the cartesian position of the satellite.
    * @param position a 3 dimensional vector
    * @return this
    * @attention This will override previous attempts of setting the position (e. g. with Keplerian Elements)
    */
    SatelliteBuilder &setPosition(const std::array<double, 3> &position);

    /**
     * Sets the position and velocity of the satellite by using the Keplerian Elements.
     * @param orbitalElements holds the Keplerian Elements
     * @return this
     * @attention This will override previous attempts of setting the velocity/ position.
     */
    SatelliteBuilder &setOrbitalElements(const OrbitalElements &orbitalElements);

    /**
     * Returns the fully build satellite. Validates if all necessary parameters are specified.
     * @return Satellite
     * @throws May throw an invalid_argument Exception if not all requirements for a satellite a fulfilled!
     */
    Satellite &getResult();

};


