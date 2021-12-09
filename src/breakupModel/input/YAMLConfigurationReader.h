#pragma once

#include <utility>
#include <exception>
#include "InputConfigurationSource.h"
#include "OutputConfigurationSource.h"
#include "YAMLDataReader.h"
#include "TLESatcatDataReader.h"
#include "breakupModel/output/CSVWriter.h"
#include "breakupModel/output/CSVPatternWriter.h"
#include "breakupModel/output/VTKWriter.h"
#include "spdlog/spdlog.h"

/**
 * Reads the breakup simulation configuration data from an YAML file.
 */
class YAMLConfigurationReader : public InputConfigurationSource, public OutputConfigurationSource {

    /*
     * The following static variables contain the names of the YAML nodes.
     * Note: C++20 would allow constexpr std::string which would be more appropriate instead of char[]
     */
    static constexpr char SIMULATION_TAG[] = "simulation";
    static constexpr char MIN_CHAR_LENGTH_TAG[] = "minimalCharacteristicLength";
    static constexpr char SIMULATION_TYPE_TAG[] = "simulationType";
    static constexpr char CURRENT_MAX_ID_TAG[] = "currentMaxID";
    static constexpr char INPUT_SOURCE_TAG[] = "inputSource";
    static constexpr char ID_FILTER_TAG[] = "idFilter";
    static constexpr char ENFORCE_MASS_CONSERVATION_TAG[] = "enforceMassConservation";
    static constexpr char RESULT_OUTPUT_TAG[] = "resultOutput";
    static constexpr char INPUT_OUTPUT_TAG[] = "inputOutput";
    static constexpr char TARGET_TAG[] = "target";
    static constexpr char KEPLER_TAG[] = "kepler";
    static constexpr char CSV_PATTERN_TAG[] = "csvPattern";

    const YAML::Node _file;

public:

    /**
     * Creates a new YAML Configuration Reader.
     * @param filename
     * @throws an exception if the file is malformed or cannot be loaded or if no SIMULATION_TAG is found
     */
    explicit YAMLConfigurationReader(const std::string &filename)
        : _file{YAML::LoadFile(filename)} {
        if (!_file[SIMULATION_TAG]) {
            throw std::runtime_error{"No tag found the YAML-Config file which specifies the simulation!"};
        }
    }

    /**
     * Creates a new YAML Configuration Reader.
     * @param filename
     * @throws an exception if the file is malformed or cannot be loaded or if no SIMULATION_TAG is found
     */
    explicit YAMLConfigurationReader(std::string &&filename)
            : _file{YAML::LoadFile(filename)} {
        if (!_file[SIMULATION_TAG]) {
            throw std::runtime_error{"No tag found the YAML-Config file which specifies the simulation!"};
        }
    }


    /**
     * Returns the minimal characteristic Length for fragments later created by the Breakup Simulation.
     * @return double
     * @throws a runtime_error if not specified
     */
    double getMinimalCharacteristicLength() const override;

    /**
     * Returns explicitly the type to use for the simulation. The Data Input should than have the corresponding
     * reasonable amount of satellites (EXPLOSION --> 1 satellite, COLLISION --> 2 satellites).
     * @attention If not given UNKNOWN is returned and the type will be derived from the number of satellites
     * @return SimulationType
     */
    SimulationType getTypeOfSimulation() const override;

    /**
    * Returns the current Maximal Given (NORAD-Catalog) ID. This is later required for the breakup simulation
    * to determine the IDs of the fragments. The nullopt is returned if this is not given.
    * @return std::optional<size_t>
    */
    std::optional<size_t> getCurrentMaximalGivenID() const override;

    /**
     * Returns an DataSource which has the ability to return an vector of satellites.
     * @return DataSource as shared pointer if it is wished to (re-)use in an object-oriented purpose
     * @throws an Exception if not given because not data == no simulation possible
     */
    std::shared_ptr<const DataSource> getDataReader() const override;

    /**
     * Returns the ID selection for the filter if given. Else an empty optional is returned.
     * @return filter or empty
     */
    std::optional<std::set<size_t>> getIDFilter() const override;

    /**
     * Returns a bool which states if the simulation should enforce the mass conservation through the generation
     * of additional fragments if the mass budget was not yet exceeded (after the normal run)
     * @return true or false if a value is given for this TAG in the YAML file, otherwise always false
     */
    bool getEnforceMassConservation() const override;

    /**
     * Reads in which Output is wished by the YAML file.
     * @return a vector containing the Outputs according to the YAML file
     */
    std::vector<std::shared_ptr<OutputWriter>> getOutputTargets() const override;

    /**
     * Reads in which Output is wished for the Input Satellites.
     * @return a vector containing the Outputs according to the YAML file
     */
    std::vector<std::shared_ptr<OutputWriter>> getInputTargets() const override;

private:

    /**
     * Internally used by getOutputTargets() and getInputTargets() to read in the YAML output specification.
     * @param node - the YAML Node RESULT_OUTPUT_TAG or either INPUT_OUTPUT_TAG
     * @return a vector containing the OutputWriter according to the YAML file
     */
    static std::vector<std::shared_ptr<OutputWriter>> extractOutputWriter(const YAML::Node &node) ;
};

