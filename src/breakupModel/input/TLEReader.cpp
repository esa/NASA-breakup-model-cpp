#include "TLEReader.h"

const std::map<char, size_t> TLEReader::alpha5NumberingSchemeOffset{
        {' ', 0},
        {'0', 0},
        {'1', 10000},
        {'2', 20000},
        {'3', 30000},
        {'4', 40000},
        {'5', 50000},
        {'6', 60000},
        {'7', 70000},
        {'8', 80000},
        {'9', 90000},
        {'A', 100000},
        {'B', 110000},
        {'C', 120000},
        {'D', 130000},
        {'E', 140000},
        {'F', 150000},
        {'G', 160000},
        {'H', 170000},
        {'J', 180000},
        {'K', 190000},
        {'L', 200000},
        {'M', 210000},
        {'N', 220000},
        {'P', 230000},
        {'Q', 240000},
        {'R', 250000},
        {'S', 260000},
        {'T', 270000},
        {'U', 280000},
        {'V', 290000},
        {'W', 300000},
        {'X', 310000},
        {'Y', 320000},
        {'Z', 330000}
};

std::map<size_t, OrbitalElements> TLEReader::getMappingIDOrbitalElements() const {
    std::map<size_t, OrbitalElements> mapping{};

    std::ifstream fileStream{_filepath};
    std::string line;
    std::string line1;
    std::string line2;

    bool line1Found = false;

    while (std::getline(fileStream, line)) {
        if (line.rfind('1', 0) == 0) {
            line1 = line;
            line1Found = true;
        } else if (line.rfind('2', 0) == 0 && line1Found) {
            line2 = line;
            auto pair = parseTLELines(line1, line2);
            mapping.insert(pair);
            line1Found = false;
        }
    }

    return mapping;
}

std::pair<size_t, OrbitalElements> TLEReader::parseTLELines(const std::string &line1, const std::string &line2) const {
    size_t id;
    std::array<double, 6> tleData{};
    int year;
    double fraction;

    try {
        //ID
        char firstCharOfID = line2.at(2);
        std::string restID = line2.substr(3, 4);
        id = std::stoul(restID);
        id += alpha5NumberingSchemeOffset.at(firstCharOfID);
        //Mean Motion [rev/day]
        tleData[0] = std::stod(line2.substr(52, 11));
        //Eccentricity
        tleData[1] = std::stod("0." + line2.substr(26, 7));
        //Inclination [deg]
        tleData[2] = std::stod(line2.substr(8, 8));
        //RAAN [deg]
        tleData[3] = std::stod(line2.substr(17, 8));
        //Argument of Perigee [deg]
        tleData[4] = std::stod(line2.substr(34, 8));
        //Mean Anomaly [deg]
        tleData[5] = std::stod(line2.substr(43, 8));

        //The year
        year = std::stoi(line1.substr(18, 2));
        year = year < 57 ? year + 2000 : year + 1900;
        //The fraction of the year
        fraction = std::stod(line1.substr(20, 8));

    } catch (std::exception &e) {
        throw std::runtime_error{"The TLE file \"" + _filepath +
                                 "\" is malformed! Some Data could not be parsed correctly into valid numbers!\n"
                                 "The issue appeared in the following line:\n"
                                 + line1
        };
    }

    OrbitalElementsFactory factory{};
    Epoch epoch{year, fraction};
    return std::make_pair(id, factory.createFromTLEData(tleData, epoch));
}
