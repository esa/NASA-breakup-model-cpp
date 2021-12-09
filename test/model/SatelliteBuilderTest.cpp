#include "gtest/gtest.h"

#include "breakupModel/model/SatelliteBuilder.h"
#include <array>
#include <exception>


class SatelliteBuilderTest : public ::testing::Test {

protected:

    virtual void SetUp() {
        _satelliteBuilder = SatelliteBuilder{};
        _array123 = std::array<double, 3>{1.0, 2.0, 3.0};
        _array000 = std::array<double, 3>{0.0, 0.0, 0.0};
    }

    SatelliteBuilder _satelliteBuilder{};
    std::array<double, 3> _array123{};
    std::array<double, 3> _array000{};

};

/**
 * Checks the correct setup for Sputnik 1
 * Mass is set, Area and L_c should then be "-1"
 */
TEST_F(SatelliteBuilderTest, Sat1_Sputnik_1_Test) {
    Satellite sat = _satelliteBuilder
            .setID(1)
            .setName("Sputnik 1")
            .setSatType("SPACECRAFT")
            .setMass(83)
            .setVelocity(_array123)
            .getResult();

    EXPECT_EQ(sat.getId(), 1);
    EXPECT_EQ(sat.getName(), "Sputnik 1");
    EXPECT_EQ(sat.getSatType(), SatType::SPACECRAFT);

    //Mass is set area and L_c are determined by it
    EXPECT_EQ(sat.getMass(), 83);
    EXPECT_NEAR(sat.getArea(), 1.2598, 0.001);
    EXPECT_NEAR(sat.getCharacteristicLength(), 1.2665, 0.001);

    EXPECT_EQ(sat.getVelocity(), _array123);
    EXPECT_EQ(sat.getPosition(), _array000);
}

/**
 * Checks the correct set-up for Vanguard 1 ("1958-002B").
 * Calculated values for Mass and L_c are validated with the Python Implementation.
 * (Python Implementation knows the radius as attribute, therefore in the corresponding check * 2.0)
 */
TEST_F(SatelliteBuilderTest, Sat2_Vanguard_1_Test) {
    Satellite sat = _satelliteBuilder
            .setID(5)
            .setName("Vanguard 1")
            .setSatType(SatType::SPACECRAFT)
            .setMassByArea(0.12)
            .setVelocity(_array000)
            .getResult();

    EXPECT_EQ(sat.getId(), 5);
    EXPECT_EQ(sat.getName(), "Vanguard 1");
    EXPECT_EQ(sat.getSatType(), SatType::SPACECRAFT);

    //Area is set (cataloged as radar cross section RCS in the satcat), mass and L_c are determined by it
    EXPECT_NEAR(sat.getMass(), 5.82933889321, 0.01);
    EXPECT_EQ(sat.getArea(), 0.12);
    EXPECT_NEAR(sat.getCharacteristicLength(), 0.195522421555 * 2, 0.01);

    EXPECT_EQ(sat.getVelocity(), _array000);
    EXPECT_EQ(sat.getPosition(), _array000);

}

/**
 * Checks the correct set-up for Tiros 4 ("1962-002A").
 * Calculated values for Mass and L_c are validated with the Python Implementation.
 * (Python Implementation knows the radius as attribute, therefore in the corresponding check * 2.0)
 */
TEST_F(SatelliteBuilderTest, Sat3_Tiros_4_Test) {
    Satellite sat = _satelliteBuilder
            .setID(226)
            .setName("Tiros 4")
            .setSatType(SatType::SPACECRAFT)
            .setMassByArea(0.8351)
            .setVelocity(_array000)
            .getResult();

    EXPECT_EQ(sat.getId(), 226);
    EXPECT_EQ(sat.getName(), "Tiros 4");
    EXPECT_EQ(sat.getSatType(), SatType::SPACECRAFT);

    //Area is set (cataloged as radar cross section RCS in the satcat), mass and L_c are determined by it
    EXPECT_NEAR(sat.getMass(), 52.1555094977, 0.01);
    EXPECT_EQ(sat.getArea(), 0.8351);
    EXPECT_NEAR(sat.getCharacteristicLength(), 0.515577914531 * 2, 0.01);

    EXPECT_EQ(sat.getVelocity(), _array000);
    EXPECT_EQ(sat.getPosition(), _array000);

}

/**
 * Checks the correct set-up for 1962-060B ("THOR ABLESTAR R/B") an old Rocket Body.
 * Calculated values for Mass and L_c are validated with the Python Implementation.
 * (Python Implementation knows the radius as attribute, therefore in the corresponding check * 2.0)
 */
TEST_F(SatelliteBuilderTest, Sat4_Thor_Ablestar_RB_Test) {
    Satellite sat = _satelliteBuilder
            .setID(447)
            .setName("1962-060B")
            .setSatType(SatType::ROCKET_BODY)
            .setMassByArea(3.5428)
            .setVelocity(_array000)
            .getResult();

    EXPECT_EQ(sat.getId(), 447);
    EXPECT_EQ(sat.getName(), "1962-060B");
    EXPECT_EQ(sat.getSatType(), SatType::ROCKET_BODY);

    //Area is set (cataloged as radar cross section RCS in the satcat), mass and L_c are determined by it
    EXPECT_NEAR(sat.getMass(), 266.991481026, 0.01);
    EXPECT_EQ(sat.getArea(), 3.5428);
    EXPECT_NEAR(sat.getCharacteristicLength(), 1.0619360926 * 2, 0.01);

    EXPECT_EQ(sat.getVelocity(), _array000);
    EXPECT_EQ(sat.getPosition(), _array000);

}

/**
 * Incomplete information for breakup
 */
TEST_F(SatelliteBuilderTest, Expection_No_ID) {
    ASSERT_THROW(
            _satelliteBuilder
                    .setName("NoSatelliteAtAll")
                    .setSatType(SatType::UNKNOWN)
                    .setMass(10)
                    .setVelocity(_array000)
                    .getResult(),
            std::runtime_error
    ) << "No exception was thrown although the satellite is invalid and has no ID";
}

/**
 * Incomplete information for breakup
 */
TEST_F(SatelliteBuilderTest, Expection_No_Mass) {
    ASSERT_THROW(
            _satelliteBuilder
                    .setID(1000)
                    .setName("NoSatelliteAtAll")
                    .setSatType(SatType::UNKNOWN)
                    .setVelocity(_array000)
                    .getResult(),
            std::runtime_error
    ) << "No exception was thrown although the satellite is invalid and has no mass";
}

/**
 * Incomplete information for breakup
 */
TEST_F(SatelliteBuilderTest, Expection_No_Velocity) {
    ASSERT_THROW(
            _satelliteBuilder
                    .setID(1000)
                    .setName("NoSatelliteAtAll")
                    .setMass(10)
                    .setSatType(SatType::UNKNOWN)
                    .getResult(),
            std::runtime_error
    ) << "No exception was thrown although the satellite is invalid and has no velocity";
}