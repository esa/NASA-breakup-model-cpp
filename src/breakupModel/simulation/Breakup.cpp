#include "Breakup.h"

void Breakup::run() {
    //0. Step: Prepare constants, etc.
    this->init();

    //1. Step: Generate the new Satellites
    this->calculateFragmentCount();

    //2. Step: Assign every new Satellite a value for L_c
    this->characteristicLengthDistribution();

    //3. Step: Calculate the A/M (area-to-mass-ratio), A (area) and M (mass) values for every Satellite
    this->areaToMassRatioDistribution();

    //4. Step: Enforce the Mass Conservation and remove (or add) fragments
    this->enforceMassConservation();

    //5. Step: Assign parent and by doing that assign each fragment a base velocity
    this->assignParentProperties();

    //6. Step: Calculate the Ejection velocity for every Satellite
    this->deltaVelocityDistribution();

    //7. Step: As a last step set the _currentMaxGivenID to the new valid value
    _currentMaxGivenID += _output.size();
}

Breakup &Breakup::setSeed(std::optional<unsigned long> seed) {
    if (seed.has_value()) {
        _fixRNG = std::mt19937 {seed.value()};
    } else {
        _fixRNG = std::nullopt;
    }
    return *this;
}

void Breakup::init() {
    _inputMass = 0;
    _outputMass = 0;
}

void Breakup::generateFragments(size_t fragmentCount, const std::array<double, 3> &position) {
    _output = Satellites{_currentMaxGivenID+1, SatType::DEBRIS, position, fragmentCount};
}

void Breakup::characteristicLengthDistribution() {
    std::for_each(std::execution::par_unseq, _output.characteristicLength.begin(), _output.characteristicLength.end(),
                  [&](double &lc) {
        lc = calculateCharacteristicLength();
    });
}

void Breakup::areaToMassRatioDistribution() {
    auto tupleView = _output.getAreaMassTuple();
    std::for_each(std::execution::par_unseq, tupleView.begin(), tupleView.end(),
                  [&](auto &tuple) {
        //Order in the tuple: 0: L_c | 1: A/M | 2: Area | 3: Mass
        auto &[lc, areaToMassRatio, area, mass] = tuple;
        //Calculate the A/M value in [m^2/kg]
        areaToMassRatio = calculateAreaMassRatio(lc);
        //Calculate the area A in [m^2]
        area = calculateArea(lc);
        //Calculate the mass m in [kg]
        mass = calculateMass(area, areaToMassRatio);
    });
}

void Breakup::enforceMassConservation() {
    //Enforce Mass Conservation if the output mass is greater than the input mass
    _outputMass = std::reduce(std::execution::par_unseq,_output.mass.begin(), _output.mass.end(), 0.0);
    spdlog::debug("The simulation got {} kg of input mass for fragments", _inputMass);
    spdlog::debug("The simulation produced {} kg of debris", _outputMass);
    size_t oldSize = _output.size();
    size_t newSize = _output.size();
    // Shrink and Remove Mass Excess
    while (_outputMass > _inputMass) {
        _outputMass -= _output.mass.back();
        newSize -= 1;
        _output.mass.pop_back();
    }
    _output.resize(newSize);

    // Add new Fragments to better fulfill the Mass Budget, if mass excess was not already removed
    if (_enforceMassConservation && newSize == oldSize) {
        this->addFurtherFragments();
        newSize = _output.size();
    }
    // Some helpful logging hints
    if (oldSize != newSize) {
        spdlog::warn("The simulation modified the number of fragments to enforce the mass conservation.");
        spdlog::warn("The fragment count was adapted from {} to {} fragments.", oldSize, newSize);
        spdlog::debug("The simulation corrected to {} kg of debris", _outputMass);
    }
}

void Breakup::addFurtherFragments() {
    while (_outputMass < _inputMass) {
        //Order in the tuple: 0: L_c | 1: A/M | 2: Area | 3: Mass
        //Create new element and assign values
        auto tuple = _output.appendElement();
        auto &[lc, areaToMassRatio, area, mass] = tuple;
        lc = calculateCharacteristicLength();
        areaToMassRatio = calculateAreaMassRatio(lc);
        area = calculateArea(lc);
        mass = calculateMass(area, areaToMassRatio);

        //Calculate new mass
        _outputMass += mass;
    }
    //Remove the element which has lead to the exceeding of the mass budget
    _outputMass -= _output.mass.back();
    _output.popBack();
}

void Breakup::deltaVelocityDistribution() {
    using namespace util;
    auto tupleView = _output.getVelocityTuple();
    std::for_each(std::execution::par_unseq, tupleView.begin(), tupleView.end(),
                  [&](auto &tuple) {
        //Order in the tuple: 0: A/M | 1: Velocity | 2: Ejection Velocity
        auto &[areaToMassRatio, velocity, ejectionVelocity] = tuple;
        //Calculates the velocity as a scalar based on Equation 11/ 12
        const double chi = log10(areaToMassRatio);
        const double mu = _deltaVelocityFactorOffset.first * chi + _deltaVelocityFactorOffset.second;
        constexpr double sigma = 0.4;
        std::normal_distribution<> normalDistribution{mu, sigma};
        double velocityScalar = std::pow(10.0, getRandomNumber(normalDistribution));

        //Transform the scalar velocity into a cartesian vector
        ejectionVelocity = calculateVelocityVector(velocityScalar);
        velocity = velocity + ejectionVelocity;
    });
}

double Breakup::calculateCharacteristicLength() {
    using util::transformUniformToPowerLaw;
    static std::uniform_real_distribution<> uniformRealDistribution{0.0, 1.0};
    const double y = getRandomNumber(uniformRealDistribution);
    return transformUniformToPowerLaw(_minimalCharacteristicLength, _maximalCharacteristicLength, _lcPowerLawExponent, y);
}

double Breakup::calculateAreaMassRatio(double characteristicLength) {
    using namespace util;
    const double logLc = std::log10(characteristicLength);

    if (characteristicLength > 0.11) {
        //Case bigger than 11 cm
        std::normal_distribution<> n1{mu_1(_satType, logLc), sigma_1(_satType, logLc)};
        std::normal_distribution<> n2{mu_2(_satType, logLc), sigma_2(_satType, logLc)};

        return std::pow(10.0, alpha(_satType, logLc) * getRandomNumber(n1) +
            (1 - alpha(_satType, logLc)) * getRandomNumber(n2));
    } else if (characteristicLength < 0.08) {
        //Case smaller than 8 cm
        std::normal_distribution<> n{mu_soc(logLc), sigma_soc(logLc)};

        return std::pow(10.0, getRandomNumber(n));
    } else {
        //Case between 8 cm and 11 cm
        std::normal_distribution<> n1{mu_1(_satType, logLc), sigma_1(_satType, logLc)};
        std::normal_distribution<> n2{mu_2(_satType, logLc), sigma_2(_satType, logLc)};
        std::normal_distribution<> n{mu_soc(logLc), sigma_soc(logLc)};

        double y1 = std::pow(10.0, alpha(_satType, logLc) * getRandomNumber(n1) +
                                 (1.0 - alpha(_satType, logLc)) * getRandomNumber(n2));
        double y0 = std::pow(10.0, getRandomNumber(n));

        //beta * y1 + (1 - beta) * y0 = beta * y1 + y0 - beta * y0 = y0 + beta * (y1 - y0)
        return y0 + (characteristicLength - 0.08) * (y1 - y0) / (0.03);
    }
}

double Breakup::calculateArea(double characteristicLength) {
    constexpr double lcBound = 0.00167;
    if (characteristicLength < lcBound) {
        constexpr double factorLittle = 0.540424;
        return factorLittle * characteristicLength * characteristicLength;
    } else {
        constexpr double exponentBig = 2.0047077;
        constexpr double factorBig = 0.556945;
        return factorBig * std::pow(characteristicLength, exponentBig);
    }
}

double Breakup::calculateMass(double area, double areaMassRatio) {
    return area / areaMassRatio;
}

std::array<double, 3> Breakup::calculateVelocityVector(double velocity) {
    std::uniform_real_distribution<> uniformRealDistribution{0.0, 1.0};

    double u = getRandomNumber(uniformRealDistribution) * 2.0 - 1.0;
    double theta = getRandomNumber(uniformRealDistribution) * 2.0 * util::PI;
    double v = std::sqrt(1.0 - u * u);

    return std::array<double, 3>
            {{v * std::cos(theta) * velocity, v * std::sin(theta) * velocity, u * velocity}};
}
