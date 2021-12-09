#include "gtest/gtest.h"

#include <utility>
#include "breakupModel/util/UtilityFunctions.h"


class UtilityFunctions : public ::testing::Test{

};

class UtilityFunctionsDouble :
        public UtilityFunctions,
        public ::testing::WithParamInterface<double> {

};

TEST_P(UtilityFunctionsDouble, CharacteristicLengthFromMass){
    using namespace util;
    double expectedCharacteristicLength = GetParam();

    double mass = calculateSphereMass(expectedCharacteristicLength);

    double actualCharacteristicLength = calculateCharacteristicLengthFromMass(mass);

    ASSERT_NEAR(actualCharacteristicLength, expectedCharacteristicLength, 0.001);
}

INSTANTIATE_TEST_SUITE_P(DoubleParam, UtilityFunctionsDouble,
                        ::testing::Values(0.2, 0.7589, 2.42, 2.5, 3, 7.89));


class UtilityFunctionsPair :
        public UtilityFunctions,
        public ::testing::WithParamInterface<std::pair<double, double>> {

};

TEST_P(UtilityFunctionsPair, TransformUniformToPowerLaw){
    using namespace util;
    auto param = GetParam();

    double expectedValue = std::get<1>(param);
    double y = std::get<0>(param);

    //std::numeric_limits<double>::infinity - Double check when Tests fails, not that great solution.
    double actualValue = transformUniformToPowerLaw(0.05, std::numeric_limits<double>::infinity(), -2.71, y);

    ASSERT_NEAR(actualValue, expectedValue, 0.0001);
}

INSTANTIATE_TEST_SUITE_P(PairDoubleDoubleParam, UtilityFunctionsPair,
                         ::testing::Values(
                                 std::make_pair(0.0966, 0.05306),
                                 std::make_pair(0.66922, 0.09549),
                                 std::make_pair(0.22816, 0.05818)
                        ));