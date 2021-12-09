#pragma once

#include "OutputWriter.h"
#include <iostream>
#include <string>
#include <sstream>
#include <exception>
#include <utility>
#include <memory>
#include <map>
#include <functional>
#include "breakupModel/model/Satellite.h"
#include "breakupModel/util/UtilityContainer.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/fmt/ostr.h"
#include "spdlog/spdlog.h"

/**
 * The CSVPatternWriter provides methods to print the Satellite catalog in an custom format.
 * It takes a string-pattern which is then used to print specific attributes of the Satellites.
 * @example
 * "IntLRAmjvp" corresponds to ID, name, type, Characteristic Length, Area, mass, ejection vel., velocity, position<br>
 * This would be an analogous to the CSVWriter without Kepler information
 * @example
 * "IntLRAmjvpaeiWwM" corresponds to (same as above) + semi-major-axis, eccentricity, inclination, RAAN, Arg. of. Per.,
 * Mean Anomaly<br>
 * This would be analogous to the CSVWriter with Kepler information
 * @example
 * "L" correspond to Characteristic Length<br>
 * THis would mean, we get a one column CSV with only the Characteristic Length
 */
class CSVPatternWriter : public OutputWriter {

    /**
     * Map which contains all available functions for a specific char. Those functions are usually getter of Satellite.
     * @example for 'A' --> satellite.getArea()
     */
    const static std::map<char, std::function<void(const Satellite &sat, std::stringstream &stream)>> functionMap;

    /**
     * Map which contains all available names for a specific char.
     * @example for 'A' --> "Area [m^2]"
     */
    const static std::map<char, std::string> headerMap;

    /**
     * Contains the functions to execute for each satellite print. We iterate over the whole vector and execute
     * the functions for each satellite.
     */
    std::vector<std::function<void(const Satellite &sat, std::stringstream &stream)>> _myToDo;

    /**
     * Contains the Header information, which is printed by iterating over the whole vector and printing the strings
     * separated with a comma.
     */
    std::vector<std::string> _myHeader;

    /**
     * The logger which is needed to write to a file sink
     */
    std::shared_ptr<spdlog::logger> _logger;

public:

    /**
     * Creates a new CSVPatternWriter to a file.
     * @param filename - string
     * @param pattern - the pattern (have a look at the headerMap, which char has which meaning)
     */
    CSVPatternWriter(const std::string &filename, const std::string &pattern)
            : _logger{spdlog::basic_logger_mt<spdlog::async_factory>("CSVPatternWriter_" + filename, filename, true)},
              _myToDo{} {
        _logger->set_pattern("%v");
        for (char c : pattern) {
            _myToDo.push_back(functionMap.at(c));
            _myHeader.push_back(headerMap.at(c));
        }
    }

    /**
    * Creates a new CSVPatternWriter with a specific logger. This constructor is especially useful for testing if no
    * asynchronous properties are wished.
    * @param logger - a shared_ptr to a logger
    * @param pattern - the pattern (have a look at the headerMap, which char has which meaning)
    */
    CSVPatternWriter(std::shared_ptr<spdlog::logger> logger, const std::string &pattern)
            : _logger{std::move(logger)},
              _myToDo{} {
        _logger->set_pattern("%v");
        for (char c : pattern) {
            _myToDo.push_back(functionMap.at(c));
            _myHeader.push_back(headerMap.at(c));
        }
    }

    /**
    * De-Registers the logger of the CSVPatternWriter, to ensure that a similar CSVPatternWriter
     * can be constructed once again.
    */
    ~CSVPatternWriter() override {
        spdlog::drop(_logger->name());
    }

    void printResult(const std::vector<Satellite> &satelliteCollection) const override;

};
