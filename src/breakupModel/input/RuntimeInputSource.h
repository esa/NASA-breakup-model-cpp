#pragma once

#include <utility>

#include "InputConfigurationSource.h"
#include "DataSource.h"

/**
 * A object which defines all parameters needed for the Simulation.
 */
class RuntimeInputSource
        : public InputConfigurationSource, public DataSource, public std::enable_shared_from_this<const DataSource> {

    const double _minimalCharacteristicLength;

    const SimulationType _simulationType{SimulationType::UNKNOWN};

    const std::optional<size_t> _currentMaximalGivenID{std::nullopt};

    const std::optional<std::set<size_t>> _idFilter{std::nullopt};

    const bool _enforceMassConservation{false};

    const std::vector<Satellite> _satellites;

public:

    /**
     * Constructs a new Runtime Source with the minimal requirements needed for the Breakup Builder.
     * @param minimalCharacteristicLength - double
     * @param satellites - satellite vector
     */
    RuntimeInputSource(double minimalCharacteristicLength, std::vector<Satellite> satellites)
            : _minimalCharacteristicLength{minimalCharacteristicLength},
              _satellites{std::move(satellites)} {}


    /**
     * Constructs a new Runtime Source with all parameters available.
     * @param minimalCharacteristicLength - double
     * @param satellites - satellite vector
     * @param simulationType - type of simulation --> strong definition, error handling possible
     * @param currentMaximalGivenId - maximal given NORAD Catalog ID
     * @param idFilter - filter which satellites to use
     * @param enforceMassConservation - should the input mass always equals the output mass (default: false)
     */
    RuntimeInputSource(double minimalCharacteristicLength, std::vector<Satellite> satellites,
                       SimulationType simulationType, size_t currentMaximalGivenId,
                       std::optional<std::set<size_t>> idFilter, bool enforceMassConservation = false)
            : _minimalCharacteristicLength{minimalCharacteristicLength},
              _satellites{std::move(satellites)},
              _simulationType{simulationType},
              _currentMaximalGivenID{std::make_optional(currentMaximalGivenId)},
              _idFilter{std::move(idFilter)},
              _enforceMassConservation{enforceMassConservation} {}


    /**
    * Constructs a new Runtime Source with all parameters available.
    * @param minimalCharacteristicLength - double
    * @param satellites - satellite vector
    * @param simulationType - type of simulation --> strong definition, error handling possible
    * @param currentMaximalGivenId - maximal given NORAD Catalog ID
    * @param idFilter - filter which satellites to use
    * @param enforceMassConservation - should the input mass always equals the output mass (default: false)
    */
    RuntimeInputSource(double minimalCharacteristicLength, std::vector<Satellite> satellites,
                       SimulationType simulationType, std::optional<size_t> currentMaximalGivenId,
                       std::optional<std::set<size_t>> idFilter, bool enforceMassConservation = false)
            : _minimalCharacteristicLength{minimalCharacteristicLength},
              _satellites{std::move(satellites)},
              _simulationType{simulationType},
              _currentMaximalGivenID{currentMaximalGivenId},
              _idFilter{std::move(idFilter)},
              _enforceMassConservation{enforceMassConservation} {}


    /**
     * Constructs a new Runtime Source with all parameters available.
     * Here you give the RuntimeInputSource another DataSource like an TLESatcatReader or a YAMLDataReader to configure
     * settings via this object but with advantage of file input.
     * @param minimalCharacteristicLength - double
     * @param dataSource - a DataSource like TLESatcatReader or YAMLDataReader
     * @param simulationType - type of simulation --> strong definition, error handling possible
     * @param currentMaximalGivenId - maximal given NORAD Catalog ID
     * @param idFilter - filter which satellites to use
     * @param enforceMassConservation - should the input mass always equals the output mass (default: false)
     */
    RuntimeInputSource(double minimalCharacteristicLength, const std::shared_ptr<const DataSource> &dataSource,
                       SimulationType simulationType, size_t currentMaximalGivenId,
                       std::optional<std::set<size_t>> idFilter, bool enforceMassConservation = false)
            : _minimalCharacteristicLength{minimalCharacteristicLength},
              _satellites{dataSource->getSatelliteCollection()},
              _simulationType{simulationType},
              _currentMaximalGivenID{std::make_optional(currentMaximalGivenId)},
              _idFilter{std::move(idFilter)},
              _enforceMassConservation{enforceMassConservation} {}


    double getMinimalCharacteristicLength() const final;

    SimulationType getTypeOfSimulation() const final;

    std::optional<size_t> getCurrentMaximalGivenID() const final;

    std::shared_ptr<const DataSource> getDataReader() const final;

    std::optional<std::set<size_t>> getIDFilter() const final;

    std::vector<Satellite> getSatelliteCollection() const final;

    bool getEnforceMassConservation() const final;
};