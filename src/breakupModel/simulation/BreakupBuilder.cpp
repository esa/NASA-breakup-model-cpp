#include "BreakupBuilder.h"

BreakupBuilder &BreakupBuilder::reconfigure(const std::shared_ptr<InputConfigurationSource> &configurationSource) {
    _configurationSource = configurationSource;
    this->setMinimalCharacteristicLength(configurationSource->getMinimalCharacteristicLength());
    this->setSimulationType(configurationSource->getTypeOfSimulation());
    this->setCurrentMaximalGivenID(configurationSource->getCurrentMaximalGivenID()),
    this->setIDFilter(configurationSource->getIDFilter());
    this->setEnforceMassConservation(configurationSource->getEnforceMassConservation());
    this->setDataSource(configurationSource->getDataReader());
    return *this;
}

BreakupBuilder &BreakupBuilder::reloadConfigurationSource() {
    this->reconfigure(_configurationSource);
    return *this;
}

BreakupBuilder &BreakupBuilder::setMinimalCharacteristicLength(double minimalCharacteristicLength) {
    _minimalCharacteristicLength = minimalCharacteristicLength;
    return *this;
}

BreakupBuilder &BreakupBuilder::setSimulationType(SimulationType simulationType) {
    _simulationType = simulationType;
    return *this;
}

BreakupBuilder &BreakupBuilder::setCurrentMaximalGivenID(const std::optional<size_t> &currentMaximalGivenID) {
    _currentMaximalGivenID = currentMaximalGivenID;
    return *this;
}

BreakupBuilder &BreakupBuilder::setIDFilter(const std::optional<std::set<size_t>> &idFilter) {
    _idFilter = idFilter;
    return *this;
}

BreakupBuilder &BreakupBuilder::setEnforceMassConservation(bool enforceMassConservation) {
    _enforceMassConservation = enforceMassConservation;
    return *this;
}

BreakupBuilder &BreakupBuilder::setDataSource(const std::vector<Satellite> &satellites) {
    _satellites = satellites;
    return *this;
}

BreakupBuilder &BreakupBuilder::setDataSource(const std::shared_ptr<const DataSource> &dataSource) {
    _satellites = dataSource->getSatelliteCollection();
    return *this;
}

std::unique_ptr<Breakup> BreakupBuilder::getBreakup() const {
    //1. Step: Max ID is derived from all available Satellites, not from only those contained in the filter
    size_t maxID = this->deriveMaximalID();

    //2. Step: Apply the filter
    auto satelliteVector = this->applyFilter();

    //3. Step: Create the Simulation if Type and Input Number of Satellites match together
    //         or try to derive the correct type
    switch (_simulationType) {
        case SimulationType::EXPLOSION:
            if (satelliteVector.size() == 1) {
                return createExplosion(satelliteVector, maxID);
            } else {
                std::stringstream message{};
                message << "No Breakup Simulation was created!\n"
                        << "You defined SimulationType: EXPLOSION\n"
                        << "The input contained after applying the filter " << satelliteVector.size() << " satellites\n"
                        << "But it should contain 1 satellite!";
                throw std::runtime_error{message.str()};
            }
        case SimulationType::COLLISION:
            if (satelliteVector.size() == 2) {
                return createCollision(satelliteVector, maxID);
            } else {
                std::stringstream message{};
                message << "No Breakup Simulation was created!\n"
                        << "You defined SimulationType: COLLISION\n"
                        << "The input contained after applying the filter " << satelliteVector.size() << " satellites\n"
                        << "But it should contain 2 satellites!";
                throw std::runtime_error{message.str()};
            }
        default:
            if (satelliteVector.size() == 1) {
                spdlog::warn("Type was not specified by configuration file, Derived 'Explosion' from 1 satellite!");
                return createExplosion(satelliteVector, maxID);
            } else if (satelliteVector.size() == 2) {
                spdlog::warn("Type was not specified by configuration file, Derived 'Collision' from 2 satellites!");
                return createCollision(satelliteVector, maxID);
            } else {
                throw std::runtime_error{"A breakup simulation could not be created because the type given"
                                         "by the configuration file was different than the number of"
                                         "satellites in the given data input would suggest. Notice:\n"
                                         "Explosion --> 1 satellite\n"
                                         "Collision --> 2 satellites"};
            }
    }
}

std::unique_ptr<Breakup> BreakupBuilder::createExplosion(std::vector<Satellite> &satelliteVector, size_t maxID) const {
    return std::make_unique<Explosion>(satelliteVector, _minimalCharacteristicLength, maxID, _enforceMassConservation);
}

std::unique_ptr<Breakup> BreakupBuilder::createCollision(std::vector<Satellite> &satelliteVector, size_t maxID) const {
    return std::make_unique<Collision>(satelliteVector, _minimalCharacteristicLength, maxID, _enforceMassConservation);
}

std::vector<Satellite> BreakupBuilder::applyFilter() const {
    if (_idFilter.has_value()) {
        std::vector<Satellite> satellitesFiltered{_satellites};
        satellitesFiltered.erase(std::remove_if(satellitesFiltered.begin(), satellitesFiltered.end(),
                                                [&](Satellite &sat) {
                                                    return _idFilter->count(sat.getId()) == 0;
                                                }),
                                 satellitesFiltered.end());
        return satellitesFiltered;
    } else {
        return _satellites;
    }
}

size_t BreakupBuilder::deriveMaximalID() const {
    return _currentMaximalGivenID.value_or(_satellites.empty() ? 0 :
                                           std::max_element(_satellites.begin(), _satellites.end(),
                                                            [](const Satellite &sat1, const Satellite &sat2) {
                                                                return sat1.getId() < sat2.getId();})->getId());
}
