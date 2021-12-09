#pragma once

#include <fstream>
#include <exception>
#include <vector>
#include <array>
#include <utility>
#include <iostream>
#include <sstream>
#include "yaml-cpp/yaml.h"
#include "DataSource.h"
#include "TLEReader.h"
#include "breakupModel/model/Satellite.h"
#include "breakupModel/model/SatelliteBuilder.h"
#include "breakupModel/model/OrbitalElementsFactory.h"

/**
 * Reads Satellites from an YAML file.
 */
class YAMLDataReader : public DataSource {

    /*
    * The following static variables contain the names of the YAML nodes.
    * Note: C++20 would allow constexpr std::string which would be more appropriate instead of char[]
    */
    static constexpr char SATELLITES_TAG[] = "satellites";
    static constexpr char ID_TAG[] = "id";
    static constexpr char NAME_TAG[] = "name";
    static constexpr char SATELLITE_TYPE_TAG[] = "satType";
    static constexpr char MASS_TAG[] = "mass";
    static constexpr char AREA_TAG[] = "area";
    static constexpr char VELOCITY_TAG[] = "velocity";
    static constexpr char POSITION_TAG[] = "position";
    static constexpr char KEPLER_TAG[] = "kepler";
    static constexpr char SEMI_MAJOR_AXIS_TAG[] = "semi-major-axis";
    static constexpr char ECCENTRICITY_TAG[] = "eccentricity";
    static constexpr char INCLINATION_TAG[] = "inclination";
    static constexpr char LONGITUDE_OF_THE_ASCENDING_NODE_TAG[] = "longitude-of-the-ascending-node";
    static constexpr char ARGUMENT_OF_PERIAPSIS_TAG[] = "argument-of-periapsis";
    static constexpr char ECCENTRIC_ANOMALY_TAG[] = "eccentric-anomaly";
    static constexpr char MEAN_ANOMALY_TAG[] = "mean-anomaly";
    static constexpr char TRUE_ANOMALY_TAG[] = "true-anomaly";

    /**
     * The root node of the YAML file
     */
    const YAML::Node _file;

public:

    /**
     * Creates a new YAML Data Reader.
     * @param filename
     * @throws an exception if the file is malformed or cannot be loaded
     */
    explicit YAMLDataReader(const std::string &filename)
            : _file{YAML::LoadFile(filename)} {}

    /**
    * Creates a new YAML Data Reader.
    * @param filename
    * @throws an exception if the file is malformed or cannot be loaded
    */
    explicit YAMLDataReader(std::string &&filename)
            : _file{YAML::LoadFile(filename)} {}

    /**
     * Returns a SatelliteCollection. Satellites are read from the YAML file.<br>
     *
     * If the file name is malformed or the file is corrupt in other ways,
     * the returned SatelliteCollection will contain no satellites.<br>
     *
     * If the file exists, but some satellites are invalid then the SatelliteCollection will only contain valid
     * satellites! A valid satellite is a satellites which has every date needed to run the simulation with it.<br>
     * @return a SatelliteCollection
     * @throws a runtime_error if a satellite is incomplete
     */
    std::vector<Satellite> getSatelliteCollection() const override;

private:

    /**
     * Parses a satellite from a given YAML Node.
     * @param satelliteBuilder - an reference to an SatelliteBuilder
     * @param node - an reference to an YAML Node
     * @return a new Satellite
     * @throws an runtime_error exception if the satellite is invalid
     */
    static Satellite parseSatellite(SatelliteBuilder &satelliteBuilder, const YAML::Node &node);

    /**
     * Parses the Keplerian Elements from a YAML Node.
     * @param satelliteBuilder - an reference to an SatelliteBuilder
     * @param node - an reference to an YAML Node, containing the Keplerian Elements
     * @note This method is used by the parseSatellite(SatelliteBuilder&, YAML::Node&) method
     * @throws a runtime_error if Kepler elements not fully given
     */
    static void parseKepler(SatelliteBuilder &satelliteBuilder, const YAML::Node &node);

    /**
     * Parses the Kepler Elements from a TLE file.
     * @param satelliteBuilder - an reference to an SatelliteBuilder
     * @param id - the id of the satellite which to find in the TLE file
     * @param tleFilepath - the path to the TLE file
     * @throws a runtime_error if the TLE does not contain the satellite
     */
    static void parseKepler(SatelliteBuilder &satelliteBuilder, size_t id, const std::string &tleFilepath);

};


