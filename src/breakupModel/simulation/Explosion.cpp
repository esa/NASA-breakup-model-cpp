#include "Explosion.h"
#include "Breakup.h"

void Explosion::init() {
    Breakup::init();
    //The pdf for Explosions is: 0.0132578/x^2.6
    _lcPowerLawExponent = -2.6;
    //Equation 11 mu = 0.2 * chi + 1.85
    _deltaVelocityFactorOffset = std::make_pair(0.2, 1.85);
}

void Explosion::calculateFragmentCount() {
    //Gets the one satellite from the input
    Satellite &sat = _input.at(0);

    //Sets the maximalCharacteristicLength which will be required later
    _maximalCharacteristicLength = sat.getCharacteristicLength();

    //Sets the satType attribute to the correct type (later required for the A/M)
    //The Default of this member is SPACECRAFT
    _satType = sat.getSatType();

    //Sets the _input mass which will be required later for mass conservation purpose
    _inputMass = sat.getMass();

    //The fragment Count, respectively Equation 2
    auto fragmentCount = static_cast<size_t>(6.0 * std::pow(_minimalCharacteristicLength, -1.6));
    this->generateFragments(fragmentCount, sat.getPosition());
}

void Explosion::assignParentProperties() {
    //The name of the fragments
    const Satellite &parent = _input.at(0);
    auto debrisNamePtr = std::make_shared<const std::string>(parent.getName() + "-Explosion-Fragment");

    auto tupleView = _output.getVNTuple();
    std::for_each(std::execution::par, tupleView.begin(), tupleView.end(),
                  [&](auto &tuple) {
        //Order in the tuple: 0: Velocity | 1: NamePtr
        auto &[velocity, name] = tuple;
        velocity = parent.getVelocity();
        name = debrisNamePtr;
    });
}
