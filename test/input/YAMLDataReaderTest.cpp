#include "gtest/gtest.h"

#include <array>
#include "breakupModel/input/YAMLDataReader.h"
#include "breakupModel/model/OrbitalElementsFactory.h"


class YAMLReaderTest : public ::testing::Test {

protected:
    virtual void SetUp() {
        SatelliteBuilder satelliteBuilder{};
        OrbitalElementsFactory factory{};

        _sat[0] = satelliteBuilder
                .setID(1)
                .setName("Iridium 33")
                .setSatType(SatType::SPACECRAFT)
                .setMass(700)
                .setVelocity({1, 1, 1})
                .getResult();

        _sat[1] = satelliteBuilder
                .setID(2)
                .setName("Kosmos 2251")
                .setSatType(SatType::SPACECRAFT)
                .setMass(900)
                .setVelocity({2, 2, 2})
                .getResult();

        _sat[2] = satelliteBuilder
                .setID(3)
                .setName("1962-060B")
                .setSatType(SatType::ROCKET_BODY)
                .setMassByArea(3.5428)
                .setVelocity({2, 2, 2})
                .getResult();

        _sat[3] = satelliteBuilder
                .setID(4)
                .setName("UFO")
                .setSatType(SatType::UNKNOWN)
                .setMassByArea(3333.33)
                .setVelocity({11, 22, 33})
                .getResult();
        _sat[4] = satelliteBuilder
                .setID(25544)
                .setName("ISS")
                .setSatType(SatType::SPACECRAFT)
                .setMassByArea(399.05)
                .setOrbitalElements(factory.createFromOnlyRadians({6798505.86, 0.0002215, 0.9013735469,
                                                                   4.724103630312, 2.237100203348, 0.2405604761},
                                                                  OrbitalAnomalyType::MEAN))
                .getResult();
    }

    std::array<Satellite, 5> _sat{};
};

/**
 * Checks if the file "resources/YamlDataReaderTest01.yaml" is correctly read in
 */
TEST_F(YAMLReaderTest, getSatelliteCollectionTest01) {
    YAMLDataReader yamlReader{"resources/YamlDataReaderTest01.yaml"};
    auto satelliteCollection = yamlReader.getSatelliteCollection();

    ASSERT_EQ(satelliteCollection.size(), 5);

    auto expectedSatellites = _sat.begin();
    for (auto &sat : satelliteCollection) {
        //== in Satellite only checks for ID equality
        ASSERT_EQ(sat, *expectedSatellites);

        //Checking the other attributes for equality
        ASSERT_EQ(sat.getName(), expectedSatellites->getName());
        ASSERT_EQ(sat.getSatType(), expectedSatellites->getSatType());
        ASSERT_NEAR(sat.getMass(), expectedSatellites->getMass(), 0.0001);
        ASSERT_NEAR(sat.getArea(), expectedSatellites->getArea(), 0.0001);
        ASSERT_NEAR(sat.getCharacteristicLength(), expectedSatellites->getCharacteristicLength(), 0.0001);
        ASSERT_EQ(sat.getVelocity(), expectedSatellites->getVelocity());
        ASSERT_EQ(sat.getPosition(), expectedSatellites->getPosition());

        ++expectedSatellites;
    }
}

/**
 * Kepler Elements not correctly given
 */
TEST_F(YAMLReaderTest, getSatelliteCollectionTest02) {
    YAMLDataReader yamlReader{"resources/YamlDataReaderTest02.yaml"};

    ASSERT_THROW(yamlReader.getSatelliteCollection(), std::runtime_error) << "This should throw a "
                                                                             "runtime exception cause eccentricity and "
                                                                             "inclination are missing!";
}

/**
 * Kepler Elements are given with two anomalies, eccentric anomaly should be preferred
 */
TEST_F(YAMLReaderTest, getSatelliteCollectionTest03) {
    YAMLDataReader yamlReader{"resources/YamlDataReaderTest03.yaml"};
    auto satelliteCollection = yamlReader.getSatelliteCollection();

    ASSERT_EQ(satelliteCollection.size(), 5);

    auto expectedISS = _sat[4];
    auto actualISS = satelliteCollection[4];

    //Checking the other attributes for equality
    ASSERT_EQ(actualISS.getName(), expectedISS.getName());
    ASSERT_EQ(actualISS.getSatType(), expectedISS.getSatType());
    ASSERT_NEAR(actualISS.getMass(), expectedISS.getMass(), 0.0001);
    ASSERT_NEAR(actualISS.getArea(), expectedISS.getArea(), 0.0001);
    ASSERT_NEAR(actualISS.getCharacteristicLength(), expectedISS.getCharacteristicLength(), 0.0001);
    //Kepler Elements should differ!
    ASSERT_NE(actualISS.getVelocity(), expectedISS.getVelocity());
    ASSERT_NE(actualISS.getPosition(), expectedISS.getPosition());

}

/**
 * Kepler Elements are given with two anomalies, eccentric anomaly should be preferred
 */
TEST_F(YAMLReaderTest, getSatelliteCollectionTest04) {
    YAMLDataReader yamlReader{"resources/YamlDataReaderTest04.yaml"};

    ASSERT_THROW(yamlReader.getSatelliteCollection(), std::runtime_error) << "This should throw a "
                                                                             "runtime exception cause there was no "
                                                                             "satellites tag inside the YAML file";

}
