#include "CSVPatternWriter.h"

const std::map<char, std::function<void(const Satellite &sat,
                                        std::stringstream &stream)>> CSVPatternWriter::functionMap{
        {'I', [](const Satellite &sat, std::stringstream &stream) -> void { stream << sat.getId(); }},
        {'n', [](const Satellite &sat, std::stringstream &stream) -> void { stream << sat.getName(); }},
        {'t', [](const Satellite &sat, std::stringstream &stream) -> void { stream << sat.getSatType(); }},
        {'L', [](const Satellite &sat, std::stringstream &stream) -> void { stream << sat.getCharacteristicLength(); }},
        {'R', [](const Satellite &sat, std::stringstream &stream) -> void { stream << sat.getAreaToMassRatio(); }},
        {'A', [](const Satellite &sat, std::stringstream &stream) -> void { stream << sat.getArea(); }},
        {'m', [](const Satellite &sat, std::stringstream &stream) -> void { stream << sat.getMass(); }},
        {'v', [](const Satellite &sat, std::stringstream &stream) -> void {
            using util::operator<<;
            stream << sat.getVelocity();
        }},
        {'j', [](const Satellite &sat, std::stringstream &stream) -> void {
            using util::operator<<;
            stream << sat.getEjectionVelocity();
        }},
        {'p', [](const Satellite &sat, std::stringstream &stream) -> void {
            using util::operator<<;
            stream << sat.getPosition();
        }},
        {'a', [](const Satellite &sat, std::stringstream &stream) -> void {
            stream << sat.getOrbitalElements().getSemiMajorAxis();
        }},
        {'e', [](const Satellite &sat, std::stringstream &stream) -> void {
            stream << sat.getOrbitalElements().getEccentricity();
        }},
        {'i', [](const Satellite &sat, std::stringstream &stream) -> void {
            stream << sat.getOrbitalElements().getInclination(AngularUnit::RADIAN);
        }},
        {'W', [](const Satellite &sat, std::stringstream &stream) -> void {
            stream << sat.getOrbitalElements().getLongitudeOfTheAscendingNode(AngularUnit::RADIAN);
        }},
        {'w', [](const Satellite &sat, std::stringstream &stream) -> void {
            stream << sat.getOrbitalElements().getArgumentOfPeriapsis(AngularUnit::RADIAN);
        }},
        {'M', [](const Satellite &sat, std::stringstream &stream) -> void {
            stream << sat.getOrbitalElements().getAnomaly(AngularUnit::RADIAN, OrbitalAnomalyType::MEAN);
        }},
        {'E', [](const Satellite &sat, std::stringstream &stream) -> void {
            stream << sat.getOrbitalElements().getAnomaly(AngularUnit::RADIAN, OrbitalAnomalyType::ECCENTRIC);
        }},
        {'T', [](const Satellite &sat, std::stringstream &stream) -> void {
            stream << sat.getOrbitalElements().getAnomaly(AngularUnit::RADIAN, OrbitalAnomalyType::TRUE);
        }}
};

const std::map<char, std::string> CSVPatternWriter::headerMap{
        {'I', "ID"},
        {'n', "Name"},
        {'t', "Satellite Type"},
        {'L', "Characteristic Length [m]"},
        {'R', "A/M [m^2/kg]"},
        {'A', "Area [m^2]"},
        {'m', "Mass [kg]"},
        {'v', "Velocity [m/s]"},
        {'j', "Ejection Velocity [m/s]"},
        {'p', "Position [m]"},
        {'a', "Semi-Major-Axis [m]"},
        {'e', "Eccentricity"},
        {'i', "Inclination [rad]"},
        {'W', "Longitude of the ascending node [rad]"},
        {'w', "Argument of periapsis [rad]"},
        {'M', "Mean Anomaly [rad]"},
        {'E', "Eccentric Anomaly [rad]"},
        {'T', "True Anomaly [rad]"}
};

void CSVPatternWriter::printResult(const std::vector<Satellite> &satelliteCollection) const {
    //Header
    std::stringstream header{};
    for (auto headerIt = _myHeader.begin(); headerIt != _myHeader.end() - 1; ++headerIt) {
        header << *headerIt << ',';
    }
    header << _myHeader.back();
    _logger->info(header.str());

    //CSV Lines
    for (const auto &sat : satelliteCollection) {
        std::stringstream stream{};
        stream.precision(17);
        for (auto funIt = _myToDo.begin(); funIt != _myToDo.end() - 1; ++funIt) {
            (*funIt)(sat, stream);
            stream << ',';
        }
        _myToDo.back()(sat, stream);
        _logger->info(stream.str());
    }
}
