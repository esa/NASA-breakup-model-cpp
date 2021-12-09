#pragma once

#include <string>
#include <memory>
#include <iostream>
#include <map>
#include <set>
#include <optional>
#include <exception>
#include "DataSource.h"

/**
 * (Expressive) Return type for getTypeOfSimulation.
 */
enum class SimulationType {
    /**
     * Describes a Simulation Breakup Event.
     */
    COLLISION,

    /**
     * Describes a Explosion Breakup Event.
     */
    EXPLOSION,

    /**
     * Simulation Type unknown and not defined by the user.
     * The processing BreakupBuilder will try to derive the Type from the number of satellites in the input (with
     * warning).
     */
    UNKNOWN
};

/**
 * Pure virtual Interface for Control Input.
 * Provides methods to get config data, like minimal L_c, type of simulation, maximal given NORAD-ID or
 * the source file(s) for Satellite Data.
 */
class InputConfigurationSource {

public:

    inline const static std::map<std::string, SimulationType> stringToSimulationType{
            {"COLLISION", SimulationType::COLLISION},
            {"CO",       SimulationType::COLLISION},
            {"EXPLOSION", SimulationType::EXPLOSION},
            {"EX",       SimulationType::EXPLOSION}
    };

    virtual ~InputConfigurationSource() = default;

    /**
     * Returns the minimal characteristic Length for fragments later created by the Breakup Simulation.
     * @return double
     */
    virtual double getMinimalCharacteristicLength() const = 0;

    /**
     * Returns explicitly the type to use for the simulation. The Data Input should than have the corresponding
     * reasonable amount of satellites (EXPLOSION --> 1 satellite, COLLISION --> 2 satellites)
     * @return SimulationType
     */
    virtual SimulationType getTypeOfSimulation() const = 0;

    /**
     * Returns the current Maximal Given (NORAD-Catalog) ID. This is later required for the breakup simulation
     * to determine the IDs of the fragments.
     * @return size_t
     */
    virtual std::optional<size_t> getCurrentMaximalGivenID() const = 0;

    /**
     * Returns an DataSource which has the ability to return an vector of satellites.
     * @return DataSource as shared pointer if it is wished to (re-)use in an object-oriented purpose
     */
    virtual std::shared_ptr<const DataSource> getDataReader() const = 0;

    /**
     * Returns a set of IDs. This set defines which Satellites should be used from the satellite collection.
     * Default implemented: It does not return any Satellites --> no filter
     * @return optional containing a filter-set or not
     */
    virtual std::optional<std::set<size_t>> getIDFilter() const = 0;

    /**
     * Returns a bool which states if the simulation should enforce the mass conservation through the generation
     * of additional fragments if the mass budget was not yet exceeded (after the normal run)
     * @return a bool
     */
    virtual bool getEnforceMassConservation() const = 0;

};
