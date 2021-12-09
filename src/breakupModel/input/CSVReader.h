#pragma once

#include <utility>
#include <array>
#include <fstream>
#include <string>
#include <sstream>
#include <exception>
#include <tuple>
#include <filesystem>
#include "breakupModel/model/Satellite.h"

/**
 * Provides the functionality to read an CSV file into an container of tuples.
 * Every Type argument represents the type of a column. If the file has an header and the corresponding argument was set
 * to true in the constructor, the header can be read as strings by using getHeader().
 *
 * @example CSVReader{int, std::string, double} reads in rows of the kind "1234,Hello World,3.33"
 *
 * @note If you define the wrong types, the CSVReader will work nevertheless or not depending on the definition of the
 * operator >> for a given type. E. g. if you define double as type, but in the the CSV is written "word" in the
 * corresponding cell. The operator >> for double reads in zero. A reverse example would be defined type string, but
 * in the cell is written "12.22", then "12.22" will be the result. The CSVReader has no clue if the data is correct,
 * the checking is the obligation of the User.<br><br>
 * Other example, where error handling is implemented: If you read in a SatType, the operator >> for SatType will throw
 * an exception if no mapping can be found. Nevertheless this is NOT implemented here in the CSVReader whereas it is
 * implemented by the "user".
 */
template<typename ...T>
class CSVReader {

    const std::string _filepath;

    bool _hasHeader;

public:

    /**
     * Constructs a new CSV Reader.
     * @param filepath
     * @param hasHeader
     * @throws an exception if the file does not exists
     */
    CSVReader(std::string filepath, bool hasHeader)
            : _filepath(std::move(filepath)),
              _hasHeader(hasHeader) {
        if (!std::filesystem::exists(_filepath)) {
            throw std::runtime_error{"The CSV file " + _filepath + " does not exist!"};
        }
    }

    ~CSVReader() = default;


private:

    /**
     * Parses one cell of the CSV line file stream to a type V by using its >> operator.
     * The getLine overhead fixes the problematic of ',' consumption.
     * @tparam V - the value to be extracted, should have an >> operator overload
     * @param istream - the line stream
     * @param value - the value extracted (non-const)
     * @return true to not cause any issues in getTuple
     */
    template<typename V>
    bool parseCell(std::istream &istream, V &value) const {
        std::string cell;
        std::getline(istream, cell, ',');
        std::stringstream cellStream {cell};
        cellStream.unsetf(std::ios_base::skipws);
        cellStream >> value;
        return true;
    }

    /**
    * Parses one cell of the CSV line file stream to a type string. Fixes the problematic of whitespace
    * consumption with the default >> operator for strings.
    * @param istream - the line stream
    * @param value - the value extracted (non-const)
    * @return true to not cause any issues in getTuple
    */
    bool parseCell(std::istream &istream, std::string &value) const {
        std::string cell;
        std::getline(istream, cell, ',');
        value = cell;
        return true;
    }

    /**
     * Creates the tuple for a given lineStream.
     * @tparam Tuple - the tuple to be filled
     * @tparam I - index sequence (packed as long as parameter list T)
     * @param lineStream - a CSV line/ row
     * @param tuple the tuple to be filled
     * @related For further information about this Idea. Code is adapted from here
     * https://stackoverflow.com/questions/34314806/parsing-a-c-string-into-a-tuple [accessed 29.06.2021]
     */
    template<typename Tuple, typename std::size_t... I>
    void getTuple(std::istream &lineStream, Tuple &tuple, std::index_sequence<I...>) const {
      (parseCell(lineStream, std::get<I>(tuple)), ...);
    }

    /**
     * Returns the next line of the CSV file as a tuple
     * @tparam Tuple - the tuple to be filled
     * @param fileStream - the CSV input stream
     * @param tuple - the tuple to be filled
     * @return success of parsing a new element
     */
    template<typename Tuple>
    bool nextLine(std::istream &fileStream, Tuple &tuple) const {
        std::string line;
        std::getline(fileStream, line);
        if (!line.empty()) {
            std::stringstream lineStream{line};
            this->getTuple(lineStream, tuple, std::make_index_sequence<sizeof...(T)>{});
            return true;
        }
        return false;
    }

public:

    /**
     * Returns the lines of the CSV file in a vector. Each line is tokenized into an tuple with the corresponding types.
     * @return vector of tokenized lines
     * @throws an exception if issues are encountered during parsing
     */
    std::vector<std::tuple<T...>> getLines() const {
        //Open fileStream stream
        std::ifstream fileStream{_filepath};
        std::vector<std::tuple<T...>> lines{};
        std::tuple<T...> t;

        //Skip header if the fileStream has a header
        if (_hasHeader) {
            std::string line;
            std::getline(fileStream, line);
        }

        //Read row by row
        while (this->nextLine(fileStream, t)) {
            lines.push_back(t);
        }

        return lines;
    }

    /**
     * Returns the header of the CSV file as a vector of strings.
     * @return vector of strings
     * @throws an exception if the CSV file does not have an header
     */
    std::array<std::string, sizeof...(T)> getHeader() const {
        if (_hasHeader) {
            std::array<std::string, sizeof...(T)> header{};
            std::ifstream fileStream{_filepath};
            std::string line;
            std::getline(fileStream, line);
            std::stringstream lineStream{line};
            std::string cell;

            auto headerIt = header.begin();
            while (std::getline(lineStream, cell, ',')) {
                *headerIt = cell;
                ++headerIt;
            }
            return header;
        } else {
            throw std::runtime_error{"The CSVReader was configured that this CSV file has no header."
                                     "Nevertheless getHeader() was called, which is wrong!"};
        }
    }
};
