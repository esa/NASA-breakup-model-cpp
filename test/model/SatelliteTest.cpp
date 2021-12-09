#include "gtest/gtest.h"

#include "breakupModel/model/Satellite.h"
#include "breakupModel/model/OrbitalElementsFactory.h"
#include "breakupModel/model/OrbitalElements.h"
#include "breakupModel/util/UtilityKepler.h"
#include <array>
#include <utility>
#include <tuple>

/**
 * Tests about Satellite construction and general stuff
 */
class SatelliteTest : public ::testing::Test {

};

/**
 * Ensure that that all arguments are correctly set-up (set to zero expect ID)
 */
TEST_F(SatelliteTest, Constructor) {
    Satellite sat{1};
    std::array<double, 3> zeroArray{{0.0, 0.0, 0.0}};

    EXPECT_EQ(sat.getId(), 1);
    EXPECT_EQ(sat.getArea(), 0.0);
    EXPECT_EQ(sat.getAreaToMassRatio(), 0.0);
    EXPECT_EQ(sat.getCharacteristicLength(), 0.0);
    EXPECT_EQ(sat.getMass(), 0.0);
    EXPECT_EQ(sat.getName(), "");
    EXPECT_EQ(sat.getVelocity(), zeroArray);
    EXPECT_EQ(sat.getPosition(), zeroArray);
}

TEST_F(SatelliteTest, operatorEQTest) {
    Satellite sat1{1};
    Satellite sat2{2};

    ASSERT_FALSE(sat1 == sat2);
}

TEST_F(SatelliteTest, operatorNEQTest) {
    Satellite sat1{1};
    Satellite sat2{2};

    ASSERT_TRUE(sat1 != sat2);
}

TEST_F(SatelliteTest, KeplerConversionTest) {
    OrbitalElementsFactory factory{};
    auto expectedKepler = factory.createFromTLEData({15.72125391, 0.0006703, 51.6416,
                                                     247.4627, 130.5360, 325.0288});
    Satellite sat{1};
    sat.setCartesianByOrbitalElements(expectedKepler);


    auto actualKepler = sat.getOrbitalElements();

    for (unsigned int i = 0; i < 6; ++i) {
        EXPECT_NEAR(actualKepler[i], expectedKepler[i], 0.0001) << "i=" << i;
    }
}

/**
 * Testing the conversion by using the implementations against each other
 */
class SatelliteOrbitalElementsTest01 : public ::testing::TestWithParam<OrbitalElements> {

};

std::vector<OrbitalElements> getOrbitalParameters() {
    OrbitalElementsFactory factory{};
    std::vector<OrbitalElements> values{};

    //Why is the 0. value not in the vector?
    //If eccentricity is zero and inclination too, then W and w have no influence/ are not unique
    std::array<double, 6> kepler{{6800000.0, 0.0, 0.0,
                                         0.0, 0.0, 1.5708}};

    for (unsigned int i = 0; i < 50; ++i) {
        kepler[0] += 100000;        //+100km
        kepler[1] += 0.01;          //+0.01 eccentricity
        kepler[2] += 0.0349066;     //+2 deg inclination
        kepler[3] += 0.0174533;     //+1 deg RAAN
        kepler[4] += 0.00872665;    //+0.5 deg argument of perigee
        values.push_back(factory.createFromOnlyRadians(kepler, OrbitalAnomalyType::MEAN));
    }

    return values;
}

TEST_P(SatelliteOrbitalElementsTest01, KeplerConversionTest) {
    auto expectedKepler = GetParam();
    Satellite sat{1};

    sat.setCartesianByOrbitalElements(expectedKepler);

    auto actualKepler = sat.getOrbitalElements();

    for (unsigned int i = 0; i < 6; ++i) {
        EXPECT_NEAR(actualKepler[i], expectedKepler[i], 0.0001) << "i = " << i;
    }
}

INSTANTIATE_TEST_SUITE_P(KeplerArgumentParam, SatelliteOrbitalElementsTest01,
                         ::testing::ValuesIn(getOrbitalParameters()));

/**
 * Testing the conversion by using hardcoded values
 */
class SatelliteOrbitalElementsTest02 : public ::testing::TestWithParam<std::tuple<
        std::array<double, 3>, std::array<double, 3>, OrbitalElements>> {

};

std::vector<std::tuple<std::array<double, 3>, std::array<double, 3>, OrbitalElements>> getOrbitalTuples() {
    OrbitalElementsFactory factory{};
    std::array<double, 3> position{};
    std::array<double, 3> velocity{};
    OrbitalElements orbitalElements{};
    std::vector<std::tuple<std::array<double, 3>, std::array<double, 3>, OrbitalElements>> values{};

    //Value Tuple 1 (METEOSAT-11 (MSG4))
    position = {18887167.8187, -37701817.738, 31043.7678};
    velocity = {2748.7608, 1377.0359, -8.8903};
    orbitalElements =
            factory.createOrbitalElements(42165260.2513,
                                          0.00006880,
                                          0.1709691, AngularUnit::DEGREE,
                                          130.8922575, AngularUnit::DEGREE,
                                          341.0477201, AngularUnit::DEGREE,
                                          184.6690779, AngularUnit::DEGREE, OrbitalAnomalyType::TRUE);
    values.emplace_back(position, velocity, orbitalElements);

    //Value Tuple 2 (METEOSAT-10 (MSG3))
    position = {24897525.3639, -34028444.5369, -840177.9325};
    velocity = {2481.1036, 1814.7995, 24.492};
    orbitalElements =
            factory.createOrbitalElements(42165127.2712,
                                          0.00017770,
                                          1.22945, AngularUnit::DEGREE,
                                          14.390738, AngularUnit::DEGREE,
                                          106.7172695, AngularUnit::DEGREE,
                                          185.0791856, AngularUnit::DEGREE, OrbitalAnomalyType::TRUE);
    values.emplace_back(position, velocity, orbitalElements);

    //Value 3 (METEOSAT-9 (MSG2))
    position = {21134674.4529, -36363005.5896, -3068449.6617};
    velocity = {2647.1592, 1553.4, -174.4594};
    orbitalElements =
            factory.createOrbitalElements(42165418.8789,
                                          0.00012650,
                                          5.294638, AngularUnit::DEGREE,
                                          68.2369626, AngularUnit::DEGREE,
                                          37.4740299, AngularUnit::DEGREE,
                                          194.5736113, AngularUnit::DEGREE, OrbitalAnomalyType::TRUE);
    values.emplace_back(position, velocity, orbitalElements);

    //Value 4 (METEOSAT-8 (MSG1))
    position = {38955984.8715, -15262526.8754, -5246772.9352};
    velocity = {1133.4294 , 2855.8646, 110.6545};
    orbitalElements =
            factory.createOrbitalElements(42166536.8719,
                                          0.00011150,
                                          7.4421763, AngularUnit::DEGREE,
                                          52.3491291, AngularUnit::DEGREE,
                                          19.7874842, AngularUnit::DEGREE,
                                          266.3388919, AngularUnit::DEGREE, OrbitalAnomalyType::TRUE);
    values.emplace_back(position, velocity, orbitalElements);

    return values;
}

TEST_P(SatelliteOrbitalElementsTest02, KeplerConversionTest) {
    auto value = GetParam();
    const auto &expectedPosition = std::get<0>(value);
    const auto &expectedVelocity = std::get<1>(value);
    const auto &expectedOrbitalElements = std::get<2>(value);

    Satellite satellite{};
    satellite.setVelocity(expectedVelocity);
    satellite.setPosition(expectedPosition);

    //Abs error for unit meter (high, because of imprecise input data! Therefore +/- 1km)
    ASSERT_NEAR(satellite.getOrbitalElements()[0], expectedOrbitalElements[0], 1000) << "Semi-major-Axis wrong";
    //Abs error for eccentricity, relative samll
    ASSERT_NEAR(satellite.getOrbitalElements()[1], expectedOrbitalElements[1], 0.000001) << "Eccentricity wrong";

    //Abs error for angles in radian
    ASSERT_NEAR(satellite.getOrbitalElements()[2], expectedOrbitalElements[2], 0.001) << "Inclination wrong";
    ASSERT_NEAR(satellite.getOrbitalElements()[3], expectedOrbitalElements[3], 0.001) << "RAAN wrong";
    ASSERT_NEAR(satellite.getOrbitalElements()[4], expectedOrbitalElements[4], 0.001) << "Arg. of. Per. wrong";
    ASSERT_NEAR(satellite.getOrbitalElements()[5], expectedOrbitalElements[5], 0.001) << "Anomaly wrong";

    satellite.setCartesianByOrbitalElements(expectedOrbitalElements);

    //Abs error for unit meter (high, because of imprecise input data! Therefore +/- 1km)
    ASSERT_NEAR(satellite.getPosition()[0], expectedPosition[0], 1000) << "X-Position wrong";
    ASSERT_NEAR(satellite.getPosition()[1], expectedPosition[1], 1000) << "Y-Position wrong";
    ASSERT_NEAR(satellite.getPosition()[2], expectedPosition[2], 1000) << "Z-Position wrong";
    ASSERT_NEAR(satellite.getVelocity()[0], expectedVelocity[0], 1000) << "X-Velocity wrong";
    ASSERT_NEAR(satellite.getVelocity()[1], expectedVelocity[1], 1000) << "Y-Velocity wrong";
    ASSERT_NEAR(satellite.getVelocity()[2], expectedVelocity[2], 1000) << "Z-Velocity wrong";

}

INSTANTIATE_TEST_SUITE_P(CartesianKeplerParamTuple, SatelliteOrbitalElementsTest02,
                         ::testing::ValuesIn(getOrbitalTuples()));