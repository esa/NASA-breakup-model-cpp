#include "YAMLDataReader.h"

std::vector<Satellite> YAMLDataReader::getSatelliteCollection() const {
    std::vector<Satellite> satelliteVector{};
    SatelliteBuilder satelliteBuilder{};

    if (_file[SATELLITES_TAG] && _file[SATELLITES_TAG].IsSequence()) {
        YAML::Node satellites{_file[SATELLITES_TAG]};
        for (auto satNode : satellites) {
            satelliteVector.push_back(parseSatellite(satelliteBuilder, satNode));
        }
    } else {
        throw std::runtime_error{"The was no satellites tag inside the YAML file, so no satellites were extracted!"};
    }
    return satelliteVector;
}

Satellite YAMLDataReader::parseSatellite(SatelliteBuilder &satelliteBuilder, const YAML::Node &node) {
    satelliteBuilder.reset();
    //Required for TLE
    size_t id = 0;
    if (node[ID_TAG]) {
        id = node[ID_TAG].as<unsigned long>();
        satelliteBuilder.setID(id);
    }
    if (node[NAME_TAG]) {
        satelliteBuilder.setName(node[NAME_TAG].as<std::string>());
    }
    if (node[SATELLITE_TYPE_TAG]) {
        satelliteBuilder.setSatType(node[SATELLITE_TYPE_TAG].as<std::string>());
    }
    if (node[MASS_TAG]) {
        satelliteBuilder.setMass(node[MASS_TAG].as<double>());
    }
    if (node[AREA_TAG]) {
        satelliteBuilder.setMassByArea(node[AREA_TAG].as<double>());
    }
    if (node[VELOCITY_TAG]) {
        satelliteBuilder.setVelocity(node[VELOCITY_TAG].as<std::array<double, 3>>());
    }
    if (node[POSITION_TAG]) {
        satelliteBuilder.setPosition(node[POSITION_TAG].as<std::array<double, 3>>());
    }
    if (node[KEPLER_TAG] && node[KEPLER_TAG].IsMap()) {             //Normal Kepler Definition
        parseKepler(satelliteBuilder, node[KEPLER_TAG]);
    } else if (node[KEPLER_TAG] && node[KEPLER_TAG].IsScalar()) {   //Kepler located in TLE file
        parseKepler(satelliteBuilder, id, node[KEPLER_TAG].as<std::string>());
    }
    return satelliteBuilder.getResult();
}

void YAMLDataReader::parseKepler(SatelliteBuilder &satelliteBuilder, const YAML::Node &node) {
    if (node[SEMI_MAJOR_AXIS_TAG] && node[ECCENTRICITY_TAG] && node[INCLINATION_TAG]
        && node[LONGITUDE_OF_THE_ASCENDING_NODE_TAG] && node[ARGUMENT_OF_PERIAPSIS_TAG]) {
        OrbitalElementsFactory factory{};
        std::array<double, 6> keplerData{};
        keplerData[0] = node[SEMI_MAJOR_AXIS_TAG].as<double>();
        keplerData[1] = node[ECCENTRICITY_TAG].as<double>();
        keplerData[2] = node[INCLINATION_TAG].as<double>();
        keplerData[3] = node[LONGITUDE_OF_THE_ASCENDING_NODE_TAG].as<double>();
        keplerData[4] = node[ARGUMENT_OF_PERIAPSIS_TAG].as<double>();

        if (node[ECCENTRIC_ANOMALY_TAG]) {
            keplerData[5] = node[ECCENTRIC_ANOMALY_TAG].as<double>();
            satelliteBuilder.setOrbitalElements(factory.createFromOnlyRadians(keplerData, OrbitalAnomalyType::ECCENTRIC));
        } else if (node[MEAN_ANOMALY_TAG]) {
            keplerData[5] = node[MEAN_ANOMALY_TAG].as<double>();
            satelliteBuilder.setOrbitalElements(factory.createFromOnlyRadians(keplerData, OrbitalAnomalyType::MEAN));
        } else if (node[TRUE_ANOMALY_TAG]) {
            keplerData[5] = node[TRUE_ANOMALY_TAG].as<double>();
            satelliteBuilder.setOrbitalElements(factory.createFromOnlyRadians(keplerData, OrbitalAnomalyType::TRUE));
        } else {
            throw std::runtime_error{"One satellite input is incomplete! "
                                     "You have to give at least one of the following orbital Anomalies "
                                     "Eccentric Anomaly > Mean Anomaly > True Anomaly [in the order how the "
                                     "program will prioritize an anomaly if multiple are given]"};
        }
    } else {
        throw std::runtime_error{"One satellite input is incomplete! "
                                 "The Keplerian Elements are not fully given!"};
    }
}

void YAMLDataReader::parseKepler(SatelliteBuilder &satelliteBuilder, size_t id, const std::string &tleFilepath) {
    try {
        TLEReader tleReader{tleFilepath};
        auto mapping = tleReader.getMappingIDOrbitalElements();
        satelliteBuilder.setOrbitalElements(mapping.at(id));
    } catch (std::exception &e) {
        std::stringstream message{};
        message << "The TLE file did not contain Kepler elements for the satellite with the ID " << id
                << ". The parsing in the YAML File Reader was therefore not successful!";
        throw std::runtime_error{message.str()};
    }
}
