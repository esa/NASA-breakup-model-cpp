#pragma once

#include "OutputWriter.h"
#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <utility>
#include <memory>
#include "breakupModel/model/Satellite.h"
#include "breakupModel/util/UtilityContainer.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/fmt/ostr.h"
#include "spdlog/spdlog.h"

/**
 * By using spdlog, this class provides methods to print the result of an Breakup event into an CSV file.
 * @note The used Logger level is "info", so this should be activated at least if output is wished!
 */
class CSVWriter : public OutputWriter {

    /**
    * The logger to write to the file sink
    */
    std::shared_ptr<spdlog::logger> _logger;

    /**
     * States if the output should contain the Orbital Elements
     */
    bool _withKepler;

public:

    /**
     * Creates a new CSVWriter.
     * Results are written to "breakupResult.csv". Kepler elements are deactivated.
     */
    CSVWriter() : CSVWriter("breakupResult.csv") {}

    /**
     * Creates a new CSVWriter.
     * Results are written to filename. Kepler elements are activated depending on the flag.
     * @param filename - a string
     * @param withKepler - if true Kepler elements are printed (default: false)
     */
    explicit CSVWriter(const std::string &filename, bool withKepler = false)
            : _logger{spdlog::basic_logger_mt<spdlog::async_factory>("CSVWriter_" + filename, filename, true)},
              _withKepler{withKepler} {
        _logger->set_pattern("%v");
    }

    /**
     * Creates a new CSVWriter with a custom logger. This constructor is especially useful for testing if no
     * asynchronous properties are wished.
     * Results are written to filename. Kepler elements are activated depending on the flag.
     * @param logger - a shared_ptr to a logger
     * @param withKepler - if true Kepler elements are printed (default: false)
     */
    explicit CSVWriter(std::shared_ptr<spdlog::logger> logger, bool withKepler = false)
            : _logger{std::move(logger)},
              _withKepler{withKepler} {
        _logger->set_pattern("%v");
    }

    /**
     * De-Registers the logger of the CSVWriter, to ensure that a similar CSVWriter can be constructed once again.
     */
    ~CSVWriter() override{
        spdlog::drop(_logger->name());
    }

    void printResult(const std::vector<Satellite> &satelliteCollection) const override;

private:

    /**
     * Prints the vector without Kepler elements.
     * @param satelliteCollection
     */
    void printStandard(const std::vector<Satellite> &satelliteCollection) const;

    /**
     * Prints the vector with Kepler elements.
     * @param satelliteCollection
     */
    void printKepler(const std::vector<Satellite> &satelliteCollection) const;

};

