#include "gtest/gtest.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <memory>
#include "breakupModel/model/Satellite.h"
#include "breakupModel/model/SatelliteBuilder.h"
#include "breakupModel/simulation/Collision.h"

class CollisionTest : public ::testing::Test {

protected:

    virtual void SetUp() {
        SatelliteBuilder satelliteBuilder{};
        sat1 = satelliteBuilder
                .setID(24946)
                .setName("Iridium 33")
                .setSatType(SatType::SPACECRAFT)
                .setMass(560)
                .setVelocity({11700.0, 0.0, 0.0})
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


TEST_F(CollisionTest, FragmentCountTest) {
    _collision->setSeed(std::make_optional(1234)).run();
    auto output = _collision->getResult();

    ASSERT_TRUE(_collision->isIsCatastrophic()) << "This collision was catastrophic";

    size_t expectedFragmentCount = 4064; //M=m_1+m_2 & L_c = 0.05 --> Equation 4

    ASSERT_EQ(output.size(), expectedFragmentCount);
}


TEST_F(CollisionTest, FragmentSizeDsitributionTest) {
    for (size_t t = 0; t < 100; ++t) {
        _collision->setSeed(std::make_optional(1234)).run();
        auto output = _collision->getResult();

        double Lc1 = sat1.getCharacteristicLength();
        double Lc2 = sat2.getCharacteristicLength();
        double expectedMaximalCharacteristicLength = Lc1 > Lc2 ? Lc1 : Lc2;

        ASSERT_FLOAT_EQ(_collision->getMaximalCharacteristicLength(), expectedMaximalCharacteristicLength);

        double Lc = _minimalCharacteristicLength;

        //1% Deviation for the Test Case (--> +-40 Fragments)
        double deviation = static_cast<double>(output.size()) * 0.01;

        while(Lc < expectedMaximalCharacteristicLength) {
            size_t count = std::count_if(output.begin(), output.end(),[Lc](Satellite &sat) {
                return sat.getCharacteristicLength() > Lc;
            });

            double expectedCount = 0.1 * std::pow(sat1.getMass() + sat2.getMass(), 0.75) * std::pow(Lc, -1.71);

            size_t expectedUpperBound = static_cast<size_t>(expectedCount + deviation);
            size_t expectedLowerBound = expectedCount - deviation > 0 ? static_cast<size_t>(expectedCount - deviation) : 0;

            ASSERT_GE(count, expectedLowerBound) << "L_c was " << Lc;;
            ASSERT_LE(count, expectedUpperBound) << "L_c was " << Lc;;

            Lc += 0.1;
        }
    }
}

TEST_F(CollisionTest, CheckNoRaceCondition) {
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