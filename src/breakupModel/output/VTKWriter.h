#pragma once

#include "OutputWriter.h"

#include <string>
#include <functional>
#include <utility>
#include "breakupModel/model/Satellite.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/fmt/ostr.h"
#include "spdlog/spdlog.h"

/**
 * Creates vtk file of the given input.
 * Can be visualized e.g. with ParaView.
 */
class VTKWriter : public OutputWriter {

    /**
     * The logger to write to the file sink
     */
    std::shared_ptr<spdlog::logger> _logger;

public:

    VTKWriter() : VTKWriter("breakupResult.vtu") {}

    /**
     * Creates a new VTKWriter to a specific file.
     * @param filename - std::string
     */
    explicit VTKWriter(const std::string &filename)
            : _logger{spdlog::basic_logger_mt<spdlog::async_factory>("VTKWriter_" + filename, filename, true)} {
        _logger->set_pattern("%v");
    }

    /**
     * Creates a new VTKWriter with a specific logger which is especially useful if no asynchronous logging properties
     * are wished (e. g. for testing purposes).
     * @param logger - a shared_ptr to a logger
     */
    explicit VTKWriter(std::shared_ptr<spdlog::logger> logger)
            : _logger{std::move(logger)} {
        _logger->set_pattern("%v");
    }

    /**
    * De-Registers the logger of the VTKWriter, to ensure that a similar VTKWriter can be constructed once again.
    */
    ~VTKWriter() override{
        spdlog::drop(_logger->name());
    }

    void printResult(const std::vector<Satellite> &satelliteCollection) const override;

private:

    /**
     * Prints a property of the points.
     * @tparam Property - the type of the property, if it is an array only size = 3 is supported!!!
     * @tparam Data - the class which contains this property
     * @param name - name of the property, e.g. mass
     * @param property - the property (normally a getter of an satellite)
     * @param dataCollection - the data
     */
    template<typename Property, typename Data>
    void printProperty(const std::string &name, const std::function<Property(const Data &data)> &property,
                       const std::vector<Data> &dataCollection) const {
        if constexpr (std::is_scalar<Property>::value) {
            _logger->info(R"(        <DataArray Name="{}" NumberOfComponents="1" format="ascii" type="Float32">)",
                          name);
            for (const auto &date : dataCollection) {
                _logger->info("          {}", property(date));
            }
            _logger->info(R"(        </DataArray>)");
        } else if constexpr (util::is_stdarray<Property>::value) {
            _logger->info(R"(        <DataArray Name="{}" NumberOfComponents="3" format="ascii" type="Float32">)", name);
            for (const auto &date : dataCollection) {
                const auto &array = property(date);
                if (std::size(array) == 3) {
                    _logger->info("          {} {} {}", array[0], array[1], array[2]);
                }
            }
            _logger->info(R"(        </DataArray>)");
        } else {

        }
    }

    /**
     * Prints the Header of the VTK file.
     * @param size - the number of points
     */
    void printHeader(size_t size) const;

    /**
     * Prints the separator between point properties and point position in cell.
     */
    void printSeparator() const;

    /**
     * Prints the Footer of the VTK file.
     */
    void printFooter() const;


};
