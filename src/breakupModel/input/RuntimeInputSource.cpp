#include "RuntimeInputSource.h"


double RuntimeInputSource::getMinimalCharacteristicLength() const {
    return _minimalCharacteristicLength;
}

SimulationType RuntimeInputSource::getTypeOfSimulation() const {
    return _simulationType;
}

std::optional<size_t> RuntimeInputSource::getCurrentMaximalGivenID() const {
    return _currentMaximalGivenID;
}

std::shared_ptr<const DataSource> RuntimeInputSource::getDataReader() const {
    return shared_from_this();
}

std::optional<std::set<size_t>> RuntimeInputSource::getIDFilter() const {
    return _idFilter;
}

std::vector<Satellite> RuntimeInputSource::getSatelliteCollection() const {
    return _satellites;
}

bool RuntimeInputSource::getEnforceMassConservation() const {
    return _enforceMassConservation;
}
