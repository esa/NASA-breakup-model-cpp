#pragma once

#include <memory>
#include <exception>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "breakupModel/input/InputConfigurationSource.h"
#include "breakupModel/input/DataSource.h"
#include "Breakup.h"
#include "Explosion.h"
#include "Collision.h"
#include "spdlog/spdlog.h"

/**
 * Interface to easily create a Breakup Simulation.
 */
class BreakupBuilder {

    std::shared_ptr<InputConfigurationSource> _configurationSource;

    double _minimalCharacteristicLength;

    SimulationType _simulationType;

    std::optional<size_t> _currentMaximalGivenID;

    std::optional<std::set<size_t>> _idFilter;

    std::vector<Satellite> _satellites;

    bool _enforceMassConservation;

public:

    explicit BreakupBuilder(const std::shared_ptr<InputConfigurationSource> &configurationSource)
            : _configurationSource{configurationSource},
              _minimalCharacteristicLength{configurationSource->getMinimalCharacteristicLength()},
              _simulationType{configurationSource->getTypeOfSimulation()},
              _currentMaximalGivenID{configurationSource->getCurrentMaximalGivenID()},
              _idFilter{configurationSource->getIDFilter()},
              _enforceMassConservation{configurationSource->getEnforceMassConservation()},
              _satellites{configurationSource->getDataReader()->getSatelliteCollection()} {}

    /**
     * Adds an input source for the satellites.
     * @param configurationReader - a shared pointer to an input source
     * @return this
     */
    BreakupBuilder &reconfigure(const std::shared_ptr<InputConfigurationSource> &configurationReader);

    /**
     * Reloads the InputConfigurationSource. This can mean e. g. to re-read the underlying file or similar actions.
     * This method is then useful if you have previously overridden some settings by the "set"-Methods.
     * @return this
     */
    BreakupBuilder &reloadConfigurationSource();

    /**
     * Overrides/ Re-Sets the Minimal Characteristic Length L_c in [m] to a specific value.
     * @param minimalCharacteristicLength in [m] as double
     * @return this
     */
    BreakupBuilder &setMinimalCharacteristicLength(double minimalCharacteristicLength);

    /**
     * Overrides/ Re-Sets the Simulation Type to a specific value.
     * @param simulationType - Explosion, Collision, Unknown
     * @return this
     */
    BreakupBuilder &setSimulationType(SimulationType simulationType);

    /**
     * Overrides/ Re-Sets the currentMaximalGivenID (e.g. maximal given NORAD Catalog ID) to a specific value.
     * If the nullopt is chosen, the maximal ID is derived from the input satellites.
     * @param currentMaximalGivenID - std::optional<size_t>
     * @return this
     */
    BreakupBuilder &setCurrentMaximalGivenID(const std::optional<size_t> &currentMaximalGivenID);

    /**
     * Overrides/ Re-Sets the ID Filter and sets it to a new set.
     * @param idFilter - contains the IDs of satellites which should be used, the rest is discarded
     * @return this
     */
    BreakupBuilder &setIDFilter(const std::optional<std::set<size_t>> &idFilter);

    /**
     * Overrides/ Re-Sets the value of enforceMassConservation to the given value.
     * @param enforceMassConservation - new Value
     * @return this
     */
    BreakupBuilder &setEnforceMassConservation(bool enforceMassConservation);

    /**
     * Overrides/ Re-Sets the Data Source to a specific Satellite vector
     * @param satellites - vector of satellites
     * @return this
     */
    BreakupBuilder &setDataSource(const std::vector<Satellite> &satellites);

    /**
     * Overrides/ Re-Sets the Data Source and calls the method getSatelliteCollection on it to set the internal
     * satellites member.
     * @param dataSource - a pointer to an DataSource
     * @return this
     */
    BreakupBuilder &setDataSource(const std::shared_ptr<const DataSource> &dataSource);

    /**
     * Creates a new Breakup Simulation with the given input.
     * Can either be a Collision or an Explosion depending on the satellite number in the SatelliteCollection.
     * Three types of Input Specification are imaginable:<br>
     * STRONG --> Specified type in config file & Satellite number are harmonic<br>
     * WEAK   --> No specified input type, but Satellite number suggests a type (error message, but simulation continues)<br>
     * NONE   --> No input type given, type cannot be derived from satellite number (throws exception)<br>
     * @return Breakup Simulation
     * @throws a runtime_error if type is not determined
     */
    std::unique_ptr<Breakup> getBreakup() const;


private:

    /**
     * Creates a Explosion Simulation.
     * @param satelliteVector - std::vector<Satellite>
     * @return a Explosion
     */
    std::unique_ptr<Breakup> createExplosion(std::vector<Satellite> &satelliteVector, size_t maxID) const;

    /**
     * Creates a Collision Simulation.
     * @param satelliteVector - std::vector<Satellite>
     * @return a Collision
     */
    std::unique_ptr<Breakup> createCollision(std::vector<Satellite> &satelliteVector, size_t maxID) const;

    /**
     * Returns an vector containing only the satellites given in the filterSet.
     * @return a modified satellite vector
     */
    [[nodiscard]] std::vector<Satellite> applyFilter() const;

    /**
     * Returns either the the maximalGivenID if this value is given via input or it derives this value from the input
     * satellites. In case the input satellite vector is empty, this function will return zero.
     * @return size_t - maxID
     */
    [[nodiscard]] size_t deriveMaximalID() const;
};
