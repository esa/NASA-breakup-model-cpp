#include "TLESatcatDataReader.h"

std::vector<Satellite> TLESatcatDataReader::getSatelliteCollection() const {
    std::vector<Satellite> satellites{};
    SatelliteBuilder satelliteBuilder{};

    auto mappingSatcat = this->getSatcatMapping();
    auto mappingTLE = _tleReader.getMappingIDOrbitalElements();

    satellites.reserve(std::max(mappingSatcat.size(), mappingTLE.size()));

    //We just search for satellites which appear in both mappings
    // --> No missing data possible (but not necessarily wrong information
    for (auto const& [id, orbitalElements] : mappingTLE) {
        if (mappingSatcat.count(id) != 0) {
            auto &dataSatcat = mappingSatcat[id];
            satellites.push_back(
                    satelliteBuilder
                            .reset()
                            .setID(id)
                            .setName(std::get<0>(dataSatcat))
                            .setSatType(std::get<1>(dataSatcat))
                            .setMassByArea(std::get<2>(dataSatcat))
                            .setOrbitalElements(orbitalElements)
                            .getResult()
            );
        }
    }
    return satellites;
}

std::map<size_t, std::tuple<std::string, SatType, double>> TLESatcatDataReader::getSatcatMapping() const {
    std::map<size_t, std::tuple<std::string, SatType, double>> mapping{};

    //If the mapping should contain more infos --> Here's the code to change that
    auto satcat = _satcatReader.getLines();
    for (auto const& [name, identifier, id, type, statusCode, owner, launchDate, launchSite, decayDate, period,
                      inclination, apogee, perigee, rcs, dataStatusCode, orbitCenter, OrbitType] : satcat) {

        mapping.insert(std::make_pair(id, std::make_tuple(name, type, rcs)));
    }

    return mapping;
}
