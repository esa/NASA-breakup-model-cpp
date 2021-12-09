#include "gtest/gtest.h"

#include <utility>
#include <vector>
#include "breakupModel/util/UtilityFunctions.h"
#include "breakupModel/util/UtilityKepler.h"


class UtilityKeplerTest01 : public ::testing::TestWithParam<double> {

};

/**
 * Conversion Test Mean Anomaly <--> Eccentric Anomaly
 */
TEST_P(UtilityKeplerTest01, meanAnomalyToEccentricAnomaly){
    using namespace util;
    double eccentricity = 0.5;

    double expectedMeanAnomaly = GetParam();

    double eccentricAnomaly = meanAnomalyToEccentricAnomaly(expectedMeanAnomaly, eccentricity);

    double actualMeanAnomaly = eccentricAnomalyToMeanAnomaly(eccentricAnomaly, eccentricity);

    ASSERT_DOUBLE_EQ(actualMeanAnomaly, expectedMeanAnomaly);
}

/**
 * Conversion Test True Anomaly <--> Eccentric Anomaly
 */
TEST_P(UtilityKeplerTest01, trueAnomalyToEccentricAnomaly){
    using namespace util;
    double eccentricity = 0.5;

    double expectedTrueAnomaly = GetParam();

    double eccentricAnomaly = trueAnomalyToEccentricAnomaly(expectedTrueAnomaly, eccentricity);

    double actualTrueAnomaly = eccentricAnomalyToTrueAnomaly(eccentricAnomaly, eccentricity);

    ASSERT_DOUBLE_EQ(actualTrueAnomaly, expectedTrueAnomaly);
}

/**
 * Returns a vector containing all possible angles in [rad] in the interval [0.0, pi[ with spacing 0.1;
 * @return an vector containing 63 values from 0.0 to 3.1
 */
std::vector<double> radValues() {
    std::vector<double> val{};
    val.reserve(63);
    double d = 0.0;
    while (d < util::PI) {
        val.push_back(d);
        d += 0.1;
    }
    return val;
}

INSTANTIATE_TEST_SUITE_P(DoubleParam, UtilityKeplerTest01,
                         ::testing::ValuesIn(radValues()));

class UtilityKeplerTest02 : public ::testing::TestWithParam<double> {

};

/**
 * Returns a vector containing double values.
 * @return an vector containing 50 values from ]0.0; 125.0]
 */
std::vector<double> meanMotionValues() {
    std::vector<double> val{};
    val.reserve(50);
    double d = 2.5;
    while (d <= 125.0) {
        val.push_back(d);
        d += 2.5;
    }
    return val;
}

/**
 * Conversion Mean Motion --> Semi Major Axis
 */
TEST_P(UtilityKeplerTest02, meanMotionToSemiMajorAxisConversion) {
    using namespace util;
    double  meanMotion = GetParam();

    double expectedSemiMajorAxis = std::pow(GRAVITATIONAL_PARAMETER_EARTH, 1.0/3.0) / std::pow(2 * PI * meanMotion / 86400.0, 2.0/3.0);

    double actualSemiMajorAxis = meanMotionToSemiMajorAxis(meanMotion);

    ASSERT_NEAR(actualSemiMajorAxis, expectedSemiMajorAxis, 0.1);
}

INSTANTIATE_TEST_SUITE_P(DoubleParam, UtilityKeplerTest02,
                         ::testing::ValuesIn(meanMotionValues()));
