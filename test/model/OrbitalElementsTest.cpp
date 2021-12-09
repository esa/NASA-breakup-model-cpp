#include "gtest/gtest.h"

#include "breakupModel/model/OrbitalElements.h"
#include "breakupModel/model/OrbitalElementsFactory.h"

TEST(OrbitalElementsTest, OrbitalElements01) {
    OrbitalElementsFactory factory{};

    double expectedSemiMajorAxis = 42165260.2513;
    double expectedEccentricity = 0.00006880;
    double expectedInclination = 89.76;
    double expectedRAAN = 1.22222;
    double expectedArgOfPer = 341.0477201;
    double expectedTrueAnomaly = 184.6690779;

    OrbitalElements actualOrbitalElements =
            factory.createOrbitalElements(expectedSemiMajorAxis,
                                          expectedEccentricity,
                                          expectedInclination, AngularUnit::DEGREE,
                                          expectedRAAN, AngularUnit::RADIAN,
                                          expectedArgOfPer, AngularUnit::DEGREE,
                                          expectedTrueAnomaly, AngularUnit::DEGREE, OrbitalAnomalyType::TRUE);

    ASSERT_DOUBLE_EQ(expectedSemiMajorAxis, actualOrbitalElements.getSemiMajorAxis());
    ASSERT_DOUBLE_EQ(expectedEccentricity, actualOrbitalElements.getEccentricity());
    ASSERT_DOUBLE_EQ(expectedInclination, actualOrbitalElements.getInclination(AngularUnit::DEGREE));
    ASSERT_DOUBLE_EQ(expectedRAAN, actualOrbitalElements.getLongitudeOfTheAscendingNode(AngularUnit::RADIAN));
    ASSERT_DOUBLE_EQ(expectedArgOfPer, actualOrbitalElements.getArgumentOfPeriapsis(AngularUnit::DEGREE));
    ASSERT_DOUBLE_EQ(expectedTrueAnomaly,
                     actualOrbitalElements.getAnomaly(AngularUnit::DEGREE, OrbitalAnomalyType::TRUE));

}

TEST(OrbitalElementsTest, OrbitalElements02) {
    OrbitalElementsFactory factory{};

    double expectedSemiMajorAxis = 42165260.2513;
    double expectedEccentricity = 0.00006880;
    double expectedInclination = 89.76;
    double expectedRAAN = 1.22222;
    double expectedArgOfPer = 341.0477201;
    double expectedTrueAnomaly = 184.6690779;

    OrbitalElements actualOrbitalElements =
            factory.createFromOnlyDegree({expectedSemiMajorAxis, expectedEccentricity,
                                          expectedInclination, expectedRAAN,
                                          expectedArgOfPer, expectedTrueAnomaly},
                                         OrbitalAnomalyType::ECCENTRIC);

    ASSERT_DOUBLE_EQ(expectedSemiMajorAxis, actualOrbitalElements.getSemiMajorAxis());
    ASSERT_DOUBLE_EQ(expectedEccentricity, actualOrbitalElements.getEccentricity());
    ASSERT_DOUBLE_EQ(expectedInclination, actualOrbitalElements.getInclination(AngularUnit::DEGREE));
    ASSERT_DOUBLE_EQ(expectedRAAN, actualOrbitalElements.getLongitudeOfTheAscendingNode(AngularUnit::DEGREE));
    ASSERT_DOUBLE_EQ(expectedArgOfPer, actualOrbitalElements.getArgumentOfPeriapsis(AngularUnit::DEGREE));
    ASSERT_DOUBLE_EQ(expectedTrueAnomaly, actualOrbitalElements.getAnomaly(AngularUnit::DEGREE));

}

TEST(OrbitalElementsTest, Epoch01) {
    Epoch epoch{2016, 031.25992506};
    std::tm actualTm = epoch.toTm();
    std::tm expectedTm {};
    expectedTm.tm_sec = 17;
    expectedTm.tm_min = 14;
    expectedTm.tm_hour = 6;
    expectedTm.tm_mday = 31;
    expectedTm.tm_mon = 0;
    expectedTm.tm_yday = 31;
    expectedTm.tm_year = 2016;

    EXPECT_EQ(actualTm.tm_sec, expectedTm.tm_sec);
    EXPECT_EQ(actualTm.tm_min, expectedTm.tm_min);
    EXPECT_EQ(actualTm.tm_hour, expectedTm.tm_hour);
    EXPECT_EQ(actualTm.tm_mday, expectedTm.tm_mday);
    EXPECT_EQ(actualTm.tm_mon, expectedTm.tm_mon);
    EXPECT_EQ(actualTm.tm_yday, expectedTm.tm_yday);
    EXPECT_EQ(actualTm.tm_year, expectedTm.tm_year);
}

TEST(OrbitalElementsTest, Epoch02) {
    Epoch epoch{2006, 040.85138889};
    std::tm actualTm = epoch.toTm();
    std::tm expectedTm {};
    expectedTm.tm_sec = 0;
    expectedTm.tm_min = 26;
    expectedTm.tm_hour = 20;
    expectedTm.tm_mday = 9;
    expectedTm.tm_mon = 1;
    expectedTm.tm_yday = 40;
    expectedTm.tm_year = 2006;

    EXPECT_EQ(actualTm.tm_sec, expectedTm.tm_sec);
    EXPECT_EQ(actualTm.tm_min, expectedTm.tm_min);
    EXPECT_EQ(actualTm.tm_hour, expectedTm.tm_hour);
    EXPECT_EQ(actualTm.tm_mday, expectedTm.tm_mday);
    EXPECT_EQ(actualTm.tm_mon, expectedTm.tm_mon);
    EXPECT_EQ(actualTm.tm_yday, expectedTm.tm_yday);
    EXPECT_EQ(actualTm.tm_year, expectedTm.tm_year);
}