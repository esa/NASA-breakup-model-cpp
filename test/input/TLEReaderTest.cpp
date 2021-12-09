#include "gtest/gtest.h"

#include <string>
#include <array>
#include <utility>
#include "breakupModel/input/TLEReader.h"
#include "breakupModel/model/OrbitalElementsFactory.h"

class TLEReaderTest : public ::testing::Test {

protected:

    virtual void SetUp() {
        OrbitalElementsFactory factory{};
        _expectedKepler_1 = factory.createFromTLEData({15.72125391, 0.0006703, 51.6416,
                                                       247.4627, 130.5360, 325.0288});
        _expectedKepler_2 = factory.createFromTLEData({1.00272877, 0.0000694, 0.0541,
                                                       226.6478, 252.0694, 256.3446});
        _expectedKepler_3 = _expectedKepler_1;

    }

    size_t _expectedID_1 = 25544;
    OrbitalElements _expectedKepler_1;

    size_t _expectedID_2 = 48808;
    OrbitalElements _expectedKepler_2;

    size_t _expectedID_3 = 275544; //T5544
    OrbitalElements _expectedKepler_3;

};

TEST_F(TLEReaderTest, readTLE_01_Test) {
    TLEReader tleReader{"resources/TLEReaderTest01.txt"};

    auto map = tleReader.getMappingIDOrbitalElements();

    ASSERT_EQ(map.size(), 1);
    ASSERT_EQ(map.count(_expectedID_1), 1);

    auto actualKepler = map[_expectedID_1];
    ASSERT_EQ(actualKepler, _expectedKepler_1);
}

TEST_F(TLEReaderTest, readTLE_02_Test) {
    TLEReader tleReader{"resources/TLEReaderTest02.txt"};

    auto map = tleReader.getMappingIDOrbitalElements();

    ASSERT_EQ(map.size(), 1);
    ASSERT_EQ(map.count(_expectedID_2), 1);

    auto actualKepler = map[_expectedID_2];
    ASSERT_EQ(actualKepler, _expectedKepler_2);
}

TEST_F(TLEReaderTest, readTLE_03_Test) {
    TLEReader tleReader{"resources/TLEReaderTest03.txt"};

    auto map = tleReader.getMappingIDOrbitalElements();

    ASSERT_EQ(map.size(), 2);

    ASSERT_EQ(map.count(_expectedID_1), 1);

    auto actualKepler = map[_expectedID_1];
    ASSERT_EQ(actualKepler, _expectedKepler_1);

    ASSERT_EQ(map.count(_expectedID_2), 1);

    actualKepler = map[_expectedID_2];
    ASSERT_EQ(actualKepler, _expectedKepler_2);
}

TEST_F(TLEReaderTest, readTLE_04_Test) {
    TLEReader tleReader{"resources/TLEReaderTest04.txt"};

    auto map = tleReader.getMappingIDOrbitalElements();

    ASSERT_EQ(map.size(), 1);

    ASSERT_EQ(map.count(_expectedID_3), 1);

    auto actualKepler = map[_expectedID_3];
    ASSERT_EQ(actualKepler, _expectedKepler_3);
}

