#pragma once

#include <utility>
#include <vector>
#include <array>
#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>
#include <memory>
#include <execution>
#include <optional>
#include <mutex>
#include "breakupModel/model/Satellite.h"
#include "breakupModel/model/Satellites.h"
#include "breakupModel/util/UtilityContainer.h"
#include "breakupModel/util/UtilityFunctions.h"
#include "breakupModel/util/UtilityAreaMassRatio.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

/**
 * Pure virtual class which needs a Collection of Satellites as input and output and simulates a breakup
 * which is either a collision or an explosion.
 */
class Breakup {

protected:

    /**
     * The minimal characteristic length in [m]
     * The Breakup Simulation will only produce fragments greater or equal this fragmentCount.
     */
    const double _minimalCharacteristicLength{0.05};

    /**
     * The maximal characteristic length in [m]
     * The value is set during the run of the simulation and derived
     * from the input satellites (the max L_c of them).
     */
    double _maximalCharacteristicLength{};

    /**
     * The Breakup simulation will assign each satellite, created after setting this, an ID greater than this number.
     * ID will be unique if _currentMaxGivenID is set to the current maximal given NORAD-Catalog ID.
     * @note Default value is zero which could led to confusion because those numbers directly after zero are already
     * assigned to existing satellites. Nevertheless the simulation never uses those IDs, so this value is only a
     * MUST if the users wants to work with valid satellite IDs beyond the Breakup simulation.
     */
    size_t _currentMaxGivenID{0};

    /**
     * Member which is required to choose the correct Distribution function for the A/M values.
     * This value is set during the run of the simulation.
     */
    SatType _satType{SatType::SPACECRAFT};

    /**
     * This is the mass-sum of the input satellites.
     * Member which is required to check if the outputMass <= inputMass, if not some fragments have to be
     * deleted.
     */
    double _inputMass{0};

    /**
     * This is the mass-sum of the output satellites (fragments).
     * Member which is required to check if the outputMass <= inputMass, if not some fragments have to be
     * deleted.
     */
    double _outputMass{0};

    /**
     * This is per default false.
     * If this is set top true then the method enforceMassConservation() will add additional fragments to the
     * output until the output mass approximately equals the input mass.
     */
    bool _enforceMassConservation{false};

    /**
     * Contains the Power Law Exponent for the L_c distribution.
     * This constant is correctly set-up in the subclasses by an init method.
     */
    double _lcPowerLawExponent{0};

    /**
     * Contains Factor and Offset of the Delta (Ejection) Velocity Distribution.
     * This constant is correctly set-up in the subclasses by an init method.
     */
    std::pair<double, double> _deltaVelocityFactorOffset{std::make_pair(0, 0)};

    /**
     * This is potential member for testing purpose. It allows the user to fixate a specific seed (with the
     * method setSeed()). The produced std::mt19937 is then saved in this member and used to calculate random
     * values. The access on this member must be thread safe!
     * @note This member is only utilised by getRandomNumber() in case of a fixed seed
     */
    std::optional<std::mt19937> _fixRNG{std::nullopt};

    /**
     * This mutex is used if the member _fixRNG has an value, in order to protect parallel access on it.
     */
    std::mutex _rngMutex;

    /**
     * Contains the input satellites. Normally the fragmentCount for this collection is either one (explosion) or
     * two (collision)
     */
    std::vector<Satellite> _input;

    /**
     * Contains the output satellites aka fragments of the collision or explosion
     */
    Satellites _output;


public:

    Breakup() = default;

    explicit Breakup(std::vector<Satellite> input)
            : _input{std::move(input)},
              _output{} {}

    Breakup(std::vector<Satellite> input, double minimalCharacteristicLength)
            : _input{std::move(input)},
              _minimalCharacteristicLength{minimalCharacteristicLength} {};

    Breakup(std::vector<Satellite> input, double minimalCharacteristicLength,
            size_t currentMaxGivenID, bool enforceMassConservation)
            : _input{std::move(input)},
              _minimalCharacteristicLength{minimalCharacteristicLength},
              _currentMaxGivenID{currentMaxGivenID},
              _enforceMassConservation{enforceMassConservation} {}


    virtual ~Breakup() = default;

    /**
     * Runs the simulation.
     * Afterwards one can get the result with the member method getResult().
     */
    virtual void run();

    /**
     * Return the given input for this breakup event.
     * @return vector of satellites containing the input satellites
     */
    [[nodiscard]] std::vector<Satellite> getInput() const {
        return _input;
    }

    /**
     * Return the result of the breakup event.
     * @return vector of satellites containing the generated fragments
     */
    [[nodiscard]] std::vector<Satellite> getResult() const {
        return _output.getAoS();
    }

    /**
     * Return the result of the breakup event.
     * @return vector of satellites containing the generated fragments in an SoA
     */
    [[nodiscard]] Satellites getResultSoA() const {
        return _output;
    }

    /**
     * If this method is called with a seed, the Breakup will use only one synchronized generator for random number
     * generation with this specific seed. This makes whole simulation predictable and therefore very useful for testing.
     * If no argument is provided or if the std::nullopt is given, the Breakup will be reset to run with multiple
     * parallel random number generators each initialized with a seed from std::random_device.
     * @param seed - optional of unsigned long
     * @return this
     */
    Breakup &setSeed(std::optional<unsigned long> seed = std::nullopt);

protected:

    /**
     * This method does set up the process and correctly inits the required variables.
     * Implementation depends on the subclass.
     */
    virtual void init();

    /**
     * Creates the a number of fragments, following the Equation 2 for Explosions and
     * Equation 4 for Collisions.
     */
    virtual void calculateFragmentCount() = 0;

    /**
     * Actually creates the fragments (Resizes the vector and assigns a unique ID and name to each fragment)
     * Further the position vector is correctly set. This one is derived from the parents.
     * @param fragmentCount - the number of fragments which should be created
     * @param debrisName - the name for the fragments
     * @param position - position of the fragment, derived from the one parent (explosion) or from first parent (collision)
     */
    virtual void generateFragments(size_t fragmentCount, const std::array<double, 3> &position);

    /**
     * Creates the Size Distribution according to an specific powerLaw Exponent.
     * The Exponent comes from the probability density function (pdf) and depends on the subclass.
     * The subclasses therefore init _lcPowerLawExponent differently.
     */
    void characteristicLengthDistribution();

    /**
     * Creates for every satellite the area-to-mass ratio according to Equation 6.
     * This method also ensures that the output mass does not exceed the input mass.
     */
    void areaToMassRatioDistribution();

    /**
     * This method enforces the Mass Conservation.
     * It removes fragments if outputMass > inputMass and
     * it generates more fragments if outputMass < inputMass && _enforceMassConservation is enabled
     */
    void enforceMassConservation();

    virtual void addFurtherFragments();

    /**
     * This Method does assign each fragment a parent (trivial in Explosion case) and checks that
     * the step before did not produce more mass than the input contained if so warning is printed
     * Furthermore by assigning a parent, this method also assigns the base velocity of this fragment
     * @note The _output of the breakup should be in a random order before this is called (this holds always true)
     */
    virtual void assignParentProperties() = 0;

    /**
     * Implements the Delta Velocity Distribution according to Equation 11/ 12.
     * The parameters can be described as the following: mu = factor * chi + offset where mu is the mean value of the
     * normal distribution. The factor and the offset are both saved,
     * depending on the subclass with different values, in _deltaVelocityFactorOffset
     * The subclasses therefore init _deltaVelocityFactorOffset differently.
     */
    void deltaVelocityDistribution();

    /**
     * This Method calculates one characteristic Length for one Debris Particle.
     * This method uses equation (2) and (4) from the the NASA Breakup Model Paper.
     * @return L_c in [m]
     */
    double calculateCharacteristicLength();

    /**
     * Calculates an A/M Value for a given L_c.
     * The utilised equation is chosen based on L_c and the SatType attribute of this Breakup.
     * This method uses equation (5), (6) and (7) from the the NASA Breakup Model Paper.
     * @param characteristicLength in [m]
     * @return A/M value in [m^2/kg]
     */
    double calculateAreaMassRatio(double characteristicLength);

    /**
     * Calculates the Area for one fragment.
     * This method uses equation (8) and (9) from the the NASA Breakup Model Paper.
     * @param characteristicLength in [m]
     * @return Area in [m^2]
     */
    static double calculateArea(double characteristicLength);


    /**
     * Calculates the Mass for one fragment.
     * This method uses equation (10) from the the NASA Breakup Model Paper.
     * @param area in [m^2]
     * @param areaMassRatio in [m^2/kg]
     * @return Mass in [kg]
     */
    static double calculateMass(double area, double areaMassRatio);

    /**
     * Transforms a scalar velocity into a 3-dimensional cartesian velocity vector.
     * The transformation is based around a uniform Distribution.
     * @param velocity - scalar velocity
     * @return 3-dimensional cartesian velocity vector
     */
    std::array<double, 3> calculateVelocityVector(double velocity);

    /**
     * Returns a random number according to a specific distribution.
     * @tparam Distribution - should be a Distribution (e. g. a uniform Distribution) which returns doubles
     * @param distribution
     * @return a random number
     * @note This method is thread safe because if different threads call this they will have different random number
     * generators (for their whole life time)
     */
    template<class Distribution>
    double getRandomNumber(Distribution &distribution) {
        if (_fixRNG.has_value()) {
            const std::lock_guard<std::mutex> lock(_rngMutex);
            return distribution(_fixRNG.value());
        } else {
            thread_local std::mt19937 rng{std::random_device{}()};
            return distribution(rng);
        }
    }

public:

    [[nodiscard]] double getMinimalCharacteristicLength() const {
        return _minimalCharacteristicLength;
    }

    [[nodiscard]] double getMaximalCharacteristicLength() const {
        return _maximalCharacteristicLength;
    }

    [[nodiscard]] size_t getCurrentMaxGivenId() const {
        return _currentMaxGivenID;
    }
};


