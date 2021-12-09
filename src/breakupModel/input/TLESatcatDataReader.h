#pragma once

#include <string>
#include <map>
#include <tuple>
#include <cmath>
#include "DataSource.h"
#include "CSVReader.h"
#include "TLEReader.h"
#include "breakupModel/model/Satellite.h"
#include "breakupModel/model/SatelliteBuilder.h"

/**
 * Class which reads data from a tle.txt and a satcat.csv
 */
class TLESatcatDataReader : public DataSource {

    /**
     * Delegation to read the satcat.csv
     * Important fields for the Breakup Simulation are:<br>
     * 1-Name, 3-ID, 4-Object-Type, 14-RCS
     * <br><br>
     * Otherwise these are all the fields available (in order of appearance):<br>
     * name, identifier, id, type, statusCode, owner, launchDate, launchSite, decayDate, period,
     * inclination, apogee, perigee, rcs, dataStatusCode, orbitCenter, OrbitType
     */
    CSVReader<std::string, std::string, size_t,
            SatType, std::string, std::string, std::string, std::string, std::string,
            double, double, double, double, double,
            std::string, std::string, std::string> _satcatReader;

    /**
     * Delegation to read the tle.txt
     */
    TLEReader _tleReader;

public:

    /**
     * Creates a new TLE Satcat Data Reader.
     * @param satcatFilename
     * @param telFilename
     * @throws if one file does not exists this constructor will "rethrow" the exception
     */
    TLESatcatDataReader(const std::string &satcatFilename, const std::string &telFilename)
            : _satcatReader{satcatFilename, true},
              _tleReader(telFilename) {}

    /**
     * Creates a new TLE Satcat Data Reader.
     * @param satcatFilename
     * @param telFilename
     * @throws if one file does not exists this constructor will "rethrow" the exception
     */
    TLESatcatDataReader(const CSVReader<std::string, std::string, size_t,
            SatType, std::string, std::string, std::string, std::string, std::string,
            double, double, double, double, double,
            std::string, std::string, std::string> &csvReader, const TLEReader &tleReader)
            : _satcatReader{csvReader},
              _tleReader{tleReader} {}

    /**
     * Returns the a SatelliteCollection by reading the given satcat.csv and TLE data.
     * Neither of the two of them contains all necessary information. So this method also merges the information
     * by using the unique ID of each satellite.
     * @return a Collection of Satellites
     * @throws a runtime_error if satcat or tle is corrupt
     */
    std::vector<Satellite> getSatelliteCollection() const override;

private:

    /**
     * Returns a mapping of satellites ID to its name, type and Radar Cross Section (RCS) in [m^2]
     * @return mapping <ID, infos>
     * @throws a runtime_error if satcat is corrupt
     */
    std::map<size_t, std::tuple<std::string, SatType, double>> getSatcatMapping() const;

};

