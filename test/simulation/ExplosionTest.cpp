#include "gtest/gtest.h"

#include <vector>
#include <algorithm>
#include <cmath>
#include <memory>
#include "breakupModel/model/Satellite.h"
#include "breakupModel/model/SatelliteBuilder.h"
#include "breakupModel/simulation/Explosion.h"

class ExplosionTest : public ::testing::Test {

protected:

    virtual void SetUp() {
        SatelliteBuilder satelliteBuilder{};
        sat = satelliteBuilder
                .setID(7946)
                .setName("1975-052B")   //Nimbus 6 R/B breakup due to Propulsion system
                .setSatType(SatType::ROCKET_BODY)
                .setMass(839)           //Upper Stage was a Delta 2nd stage with 839 kg mass
                .setVelocity({0.0, 0.0, 0.0})   //Irrelevant
                .getResult();

        _input.push_back(sat);


        _explosion = std::make_unique<Explosion>(_input, _minimalCharacteristicLength);
    }

    std::vector<Satellite> _input{};
    Satellite sat;

    double _minimalCharacteristicLength{0.05};

    std::unique_ptr<Explosion> _explosion;

};


TEST_F(ExplosionTest, FragmentCountTest) {
    _explosion->setSeed(std::make_optional(1234)).run();
    auto output = _explosion->getResult();

    size_t expectedFragmentCount = 724; //L_c = 0.05
    ASSERT_EQ(output.size(), expectedFragmentCount);
}


TEST_F(ExplosionTest, FragmentSizeDsitributionTest) {
    for (size_t t = 0; t < 100; ++t) {
        _explosion->setSeed(std::make_optional(1234)).run();
        auto output = _explosion->getResult();

        double expectedMaximalCharacteristicLength = sat.getCharacteristicLength();

        ASSERT_FLOAT_EQ(_explosion->getMaximalCharacteristicLength(), expectedMaximalCharacteristicLength);

        double Lc = _minimalCharacteristicLength;

        //2% Deviation for the Test Case (--> +-14 Fragments)
        double deviation = static_cast<double>(output.size()) * 0.02;

        while(Lc < expectedMaximalCharacteristicLength) {
            size_t count = std::count_if(output.begin(), output.end(),[Lc](Satellite &sat) {
                return sat.getCharacteristicLength() > Lc;
            });

            double expectedCount = 6 * std::pow(Lc, -1.6);

            size_t expectedUpperBound = static_cast<size_t>(expectedCount + deviation);
            size_t expectedLowerBound = expectedCount - deviation > 0 ? static_cast<size_t>(expectedCount - deviation) : 0;

            ASSERT_GE(count, expectedLowerBound) << "L_c was " << Lc;
            ASSERT_LE(count, expectedUpperBound) << "L_c was " << Lc;

            Lc += 0.1;
        }
    }

}

TEST_F(ExplosionTest, CheckNoRaceCondition) {
    for (size_t x = 0; x < 50; ++x) {
        size_t count = 0;
        _explosion->run();
        auto output = _explosion->getResultSoA();
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