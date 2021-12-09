#pragma once

#include <utility>
#include <array>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include "breakupModel/model/OrbitalElementsFactory.h"

/**
 * Provides the functionality to parse a TLE (Two-Line-Format) with the Alpha-5 scheme.
 * @note The TLE reader ONLY extracts arguments used by the simulation the rest is "thrown away". This behavior can be
 * modified if wished.
 */
class TLEReader {

    const std::string _filepath;

    /**
     * This maps the first char of the ID to the corresponding offset.
     * Notice that letters 'I' and 'O' does not have any mapping
     * @related For further information about the mapping have a look at https://www.space-track.org
     */
    static const std::map<char, size_t> alpha5NumberingSchemeOffset;

public:

    /**
     * Constructs are new TLE Reader.
     * @param filepath
     * @throws an exception if the file does not exists
     */
    explicit TLEReader(std::string filepath)
        : _filepath(std::move(filepath)) {
        if (!std::filesystem::exists(_filepath)) {
            throw std::runtime_error{"The TLE file " + _filepath + " does not exist!"};
        }
    }

    virtual ~TLEReader() = default;

    /**
     * Returns a mapping from satellites ID to their Keplerian Elements as read from the TLE file.
     * The Keplerian Elements are sorted in the order:<br>
     * a (semir-major-axis) , e (eccentricity), i (inclination), W (longitude of the ascending node),
     * w (argument of periapsis), MA (mean Anomaly)
     * @return mapping ID and KeplerElements
     * @throws an exception if the TLE is malformed or any other issues are encountered during the parsing
     */
    std::map<size_t, OrbitalElements> getMappingIDOrbitalElements() const;

private:

    /**
     * Parses the line two of an TLE entry to a pair of ID and Kepler elements.
     * @param line1 - the input stream
     * @return a pair of ID and KeplerElements
     * @throws an exception if the TLE is malformed or any other issues are encountered during the parsing
     */
    std::pair<size_t, OrbitalElements> parseTLELines(const std::string &line1, const std::string &line2) const;

};

