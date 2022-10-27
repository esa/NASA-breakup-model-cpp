#include "gtest/gtest.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <memory>
#include "breakupModel/model/Satellite.h"
#include "breakupModel/model/SatelliteBuilder.h"
#include "breakupModel/simulation/Collision.h"

class NonCatastrophicCollisionTest : public ::testing::Test {

protected:

    virtual void SetUp() {
        SatelliteBuilder satelliteBuilder{};
        sat1 = satelliteBuilder
                .setID(24946)
                .setName("Iridium 33")
                .setSatType(SatType::SPACECRAFT)
                .setMass(560)
                .setVelocity({100.0, 0.0, 0.0})
                .getResult();
        sat2 = satelliteBuilder
                .setID(22675)
                .setName("Kosmos 2251")
                .setSatType(SatType::SPACECRAFT)
                .setMass(950)
                .setVelocity({0, 0.0, 0.0})
                .getResult();

        _input.push_back(sat1);
        _input.push_back(sat2);


        _collision = std::make_unique<Collision>(_input, _minimalCharacteristicLength);
    }

    std::vector<Satellite> _input{};

    Satellite sat1;
    Satellite sat2;

    double _minimalCharacteristicLength{0.05};

    std::unique_ptr<Collision> _collision;

};


TEST_F(NonCatastrophicCollisionTest, FragmentCountTest) {
    _collision->setSeed(std::make_optional(8)).run();
    auto output = _collision->getResult();

    ASSERT_FALSE(_collision->isIsCatastrophic()) << "This collision was non-catastrophic";

    size_t expectedFragmentCount = 61;

    ASSERT_EQ(output.size(), expectedFragmentCount);
}

TEST_F(NonCatastrophicCollisionTest, CheckRemnant) {
    _collision = std::make_unique<Collision>(_input, _minimalCharacteristicLength, 0, true);
    _collision->setSeed(std::make_optional(8)).run();
    auto output = _collision->getResult();

    ASSERT_FALSE(_collision->isIsCatastrophic()) << "This collision was non-catastrophic";

    size_t expectedFragmentCount = 62;

    ASSERT_EQ(output.size(), expectedFragmentCount);

    // The Remnant
    ASSERT_NEAR(output[0].getMass(), 1505.0, 1.0);
}


TEST_F(NonCatastrophicCollisionTest, FragmentSizeDsitributionTest) {
    using namespace util;
    _collision->setSeed(std::make_optional(10)).run();
    auto output = _collision->getResult();

    double Lc1 = sat1.getCharacteristicLength();
    double Lc2 = sat2.getCharacteristicLength();
    double expectedMaximalCharacteristicLength = Lc1 > Lc2 ? Lc1 : Lc2;

    ASSERT_FLOAT_EQ(_collision->getMaximalCharacteristicLength(), expectedMaximalCharacteristicLength);

    double Lc = _minimalCharacteristicLength;

    //10% Deviation for the Test Case (--> +-6 Fragments)
    double deviation = static_cast<double>(output.size()) * 0.1;

    while(Lc < expectedMaximalCharacteristicLength) {
        size_t count = std::count_if(output.begin(), output.end(),[Lc](Satellite &sat) {
            return sat.getCharacteristicLength() > Lc;
        });

        const double dv = euclideanNorm(sat1.getVelocity() - sat2.getVelocity());
        const double dv2 = dv * dv;

        double expectedCount = 0.1 * std::pow(sat1.getMass() * dv2 / 1e6, 0.75) * std::pow(Lc, -1.71);

        size_t expectedUpperBound = static_cast<size_t>(expectedCount + deviation);
        size_t expectedLowerBound = expectedCount - deviation > 0 ? static_cast<size_t>(expectedCount - deviation) : 0;

        ASSERT_GE(count, expectedLowerBound) << "L_c was " << Lc;
        ASSERT_LE(count, expectedUpperBound) << "L_c was " << Lc;

        Lc += 0.1;
    }
}

TEST_F(NonCatastrophicCollisionTest, CheckNoRaceCondition) {
    for (size_t x = 0; x < 50; ++x) {
        size_t count = 0;
        _collision->run();
        auto output = _collision->getResultSoA();
        for (double lc1 : output.characteristicLength) {
            for (double lc2 : output.characteristicLength) {
                bool condition = std::abs(lc1 - lc2) < 1e-16;
                if (condition) {
                    count += 1;
                }
            }
        }
        count -= output.characteristicLength.size();
        //10 is threshold
        //If we would have race conditions, it can be assumed that there are a lot more than 10 duplicates
        EXPECT_LT(count, 10) << "Count of Duplicates in Iteration " << x << "\n"
        << "If this test fails this not necessarily bad. This checks if we have any duplicates in the L_c set.\n"
           "If there are any, this might be an issue and a hint for a race condition but not necessarily\n"
           "It could also be just a random coincidence of the RNG\n"
           "Rerun this in such a case!\n";
    }
}